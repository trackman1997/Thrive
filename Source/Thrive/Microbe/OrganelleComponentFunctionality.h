// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

//#include "CellBase.h"
//#include "OrganelleComponent.h"
#include "UObject/NoExportTypes.h"
#include "OrganelleComponentFunctionality.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class THRIVE_API UOrganelleComponentFunctionality : public UObject
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable)
	//virtual void OnAddedToMicrobe(ACellBase* AttachedMicrobe, UOrganelleComponent* Organelle);
	
	UFUNCTION(BlueprintCallable)
	virtual void OnRemovedFromMicrobe();

	UFUNCTION(BlueprintCallable)
	virtual void OnUpdate(float DeltaTime, float OrganelleHealth);

//protected:
	//UPROPERTY()
	//ACellBase* Microbe = nullptr;
};
