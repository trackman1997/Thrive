// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"

#include "CompoundCloud.h"
#include "MicrobeGameModeBase.h"

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
    AMicrobeGameModeBase* GameMode = Cast<AMicrobeGameModeBase>(GetWorld()->GetAuthGameMode());

    if(!GameMode){

        LOG_FATAL("Compound cloud initialize couldn't get the gamemode");
        return;
    }

    auto* Registry = GameMode->GetCompoundRegistry();
    
    check(DynMaterial);

    // These set the compound colours and create the layers //
    if(Compound1 != ECompoundID::Invalid)
        _SetupCompound(Compound1, Registry);

    if(Compound2 != ECompoundID::Invalid)
        _SetupCompound(Compound2, Registry);

    if(Compound3 != ECompoundID::Invalid)
        _SetupCompound(Compound3, Registry);

    if(Compound4 != ECompoundID::Invalid)
        _SetupCompound(Compound4, Registry);
}

void ACompoundCloud::_SetupCompound(ECompoundID Compound, UCompoundRegistry* Registry){

    CompoundLayers.Add(FLayerData(Compound, TextureSize, TextureSize, CloudScale));
    
    check(CompoundLayers.Num() <= 4);

    DynMaterial->SetVectorParameterValue(FName(*FString::Printf(TEXT("CompoundColour%d"),
                CompoundLayers.Num())), Registry->GetColour(Compound));
}

void ACompoundCloud::OnConstruction(const FTransform& Transform){

    Super::OnConstruction(Transform);

    if(!CloudMaterialBase){

        LOG_ERROR("CompoundCloud didn't find the base material");
        return;
    }
    
    DynMaterial = UMaterialInstanceDynamic::Create(CloudMaterialBase, this);

    // When using single channel
    // DensityMaterial = UTexture2D::CreateTransient(TextureSize, TextureSize, PF_G8);
    // We use all 4 channels to pack 4 compounds into one cloud
    DensityMaterial = UTexture2D::CreateTransient(TextureSize, TextureSize, PF_R8G8B8A8);
    
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

void ACompoundCloud::SetShared(const std::shared_ptr<FSharedCloudData> &Data){

    SharedCloudData = Data;
}

// Called when the game starts or when spawned
void ACompoundCloud::BeginPlay()
{
	Super::BeginPlay();
}
// ------------------------------------ //
void ACompoundCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


    if(bIsDirty)
        UpdateTexture();
}
// ------------------------------------ //
bool ACompoundCloud::AddCloud(ECompoundID Compound, float Density, float X, float Y){

    uint32_t ArrayX, ArrayY;
    if(!GetTargetCoordinates(X, Y, ArrayX, ArrayY))
        return false;

    auto* Layer = GetLayerForCompound(Compound);

    if(!Layer)
        return false;

    Layer->Density->GetData()[ArrayX][ArrayY] += Density;
    return true;
}

int ACompoundCloud::TakeCompound(ECompoundID Compound, float X, float Y, float Rate){

    uint32_t ArrayX, ArrayY;
    if(!GetTargetCoordinates(X, Y, ArrayX, ArrayY))
        return 0;

    auto* Layer = GetLayerForCompound(Compound);

    if(!Layer)
        return 0;

    const auto CanTake = static_cast<int>(Layer->Density->GetData()[ArrayX][ArrayY] * Rate);

    if(CanTake < 1)
        return 0;

    Layer->Density->GetData()[ArrayX][ArrayY] -= CanTake;
    checkSlow(Layer->Density->GetData()[ArrayX][ArrayY] >= 0.f);
    return CanTake;
}

bool ACompoundCloud::AmountAvailable(TArray<std::tuple<ECompoundID, int>> &Result, float X,
    float Y)
{
    uint32_t ArrayX, ArrayY;
    if(!GetTargetCoordinates(X, Y, ArrayX, ArrayY))
        return false;

    Result.Empty(CompoundLayers.Num());
    
    for(const auto& Layer : CompoundLayers){

        const auto Available = Layer.Density->GetData()[ArrayX][ArrayY];

        if(Available > 0)
            Result.Add(std::make_tuple(Layer.ID, Available));
    }
    
    return true;
}
// ------------------------------------ //
ACompoundCloud::FLayerData* ACompoundCloud::GetLayerForCompound(ECompoundID Compound){

    for(auto& Layer : CompoundLayers){

        if(Layer.ID == Compound)
            return &Layer;
    }
    
    return nullptr;
}
// ------------------------------------ //
void ACompoundCloud::UpdateTexture(){

    bIsDirty = false;

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
// ------------------------------------ //


// ------------------------------------ //
// FLayerData
ACompoundCloud::FLayerData::FLayerData(ECompoundID InID, int32_t Width, int32_t Height,
    float ResolutionFactor) :
    ID(InID), GridSize(ResolutionFactor)
{
    
    
}

ACompoundCloud::FLayerData::FLayerData(FLayerData&& MoveFrom) :
    ID(MoveFrom.ID), Blob1(std::move(MoveFrom.Blob1)),
    Blob2(std::move(MoveFrom.Blob2)), GridSize(MoveFrom.GridSize)
{
    MoveFrom.Density = nullptr;
    MoveFrom.LastDensity = nullptr;
    
    Density = &Blob1;
    LastDensity = &Blob2;
}

