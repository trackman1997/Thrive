#!/usr/bin/env ruby
# Sanitizes compile commands for use with clang tools
require 'json'
require 'fileutils'

FILE_CHECK = Regexp.new(/thrive/i).freeze

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


