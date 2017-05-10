// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "MembraneComponent.h"



UMembraneComponent::UMembraneComponent() : Super(){

    SetUsesPhysics(true);
    //bUsesPhysics = true;

    // Maybe call SetCollisionProfileName(TEXT("Pawn")); here
    SetCollisionProfileName(TEXT("Pawn"));
    //SetCollisionProfileName(TEXT("BlockAll"));

    //SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

    
    //SetMaterial(0, );
}

// ------------------------------------ //
void UMembraneComponent::BeginPlay(){

    Super::BeginPlay();

    if(OrganelleContainerComponent){

        
        
    } else {

        TArray<FGeneratedTriangle> box;

        FGeometryGenerator::CreateBoxGeometry(60, box);
        
        SetGeneratedMeshTriangles(box);
        UpdateCollision();
    }
}


