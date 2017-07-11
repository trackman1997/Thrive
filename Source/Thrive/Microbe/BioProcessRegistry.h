// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "UObject/NoExportTypes.h"
#include "Common/JsonRegistry.h"
#include "BioProcessRegistry.generated.h"

//! Data for a registered bio process type.
USTRUCT(BlueprintType)
struct FBioProcessType : public FJsonRegistryType {

	GENERATED_BODY()

	//! Map with the input compounds of this bio process, and its quantities.
	UPROPERTY()
	TMap<FName, int> Inputs;

	//! Map with the output compounds of this bio process, and its quantities.
	UPROPERTY()
	TMap<FName, int> Outputs;
};

/**
 * 
 */
UCLASS(BlueprintType)
class THRIVE_API UBioProcessRegistry : public UObject, public TJsonRegistry<FBioProcessType>
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void Init(UCompoundRegistry* ACompoundRegistry);

	virtual FBioProcessType GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData);

	//! Loads default bio processes
	//! \todo Add a blueprint overrideable method for adding extra things
	UFUNCTION(BlueprintCallable)
	void LoadDefaultBioProcesses(FString Path);
	
	//! Registers a new bio proces with the specified properties
	//! \note The ID should be left empty as it will be overwritten
	//! \returns True if succeeded. False if the name is already in use
	UFUNCTION(BlueprintCallable)
	bool RegisterBioProcessType(FBioProcessType &Properties);

	//! Returns the properties of a bio process type. Or InvalidBioProcess if not found
	//! \note The returned value should NOT be changed
	UFUNCTION(BlueprintCallable)
	FBioProcessType const& GetBioProcessData(const FName &BioProcessName) const;

private:
	UCompoundRegistry* CompoundRegistry = nullptr;
};
