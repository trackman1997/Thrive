#!/usr/bin/env ruby
# coding: utf-8
# Setup script for Thrive.

require 'fileutils'
require 'os'

require_relative 'linux_setup/RubyCommon.rb'

def checkRunFolder(suggestedfolder)

  if !File.exist? "Thrive.uproject"

    error "This script needs to be ran inside the thrive folder"
    exit 2
  end

  thirdPartyFolder = File.join Dir.pwd, "ThirdPartyBuild"

  FileUtils.mkdir_p thirdPartyFolder
  FileUtils.mkdir_p File.join Dir.pwd, "ThirdParty"
  
  thirdPartyFolder
  
end

def projectFolder(baseDir)

  File.expand_path File.join(baseDir, "../")
  
end

def parseExtraArgs

  if ARGV.length > 1

    onError("Unrecognized command line options.\n" +
            "Expected only username in addition to other arguments. Got: #{ARGV.join(' ')}")
    
  end
  
  $svnUser = ARGV[0]
  ARGV.shift
  
end

require_relative 'linux_setup/RubySetupSystem.rb'

if !$svnUser
  $svnUser = "thrive"
end

WantedURL = "https://#{$svnUser}@boostslair.com/svn/Thrive_Content"

clangPath = which("clang")

if clangPath == nil

  onError("Clang is not installed, or in path")
  
end

# FFMPEG setup
# TODO: disable unused codecs to save space
ffmpeg = FFMPEG.new(
  :version => "release/3.3",
  :installPath => File.join(ProjectDir, "ThirdParty/ffmpeg"),
  :noInstallSudo => true,
  # :enablePIC => true,
  :buildShared => true,
  :enableSmall => true,
  # :noStrip => true,
  :extraOptions => [
    "--disable-postproc", "--disable-avdevice",
    "--disable-avfilter",
    "--enable-rpath",
    
    # Same compiler as ue4
    "--cc=clang", "--cxx=clang",
    "--disable-network",

    # Can't be bothered to check which specific things we need so some of these disables
    # are disabled
    #"--disable-everything",
    #"--disable-demuxers",
    "--disable-encoders",
    "--disable-decoders",
    "--disable-hwaccels",
    "--disable-muxers",
    #"--disable-parsers",
    #"--disable-protocols",
    "--disable-indevs",
    "--disable-outdevs",
    "--disable-filters",

    # Wanted things
    # These aren't enough so all the demuxers protocols and parsers are enabled
    "--enable-decoder=aac", "--enable-decoder=mpeg4", "--enable-decoder=h264",
    "--enable-parser=h264", "--enable-parser=aac", "--enable-parser=mpeg4video",
    "--enable-demuxer=h264", "--enable-demuxer=aac", "--enable-demuxer=m4v",

    
    # Disable all the external libraries
    "--disable-bzlib", "--disable-iconv",
    "--disable-libxcb",
    "--disable-lzma", "--disable-sdl2", "--disable-xlib", "--disable-zlib",
    "--disable-audiotoolbox", "--disable-cuda", "--disable-cuvid",
    "--disable-nvenc", "--disable-vaapi", "--disable-vdpau",
    "--disable-videotoolbox"
  ]
)

installer = Installer.new(
  Array[
    ffmpeg
  ])

installer.run

puts ""

info "Copying dynamic libraries to project bin directories"

copyCount = 0

if OS.linux?

  bindir = File.join ProjectDir, "Binaries", "Linux"

  FileUtils.mkdir_p bindir
  
  libdir = File.join ProjectDir, "ThirdParty", "ffmpeg", "lib"
  Dir.foreach(libdir) do |libname|

    if libname =~ /lib.*\.so.*/i

      FileUtils.cp File.join(libdir, libname), bindir
      copyCount += 1
    end
  end 
  
elsif OS.mac?
  
  abort("todo")
  
elsif OS.windows?

  bindir = File.join ProjectDir, "Binaries", "Linux"

  FileUtils.mkdir_p bindir
  
  libdir = File.join ProjectDir, "ThirdParty", "ffmpeg", "lib"
  Dir.foreach(libdir) do |libname|

    if libname =~ /.*\.dll.*/i

      FileUtils.cp File.join(libdir, libname), bindir
      copyCount += 1
    end
  end
  
else
  onError("unkown os")
end


success "Copied #{copyCount} libraries/links to the bin folder"
puts ""
exit
info "Thrive folder setup"

puts "Using svn user: #{$svnUser}"

info "Running Thrive folder setup"

if not File.exist? "Content"

  info "Content folder doesn't exist, checking it out"
  
  system "svn co --force #{WantedURL} --username #{$svnUser} ."
  onError "Failed to clone repository" if $?.exitstatus > 0

  success "Checkout succeeded"
  
end

success "Content folder exists"

# Make sure svn username is correct
match = `svn info`.strip.match(/.*URL:\s?(.*thrive\S+).*/i)

abort("'svn info' unable to find URL with regex") if !match

currenturl = match.captures[0]

if currenturl != WantedURL

  info "SVN url is not the target url with username, #{currenturl} != #{WantedURL}"

  systemChecked "svn relocate #{WantedURL}"

  success "svn URL updated"
  
end

info "svn URL is correct"


# Prints files that SVN should be set to ignore
def printSVNIgnoreList

  ignoreList = []

  Dir.foreach(".") do |fname|

    if !fname.match(/.*Content.*/i) &&
       fname != "." && fname != ".." && fname != ".svn"
       
       ignoreList.push fname
      
    end
  end

  puts "Suggested SVN ignore list:"
  puts ignoreList.join"\n"
  
end

#printSVNIgnoreList


info "Making sure submodules are fine"

if !File.exists? "Plugins/VictoryPlugin/VictoryBPLibrary.uplugin"

  info "Submodules aren't initialized..."

  system "git submodule init"
  onError "Failed to initialize submodules" if $?.exitstatus > 0

  success "git submodules have been initialized"
  
end

system "git submodule update"
if $?.exitstatus > 0

  warning "Failed to update submodules, this may or may not be a problem"
  
end

# Create staging folder
FileUtils.mkdir_p "Staging"
  

success "Done. Open 'Thrive.uproject' in Unreal Engine 4 editor to start working."

exit 0

