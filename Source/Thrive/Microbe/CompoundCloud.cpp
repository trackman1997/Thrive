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

void ACompoundCloud::Initialize(ECompoundID Compound1, ECompoundID Compound2,
    ECompoundID Compound3, ECompoundID Compound4)
{

    // Set the compound colours //
    check(DynMaterial);
    
    // DynMaterial->SetVectorParameter("CompoundColour1", Registry->GetColour(Compound1));
    
}

void ACompoundCloud::OnConstruction(const FTransform& Transform){

    Super::OnConstruction(Transform);

    if(!CloudMaterialBase){

        LOG_ERROR("CompoundCloud didn't find the base material");
        return;
    }
    
    UMaterialInstanceDynamic* DynMaterial =
        UMaterialInstanceDynamic::Create(CloudMaterialBase, this);

    // When using single channel
    // DensityMaterial = UTexture2D::CreateTransient(1024, 1024, PF_G8);
    // We use all 4 channels to pack 4 compounds into one cloud
    DensityMaterial = UTexture2D::CreateTransient(1024, 1024, PF_R8G8B8A8);
    
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

    // Note: the channels are in order 3rd compound, 2nd compound, 1st compound, 4th compound
    // so when filling take that into account

    // This means that the first value is the 3rd compound then the
    // second and only then the first compound.
    
    // Also changing the pixel format seemed to not affect this order
    // so even though the format is PF_R8G8B8A8 the material acts as if the channels are
    // B G R A

    if(!DensityMaterial || !DensityMaterial->Resource){

        LOG_ERROR("Texture not initialized in update texture");
        return;
    }

    constexpr uint8_t BytesPerPixel = 4;
    //constexpr uint8_t BytesPerPixel = 1;

    const auto Width = DensityMaterial->GetSizeX();
    const auto Height = DensityMaterial->GetSizeY();

    const auto TotalBytes = Width * Height * BytesPerPixel;
    const auto Pitch = Width * BytesPerPixel;

    uint8_t* TextureData = reinterpret_cast<uint8_t*>(FMemory::Malloc(TotalBytes, 4));

    FMemory::Memzero(TextureData, TotalBytes);

    // for(size_t x = 0; x < Width; ++x){
    //     for(size_t y = 0; y < Height; ++y){

    //         TextureData[(y * Pitch) + (x * BytesPerPixel)] = x % 255;
    //     }
    // }

    

    //FMemory::Memset(TextureData, 255, TotalBytes);

    for(size_t i = 0; i < TotalBytes / 2; i += 4)
        TextureData[i] = 255;

    for(size_t i = (TotalBytes / 2) + 3; i < TotalBytes; i += 4)
        TextureData[i] = 255;
    
    FTextureHelper::UpdateTextureRegions(DensityMaterial, 0, 1,
        new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height), 
        Pitch, BytesPerPixel, TextureData, true);
}

