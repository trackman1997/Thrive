// Copyright (C) 2013-2017  Revolutionary Games
using UnrealBuildTool;
using System.IO;

public class Thrive : ModuleRules
{
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath( Path.Combine( ModuleDirectory, "../../ThirdParty/" ) ); }
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
	}

    public bool LoadFFMPEG(TargetInfo Target)
    {
        bool isLibrarySupported = false;
 
        // Create ffmpeg Path 
        string ffmpegPath = Path.Combine(ThirdPartyPath, "ffmpeg");
 
        // Get Library Path 
        // bool isdebug = Target.Configuration == UnrealTargetConfiguration.Debug &&
        //     BuildConfiguration.bDebugBuildsActuallyUseDebugCRT;
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            
            //LibPath = Path.Combine(ffmpegPath, "Libraries", "Win64");
            isLibrarySupported = true;
            
        }
        else if (Target.Platform == UnrealTargetPlatform.Win32)
        {
            
            //LibPath = Path.Combine(ffmpegPath, "Libraries", "Win32");
            isLibrarySupported = true;
            
        } else if(Target.Platform == UnrealTargetPlatform.Linux){

            isLibrarySupported = true;

            // Add Dependencies
            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libavcodec/libavcodec.a"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libavdevice/libavdevice.a"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libavfilter/libavfilter.a"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libavformat/libavformat.a"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libavresample/libavresample.a"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libavutil/libavutil.a"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libswresample/libswresample.a"));

            PublicAdditionalLibraries.Add(Path.Combine(ffmpegPath, 
                    "libswscale/libswscale.a"));


            // System.Console.WriteLine("Path: " + Path.Combine(ffmpegPath, "libavcodec"));
            
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libavcodec"));
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libavdevice"));
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libavfilter"));
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libavformat"));
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libavresample"));
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libavutil"));
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libswresample"));
            // PublicLibraryPaths.Add(Path.Combine(ffmpegPath, "libswscale"));
                 
            
        } else
        {
            string Err = string.Format("{0} unsupported platform with ffmpeg, building {1}.",
                Target.Platform.ToString(), this.ToString());
            
            System.Console.WriteLine(Err);
        }
 
        if (isLibrarySupported)
        {
            //Add Include path 
            PublicIncludePaths.AddRange(new string[] { ffmpegPath });
        }
 
        Definitions.Add(string.Format("WITH_FFMPEG={0}", isLibrarySupported ? 1 : 0));
 
        return isLibrarySupported;
    }
}

