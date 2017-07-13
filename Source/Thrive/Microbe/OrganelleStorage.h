// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Microbe/OrganelleComponentFunctionality.h"
#include "OrganelleStorage.generated.h"

/**
 * 
 */
UCLASS()
class THRIVE_API UOrganelleStorage : public UOrganelleComponentFunctionality
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Init(float AStorageCapacity);

	//UFUNCTION(BlueprintCallable)
	//virtual void OnAddedToMicrobe(ACellBase* AttachedMicrobe, UOrganelleComponent* Organelle);

	UFUNCTION(BlueprintCallable)
	virtual void OnRemovedFromMicrobe();

private:
	float StorageCapacity = 10.0;
};
