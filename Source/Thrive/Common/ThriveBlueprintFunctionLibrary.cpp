// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "ThriveBlueprintFunctionLibrary.h"


FString UThriveBlueprintFunctionLibrary::GetProjectVersion(){

    FString ProjectVersion;
    
    GConfig->GetString(
        TEXT("/Script/EngineSettings.GeneralProjectSettings"),
        TEXT("ProjectVersion"),
        ProjectVersion,
        GGameIni
    );
    
    return ProjectVersion;
}

FString UThriveBlueprintFunctionLibrary::GetGameContentDir(){

    return FPaths::GameContentDir();
}

