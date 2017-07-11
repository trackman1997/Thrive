// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "CompoundRegistry.h"
#include "Common/JsonRegistry.h"
#include "UObject/NoExportTypes.h"
#include "BiomeRegistry.generated.h"

//! Data for a registered biome type.
USTRUCT(BlueprintType)
struct FBiomeType : public FJsonRegistryType {
	GENERATED_BODY()

public:
	//! Map with the compounds present on this biome, and its quantities.
	UPROPERTY()
	TMap<FName, int> Compounds;
};

/**
 * 
 */
UCLASS(BlueprintType)
class THRIVE_API UBiomeRegistry : public UObject, public TJsonRegistry<FBiomeType> {
	GENERATED_BODY()

public:
	void Init(UCompoundRegistry* ACompoundRegistry);

	//! Loads default bio processes
	//! \todo Add a blueprint overrideable method for adding extra things
	UFUNCTION(BlueprintCallable)
	void LoadDefaultBiomes(FString Path);

	virtual FBiomeType GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData);

	//! Registers a new biome with the specified properties
	//! \returns True if succeeded. False if the name is already in use
	UFUNCTION(BlueprintCallable)
	bool RegisterBiomeType(FBiomeType &Properties);

	//! Returns the properties of a biome type. Or InvalidBiome if not found
	//! \note The returned value should NOT be changed
	UFUNCTION(BlueprintCallable)
	FBiomeType const& GetBiomeData(const FName &BiomeName) const;

private:
	UCompoundRegistry* CompoundRegistry = nullptr;
};
