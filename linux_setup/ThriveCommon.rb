# Common thrive methods here
# Requires the RubySetupsystem to have been loaded before
require_relative 'RubyCommon.rb'

def thrivestandardCopyHelper(targetFolder, libdir)

  copyCount = 0
  FileUtils.mkdir_p targetFolder
  
  if OS.linux?

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



def ffmpegCopyLibs(targetFolder)

  thrivestandardCopyHelper(targetFolder,
                           if OS.linux?
                             File.join ProjectDir, "ThirdParty", "ffmpeg", "lib"
                           elsif OS.mac?
                             File.join ProjectDir, "ThirdParty", "ffmpeg", "lib"
                           elsif OS.windows?
                             File.join ProjectDir, "ThirdParty", "ffmpeg", "lib"
                           else
                             onError("unkown os")
                           end
                          )
end


def portaudioCopyLibs(targetFolder)
  
  thrivestandardCopyHelper(targetFolder,
                           if OS.linux?
                             File.join ProjectDir, "ThirdParty", "portaudio", "lib"
                           elsif OS.mac?
                             File.join ProjectDir, "ThirdParty", "portaudio", "lib"
                           elsif OS.windows?
                             File.join ProjectDir, "ThirdParty", "portaudio", "lib"
                           else
                             onError("unkown os")
                           end
                          )
end

