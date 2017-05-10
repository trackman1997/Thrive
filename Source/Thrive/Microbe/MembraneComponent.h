// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "CommonComponents/ThriveGeneratedMeshComponent.h"

#include "OrganelleContainerComponent.h"

#include "MembraneComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Thrive), meta=(BlueprintSpawnableComponent))
class THRIVE_API UMembraneComponent : public UThriveGeneratedMeshComponent
{
	GENERATED_BODY()
	
public:

    UMembraneComponent();

    /**
    * Builds the membrane based on the set UOrganelleContainerComponent
    */
    void BeginPlay() override;
	
 protected:

    UPROPERTY(BlueprintReadOnly)
    UOrganelleContainerComponent* OrganelleContainerComponent = nullptr;
    
};
