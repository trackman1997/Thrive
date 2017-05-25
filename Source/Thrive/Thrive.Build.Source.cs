// Copyright (C) 2013-2017  Revolutionary Games
// (@0@)

using UnrealBuildTool;
using System.IO;

public class Thrive : ModuleRules
{
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(
                Path.Combine( ModuleDirectory, "../../ThirdParty/" ) );
        }
    }
    
	public Thrive(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                
                // RTC requires these
                "RuntimeMeshComponent",
                "RHI",
                "RenderCore",
                "ShaderCore",

                // Victory requires these.
                // We would probably require Slate and UMG things anyway 
                "VictoryBPLibrary",
                "UMG",
                "Slate",
                "SlateCore",
                "ImageWrapper"
            });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        LoadFFMPEG(Target);

        LoadPortAudio(Target);
	}

    public bool LoadFFMPEG(TargetInfo Target)
    {
        bool isLibrarySupported = false;
 
        // Create ffmpeg Path 
        string ffmpegPath = Path.Combine(ThirdPartyPath, "ffmpeg");

        string LibPath = "";
        
        // Get Library Path 
        // bool isdebug = Target.Configuration == UnrealTargetConfiguration.Debug &&
        //     BuildConfiguration.bDebugBuildsActuallyUseDebugCRT;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            
            //LibPath = Path.Combine(ffmpegPath, "Libraries", "Win64");
            
            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libavcodec.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libavformat.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libavutil.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libswresample.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libswscale.lib"));

            LibPath = Path.Combine(ffmpegPath, "lib");
            
            isLibrarySupported = true;
            
        }
        else if (Target.Platform == UnrealTargetPlatform.Win32)
        {
            
            //LibPath = Path.Combine(ffmpegPath, "Libraries", "Win32");
            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libavcodec.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libavformat.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libavutil.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libswresample.lib"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, "lib", 
                    "libswscale.lib"));

            LibPath = Path.Combine(ffmpegPath, "lib");
            
            isLibrarySupported = true;
            
        } else if(Target.Platform == UnrealTargetPlatform.Linux){

            isLibrarySupported = true;

            (@1@);
            
            LibPath = Path.Combine(ffmpegPath, "lib");

            
        } else
        {
            string Err = string.Format("{0} unsupported platform with ffmpeg, building {1}.",
                Target.Platform.ToString(), this.ToString());
            
            System.Console.WriteLine(Err);
        }
 
        if (isLibrarySupported)
        {
            //Add Include path 
            PublicIncludePaths.AddRange(new string[] { Path.Combine(
                        ffmpegPath, "include") });

            PublicLibraryPaths.Add(LibPath);
            //System.Console.WriteLine(LibPath);
        }
 
        Definitions.Add(string.Format("WITH_FFMPEG={0}", isLibrarySupported ? 1 : 0));
 
        return isLibrarySupported;
    }


    public bool LoadPortAudio(TargetInfo Target)
    {
        bool isLibrarySupported = false;
 
        // Create portaudio Path 
        string portaudioPath = Path.Combine(ThirdPartyPath, "portaudio");

        string LibPath = "";
        
        // Get Library Path 
        // bool isdebug = Target.Configuration == UnrealTargetConfiguration.Debug &&
        //     BuildConfiguration.bDebugBuildsActuallyUseDebugCRT;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            
            //LibPath = Path.Combine(portaudioPath, "Libraries", "Win64");
            
            PublicAdditionalLibraries.Add(Path.Combine(portaudioPath, "lib", 
                    "portaudio.lib"));

            LibPath = Path.Combine(portaudioPath, "lib");
            
            isLibrarySupported = true;
            
        }
        else if (Target.Platform == UnrealTargetPlatform.Win32)
        {
            PublicAdditionalLibraries.Add(Path.Combine(portaudioPath, "lib", 
                    "portaudio.lib"));
            
            LibPath = Path.Combine(portaudioPath, "lib");
            
            isLibrarySupported = true;
            
        } else if(Target.Platform == UnrealTargetPlatform.Linux){

            isLibrarySupported = true;

            PublicAdditionalLibraries.Add(Path.Combine(portaudioPath, "lib", 
                    "libportaudio.so"));

            LibPath = Path.Combine(portaudioPath, "lib");
            
            isLibrarySupported = true;
            
        } else
        {
            string Err = string.Format("{0} unsupported platform with portaudio, building {1}.",
                Target.Platform.ToString(), this.ToString());
            
            System.Console.WriteLine(Err);
        }
 
        if (isLibrarySupported)
        {
            //Add Include path 
            PublicIncludePaths.AddRange(new string[] { Path.Combine(
                        portaudioPath, "include") });

            PublicLibraryPaths.Add(LibPath);
            //System.Console.WriteLine(LibPath);
        }
 
        Definitions.Add(string.Format("WITH_PORTAUDIO={0}", isLibrarySupported ? 1 : 0));
 
        return isLibrarySupported;
    }
}

