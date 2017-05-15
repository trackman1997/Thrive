// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ThriveBlueprintFunctionLibrary.generated.h"

/**
 * Extra utilities exposed to blueprints
 */
UCLASS()
class THRIVE_API UThriveBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintPure, meta = (DisplayName = "Project Version"),
        Category = "Thrive|Common")
    static FString GetProjectVersion();

    UFUNCTION(BlueprintPure, meta = (DisplayName = "Content Dir"),
        Category = "Thrive|Common")
    static FString GetGameContentDir();
	
};
