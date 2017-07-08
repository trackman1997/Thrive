// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "MicrobeCommon.h"
#include "UObject/NoExportTypes.h"
#include "BiomeRegistry.generated.h"

//! Data for a registered biome type.
USTRUCT(BlueprintType)
struct FBiomeType {

	GENERATED_BODY()

	//! ID of this biome.
	//! If invalid the wanted biome type didn't exist
	UPROPERTY(BlueprintReadOnly)
	EBiomeID ID = EBiomeID::Invalid;

	//! Used to search by name.
	UPROPERTY(BlueprintReadOnly)
	FName InternalName;

	//! Used by the GUI and stuff.
	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;

	//! Map with the compounds present on this biome, and its quantities.
	UPROPERTY()
	TMap<ECompoundID, int> Compounds;
};


/**
 * 
 */
UCLASS()
class THRIVE_API UBiomeRegistry : public UObject
{
	GENERATED_BODY()

public:
	UBiomeRegistry();

	//! Loads default bio processes
	//! \todo Add a blueprint overrideable method for adding extra things
	UFUNCTION(BlueprintCallable)
	void LoadDefaultBiomes(UCompoundRegistry* CompoundRegistry);

	//! Returns biome's id based on the name
	//! \note This should be called once and the result stored for performance reasons
	UFUNCTION(BlueprintCallable)
	EBiomeID GetBiomeByName(const FName &BiomeName) const;

	//! Registers a new biome with the specified properties
	//! \note The ID should be left empty as it will be overwritten
	//! \returns True if succeeded. False if the name is already in use
	UFUNCTION(BlueprintCallable)
	bool RegisterBiomeType(FBiomeType &Properties);

	//! Returns the properties of a biome type. Or InvalidBiome if not found
	//! \note The returned value should NOT be changed
	UFUNCTION(BlueprintCallable)
	FBiomeType const& GetBiomeData(EBiomeID ID) const;

	//! Registered biomes.
	UPROPERTY()
	TArray<FBiomeType> RegisteredBiomes;

private:
	//! For returning references to invalid biomes,
	UPROPERTY()
	FBiomeType InvalidBiome;
};
