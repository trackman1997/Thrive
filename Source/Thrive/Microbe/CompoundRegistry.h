// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "UObject/NoExportTypes.h"
#include "Common/JsonRegistry.h"
#include "CompoundRegistry.generated.h"


//! Data for a registered compound type
USTRUCT(BlueprintType)
struct FCompoundType : public  FJsonRegistryType {

    GENERATED_BODY()

public:
	//! The colour of this compound, in clouds and in the GUI.
    UPROPERTY(BlueprintReadOnly)
    FLinearColor Colour = FColor(0, 0, 0, 1);

	//! How much space does this compound occupy per unit.
	UPROPERTY(BlueprintReadOnly)
	float Volume;

	//! Whether this compound has any functionality other
	//! than a stepping stone to other compounds in the
	//! process chain.
	UPROPERTY(BlueprintReadOnly)
	bool bUseful;

	//! Whether this compound can form clouds.
	UPROPERTY(BlueprintReadOnly)
	bool bCloud;
};

/**
 * 
 */
UCLASS(BlueprintType)
class THRIVE_API UCompoundRegistry : public UObject, public TJsonRegistry<FCompoundType>
{
	GENERATED_BODY()

public:
	virtual FCompoundType GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData);

    //! Loads default compounds
    //! \todo Add a blueprint overrideable method for adding extra things
    void LoadDefaultCompounds(FString Path);

    //! Registers a new compound with the specified properties
    //! \note The ID should be left empty as it will be overwritten
    //! \returns True if succeeded. False if the name is already in use
    UFUNCTION(BlueprintCallable)
    bool RegisterCompoundType(FCompoundType &Properties);


    //! Returns the properties of a compound type. Or InvalidCompound if not found
    //! \note The returned value should NOT be changed
    UFUNCTION(BlueprintCallable)
    FCompoundType const& GetCompoundData(const FName &CompoundName) const;

	UFUNCTION(BlueprintCallable)
	TArray<FCompoundType> const& GetRegisteredCompounds() const;

	UFUNCTION(BlueprintCallable)
	bool IsValidCompound(const FName &CompoundName) const {
		return GetCompoundData(CompoundName).InternalName != InvalidType.InternalName;
	}
};
