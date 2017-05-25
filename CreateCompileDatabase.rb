#!/usr/bin/env ruby
# Sanitizes compile commands for use with clang tools
require 'json'
require 'fileutils'

require_relative 'linux_setup/RubyCommon.rb'

FILE_CHECK = Regexp.new(/thrive/i).freeze
THRIVE_SOURCE = Regexp.new(/Thrive.cpp/).freeze

# Run cmake to make this work
FileUtils.mkdir_p "cmake_build"

Dir.chdir("cmake_build") do

  # TODO: use a timestamp here to detect when CMakeLists.txt has been changed
  info "Updating cmake compile database"
  systemChecked("cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
  
end

abort "cmake not ran" if
  !File.exists? "cmake_build"

Dir.chdir("cmake_build"){

  abort "cmake not run with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON" if
    !File.exists? "compile_commands.json"

  
  
  json = JSON.parse File.read "compile_commands.json"

  puts "#{json.length} compile commands for files"

  #puts json

  # Remove excess ones
  filtered = json.select{|o|

    if o.include?("file") && FILE_CHECK =~ o['file']
      true
    else
      false
    end
  }

  puts "#{filtered.length} remaining after filtering"

  if true
    # Only keep one from the base folder
    filtered = json.select{|o|

      if THRIVE_SOURCE =~ o['file']
        true
      else
        false
      end
    }

    puts "#{filtered.length} remaining after keeping only one"
  end
  
  # Add c++11 flag: --std=c++11
  filtered.map!{|o|

    if !o["command"].include?("std=c++")

      o["command"] += " --std=c++11"
      #puts "adding c++11 flag"
    end

    o
  }

  File.open("compile_commands.json","w") do |f|
    f.write filtered.to_json
  end
  
}

Dir.chdir("Source"){

  FileUtils.ln_sf '../cmake_build/compile_commands.json', 'compile_commands.json'
  
}


