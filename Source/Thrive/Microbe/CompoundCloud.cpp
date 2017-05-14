// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundCloud.h"
#include "Generation/TextureHelper.h"


// Sets default values
ACompoundCloud::ACompoundCloud()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    static ConstructorHelpers::FObjectFinder<UMaterialInterface> CloudMaterialFinder(
        TEXT("/Game/Microbe/CompoundClouds/CompoundCloudMaterial"));

    if(CloudMaterialFinder.Object)
        CloudMaterialBase = CloudMaterialFinder.Object;


    PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root Plane"));
    RootComponent = PlaneMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshFinder(
        TEXT("/Game/Common/Models/UnitPlane"));

    if(PlaneMeshFinder.Succeeded()){
        
        PlaneMesh->SetStaticMesh(PlaneMeshFinder.Object);
        PlaneMesh->SetWorldScale3D(FVector(5.12));

        PlaneMesh->SetMaterial(0, nullptr);
    }
}

void ACompoundCloud::OnConstruction(const FTransform& Transform){

    Super::OnConstruction(Transform);

    if(!CloudMaterialBase){

        LOG_ERROR("CompoundCloud didn't find the base material");
        return;
    }
    
    UMaterialInstanceDynamic* DynMaterial =
        UMaterialInstanceDynamic::Create(CloudMaterialBase, this);

    
    DensityMaterial = UTexture2D::CreateTransient(1024, 1024, PF_R8_UINT);

    if(!DensityMaterial){

        LOG_ERROR("Failed to create cloud density texture");
        return;
    }

    //Make sure it won't be compressed
    DensityMaterial->CompressionSettings =
        TextureCompressionSettings::TC_VectorDisplacementmap;
    
    //Turn off Gamma-correction
    DensityMaterial->SRGB = 0;

    // Allocate the texture RHI resource things
    //Update the texture with new variable values.
    DensityMaterial->UpdateResource();

    DynMaterial->SetTextureParameterValue("CloudDensityData", DensityMaterial);

    PlaneMesh->SetMaterial(0, DynMaterial);
}

// Called when the game starts or when spawned
void ACompoundCloud::BeginPlay()
{
	Super::BeginPlay();

    UpdateTexture();
}

// Called every frame
void ACompoundCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACompoundCloud::UpdateTexture(){

    constexpr uint8_t BytesPerPixel = 1;

    const auto Width = DensityMaterial->GetSizeX();
    const auto Height = DensityMaterial->GetSizeY();

    const auto TotalBytes = Width * Height * BytesPerPixel;
    const auto Pitch = Width * BytesPerPixel;

    void* TextureData = FMemory::Malloc(TotalBytes, 4);

    FMemory::Memzero(TextureData, TotalBytes);
    
    FTextureHelper::UpdateTextureRegions(DensityMaterial, 0, 1,
        new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height), 
        Pitch, BytesPerPixel, reinterpret_cast<uint8_t*>(TextureData), true);
}

