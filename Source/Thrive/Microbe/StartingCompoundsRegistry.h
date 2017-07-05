// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "CompoundRegistry.h"
#include "MicrobeCommon.h"
#include "UObject/NoExportTypes.h"
#include "StartingCompoundsRegistry.generated.h"

/**
 * 
 */
UCLASS()
class THRIVE_API UStartingCompoundsRegistry : public UObject
{
public:
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void LoadStartingCompounds(UCompoundRegistry* CompoundRegistry);

	UPROPERTY()
	TMap<ECompoundID, float> StartingCompounds;
};
