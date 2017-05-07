#!/usr/bin/env ruby

require 'find'


if File.exists? "Source"
  runFolder = "."
else
  runFolder = "../"
end

Dir.chdir(runFolder){

  file_paths = []
  Find.find('.') do |path|
    file_paths << path if (not path =~ /.*\/Build\/.*/) and
      (not path =~ /.*\/Staging\/.*/) and
      (not path =~ /.*\/cmake_build\/.*/) and ((path =~ /.*\.h/) or
                                         (path =~ /.*\.cpp/))
  end

  File.open("cscope.files", "w") do |f|
    f.puts(file_paths)
  end

  system "cscope -b"
}
