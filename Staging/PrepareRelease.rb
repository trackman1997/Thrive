#!/usr/bin/env ruby
# coding: utf-8
# Helper for copying needed files into the release folders

require 'fileutils'
require 'os'

require_relative '../linux_setup/RubyCommon.rb'

def checkRunFolder(suggestedfolder)

  if !File.exist? "PrepareRelease.rb"

    error "This script needs to be ran inside the Staging folder"
    exit 2
  end

  suggestedfolder
  
end

def projectFolder(baseDir)

  File.expand_path File.join(baseDir, "../")
  
end

# def parseExtraArgs

#   if ARGV.length > 1

#     onError("Unrecognized command line options.\n" +
#             "Expected only username in addition to other arguments. Got: #{ARGV.join(' ')}")
    
#   end
  
#   $svnUser = ARGV[0]
#   ARGV.shift
  
# end

require_relative '../linux_setup/RubySetupSystem.rb'
require_relative '../linux_setup/ThriveCommon.rb'

targetFolder = File.join(CurrentDir, if
                          OS.linux? then
                          "LinuxNoEditor"
                        elsif
                          OS.mac? then
                          abort("todo")
                        elsif OS.windows? then
                          "Win64NoEditor"
                        else
                          onError("unkown os")
                         end
                        )

onError "Target folder '#{targetFolder}' doesn't exists" if !File.exists? targetFolder

info "Preparing folder '#{targetFolder}'"

FileUtils.cp File.join(ProjectDir, "LICENSE.txt"), targetFolder
FileUtils.cp File.join(ProjectDir, "UE4_EULA_DISCLAIMER.txt"), targetFolder

Dir.chdir(ProjectDir) do

  File.open(File.join(targetFolder, "revision.txt"), 'w') {
    |file| file.write("Package time: " + `date --iso-8601=seconds` + "\n\n" + `git log -n 1`)
  }
end

puts ""

info "Copying ffmpeg"

if OS.linux?

  bindir = File.join targetFolder, "Thrive", "Binaries", "Linux"
  
elsif OS.mac?
  
  abort("todo")
  
elsif OS.windows?

  # TODO 32 bit
  bindir = File.join targetFolder, "Thrive", "Binaries", "Win64"
  
else
  onError("unkown os")
end

puts "Target folder: #{bindir}"

copyCount = ffmpegCopyLibs bindir

success "Copied #{copyCount} libraries"

puts ""
info "Deleting log files"

FileUtils.rm_rf File.join(targetFolder, "Thrive", "Saved", "Logs")

info "Deleting config files (game specific)"

FileUtils.rm_rf File.join(targetFolder, "Thrive", "Saved", "Config")

error "TODO: stripping"

success "Done. Now run 'ZipUpRelease.rb' to create redistributable zips"

exit 0

