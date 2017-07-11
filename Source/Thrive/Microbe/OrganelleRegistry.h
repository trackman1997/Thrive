// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "CompoundRegistry.h"
#include "Common/JsonRegistry.h"
#include "UObject/NoExportTypes.h"
#include "OrganelleRegistry.generated.h"

//! Data for a registered organelle
USTRUCT(BlueprintType)
struct FOrganelleType : public  FJsonRegistryType {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString Gene;

	UPROPERTY(BlueprintReadOnly)
	float Mass = 0.1;

	UPROPERTY(BlueprintReadOnly)
	int MPCost = 0;

	UPROPERTY()
	TMap<FName, float> Composition;
};

/**
 * 
 */
UCLASS(BlueprintType)
class THRIVE_API UOrganelleRegistry : public UObject, public TJsonRegistry<FOrganelleType> {
	GENERATED_BODY()

public:
	void Init(UCompoundRegistry* ACompoundRegistry);

	virtual FOrganelleType GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData);

	//! Loads default organelles
	//! \todo Add a blueprint overrideable method for adding extra things
	void LoadDefaultOrganelles(FString Path);

	//! Registers a new organelle with the specified properties
	//! \returns True if succeeded. False if the name is already in use
	UFUNCTION(BlueprintCallable)
	bool RegisterOrganelleType(FOrganelleType &Properties);

	//! Returns the properties of an organelle. Or InvalidOrganelle if not found
	//! \note The returned value should NOT be changed
	UFUNCTION(BlueprintCallable)
	FOrganelleType const& GetOrganelleData(const FName &OrganelleName) const;

	UFUNCTION(BlueprintCallable)
	TArray<FOrganelleType> const& GetRegisteredOrganelles() const;

private:
	UCompoundRegistry* CompoundRegistry = nullptr;
};
