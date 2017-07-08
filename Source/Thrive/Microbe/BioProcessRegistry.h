// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "MicrobeCommon.h"
#include "UObject/NoExportTypes.h"
#include "BioProcessRegistry.generated.h"

//! Data for a registered bio process type.
USTRUCT(BlueprintType)
struct FBioProcessType {

	GENERATED_BODY()

	//! ID of this bio process.
	//! If invalid the wanted bio process type didn't exist
	UPROPERTY(BlueprintReadOnly)
	EBioProcessID ID = EBioProcessID::Invalid;

	//! Used to search by name.
	UPROPERTY(BlueprintReadOnly)
	FName InternalName;

	//! Used by the GUI and stuff.
	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;

	//! Map with the input compounds of this bio process, and its quantities.
	UPROPERTY()
	TMap<ECompoundID, int> Inputs;

	//! Map with the output compounds of this bio process, and its quantities.
	UPROPERTY()
	TMap<ECompoundID, int> Outputs;
};

/**
 * 
 */
UCLASS(BlueprintType)
class THRIVE_API UBioProcessRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	UBioProcessRegistry();

	//! Loads default bio processes
	//! \todo Add a blueprint overrideable method for adding extra things
	UFUNCTION(BlueprintCallable)
	void LoadDefaultBioProcesses(UCompoundRegistry* CompoundRegistry);

	//! Returns bio process's id based on the name
	//! \note This should be called once and the result stored for performance reasons
	UFUNCTION(BlueprintCallable)
	EBioProcessID GetBioProcessByName(const FName &BioProcessName) const;
	
	//! Registers a new bio proces with the specified properties
	//! \note The ID should be left empty as it will be overwritten
	//! \returns True if succeeded. False if the name is already in use
	UFUNCTION(BlueprintCallable)
	bool RegisterBioProcessType(FBioProcessType &Properties);

	//! Returns the properties of a bio process type. Or InvalidBioProcess if not found
	//! \note The returned value should NOT be changed
	UFUNCTION(BlueprintCallable)
	FBioProcessType const& GetBioProcessData(EBioProcessID ID) const;

	//! Registered processes
	UPROPERTY()
	TArray<FBioProcessType> RegisteredBioProcesses;

private:

	//! For returning references to invalid bio processes
	UPROPERTY()
	FBioProcessType InvalidBioProcess;
};
