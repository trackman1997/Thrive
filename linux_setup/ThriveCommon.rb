# Common thrive methods here
# Requires the RubySetupsystem to have been loaded before
require_relative 'RubyCommon.rb'

def ffmpegCopyLibs(targetFolder)

  copyCount = 0
  
  if OS.linux?

    FileUtils.mkdir_p targetFolder
    
    libdir = File.join ProjectDir, "ThirdParty", "ffmpeg", "lib"
    Dir.foreach(libdir) do |libname|

      if libname =~ /lib.*\.so.*/i

        libFile = File.join(libdir, libname)

        copyPreserveSymlinks libFile, targetFolder


        copyCount += 1
        
      end
    end 
    
  elsif OS.mac?
    
    abort("todo")
    
  elsif OS.windows?

    FileUtils.mkdir_p targetFolder
    
    libdir = File.join ProjectDir, "ThirdParty", "ffmpeg", "lib"
    Dir.foreach(libdir) do |libname|

      if libname =~ /.*\.dll.*/i

        FileUtils.cp File.join(libdir, libname), targetFolder, :preserve => true
        copyCount += 1
      end
    end
    
  else
    onError("unkown os")
  end

  copyCount
end
