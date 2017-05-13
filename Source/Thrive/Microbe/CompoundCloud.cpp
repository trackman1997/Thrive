// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundCloud.h"


// Sets default values
ACompoundCloud::ACompoundCloud()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    static ConstructorHelpers::FObjectFinder<UMaterialInterface> CloudMaterialFinder(
        TEXT("/Game/Microbe/CompoundClouds/CompoundCloudMaterial"));

    if(CloudMaterialFinder.Object)
        CloudMaterialBase = CloudMaterialFinder.Object;
    
}

// Called when the game starts or when spawned
void ACompoundCloud::BeginPlay()
{
	Super::BeginPlay();


    if(!CloudMaterialBase){

        LOG_ERROR("CompoundCloud didn't find the base material");
        return;
    }
    
    UMaterialInstanceDynamic* DynMaterial =
        UMaterialInstanceDynamic::Create(MembraneMaterialBase, this);

        // Set colour //
        // TODO: this also needs to be called only when the colour changes
        DynMaterial->SetVectorParameterValue("MembraneColourTint", MembraneColourTint);
        
        GeometryReceiver->SetMaterial(0, DynMaterial);

        DensityMaterial = UTexture2D::CreateTransient(1024, 1024, PF_R8_UINT);
    
}

// Called every frame
void ACompoundCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACompoundCloud::UpdateTexture(){

    

}

