// Copyright (C) 2013-2017  Revolutionary Games

#pragma once


#include "OrganelleContainerComponent.h"

#include "RuntimeMeshComponent.h"

#include "MembraneComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Thrive), meta=(BlueprintSpawnableComponent))
class THRIVE_API UMembraneComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

    UMembraneComponent();

    /**
    * Builds the membrane based on the set UOrganelleContainerComponent
    */
    void BeginPlay() override;

    //! Creates the mesh, needs to be called in actor OnConstruction
    void CreateMembraneMesh(URuntimeMeshComponent* GeometryReceiver);
	
 protected:

    UPROPERTY(BlueprintReadOnly)
    UOrganelleContainerComponent* OrganelleContainerComponent = nullptr;
    
};
