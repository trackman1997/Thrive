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
require_relative 'linux_setup/ThriveCommon.rb'

if !$svnUser
  $svnUser = "thrive"
end

WantedURL = "https://#{$svnUser}@boostslair.com/svn/Thrive_Content"

# Use clang when not on windows to compile ffmpeg (we could probably 
# skip this and use the system default just fine, and portaudio doesn't use clang)
if !OS.windows?
  clangPath = which("clang")

  if clangPath == nil

    onError("Clang is not installed, or in path")
  end
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
    if !OS.windows? then 
      "--enable-rpath"
    else
      ""
    end,
    
    # Same compiler as ue4 (if not on windows)
    if !OS.windows? then 
      ["--cc=clang", "--cxx=clang"]
    else
      ""
    end,
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
  ].flatten
)

portaudio = PortAudio.new(
  :version => "396fe4b6699ae929d3a685b3ef8a7e97396139a4",
  :installPath => File.join(ProjectDir, "ThirdParty/portaudio"),
  :noInstallSudo => true,
  :noOSS => true
)

installer = Installer.new(
  Array[
    ffmpeg,
    portaudio
  ])

installer.run

puts ""

info "Copying dynamic libraries to project bin directories"

if OS.linux?

  bindir = File.join ProjectDir, "Binaries", "Linux"
  
elsif OS.mac?
  
  onError("todo")
  
elsif OS.windows?

  # TODO 32 bit
  bindir = File.join ProjectDir, "Binaries", "Win64"
  
else
  onError("unkown os")
end

copyCount = ffmpegCopyLibs bindir
copyCount += portaudioCopyLibs bindir

success "Copied #{copyCount} libraries/links to the bin folder"
puts ""

sourceBuildFile = File.read(File.join ProjectDir, "Source", "Thrive", "Thrive.Build.Source.cs")

sourceBuildFile.gsub! /\(@0@\)/, "This file was configured by SetupThrive.rb, " +
                             "Don't edit Thrive.Build.cs"

if OS.linux?

  ffmpegLibsList = [
    "libavcodec.so", "libavformat.so", "libavutil.so",
    "libswresample.so", "libswscale.so"
  ]

  ffmpegLibsList.map!{|i| File.join ProjectDir, "ThirdParty", "ffmpeg", "lib", i}

  # Find final targets of the ffmpeg libs
  if ffmpegLibsList.length < 1
    onError "FFMPEG libraries (non-symlink) weren't found and setup cannot continue"
  end

  # Do clue why ue4 strips everythin after last '.' and thus we need
  # to append .so to make stuff work
  libSetupCode = ffmpegLibsList.map{|l|
    "PublicAdditionalLibraries.Add(\"#{l}\");"
  }.join("\n")
  
  sourceBuildFile.gsub! /\(@1@\)/, "#{libSetupCode}"
  
elsif OS.mac?

  sourceBuildFile.gsub! /\(@1@\)/, "//Not used on this platform"
  onError("todo")
  
elsif OS.windows?

  sourceBuildFile.gsub! /\(@1@\)/, "//Not used on this platform"
  
else
  onError("unkown os for build script setup")
end


target = File.join ProjectDir, "Source", "Thrive", "Thrive.Build.cs"
info "Writing thrive module file: #{target}"
File.open(target, 'w') { |file| file.write(sourceBuildFile) }

success "Wrote build file '#{target}'"

puts ""

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

onError("'svn info' unable to find URL with regex") if !match

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
  
puts ""
success "Done. Open 'Thrive.uproject' in Unreal Engine 4 editor to start working."

exit 0

