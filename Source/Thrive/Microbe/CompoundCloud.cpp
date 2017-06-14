// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"

#include "CompoundCloud.h"
#include "MicrobeGameModeBase.h"

#include "Generation/TextureHelper.h"


// Sets default values
ACompoundCloud::ACompoundCloud()
{
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
        PlaneMesh->SetWorldScale3D(FVector(20));

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

    if(!ensure(SharedCloudData))
        return;
    
    for(auto& Layer : CompoundLayers){
        Layer.Update(DeltaTime, *SharedCloudData);
        bIsDirty = true;
    }
    
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

    Layer->Density[ArrayX][ArrayY] += Density;
    return true;
}

int ACompoundCloud::TakeCompound(ECompoundID Compound, float X, float Y, float Rate){

    uint32_t ArrayX, ArrayY;
    if(!GetTargetCoordinates(X, Y, ArrayX, ArrayY))
        return 0;

    auto* Layer = GetLayerForCompound(Compound);

    if(!Layer)
        return 0;

    const auto CanTake = static_cast<int>(Layer->Density[ArrayX][ArrayY] * Rate);

    if(CanTake < 1)
        return 0;

    Layer->Density[ArrayX][ArrayY] -= CanTake;
    checkSlow(Layer->Density.[ArrayX][ArrayY] >= 0.f);
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

        const auto Available = Layer.Density[ArrayX][ArrayY];

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

    // This is probably not needed and could be removed
    // When all layers are used
    FMemory::Memzero(TextureData, TotalBytes);

    // for(size_t x = 0; x < Width; ++x){
    //     for(size_t y = 0; y < Height; ++y){

    //         TextureData[(y * Pitch) + (x * BytesPerPixel)] = x % 255;
    //     }
    // }

    // First layer
    int LayerNumber = 0;

    for(const auto& Layer : CompoundLayers){

        auto* StartPtr = TextureData + LayerNumber;

        // TODO: check could this be rolled in a single loop to improve cache hits
        CopyLayerDataToMemory(StartPtr, TotalBytes - LayerNumber, Layer.Density, 4);
        
        LayerNumber++;
    }
    
    FTextureHelper::UpdateTextureRegions(DensityMaterial, 0, 1,
        new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height), 
        Pitch, BytesPerPixel, TextureData, true);
}

void ACompoundCloud::CopyLayerDataToMemory(uint8_t* Target, size_t TargetSize,
    const TArray<TArray<float>> &Source, int BPP) const
{
    size_t i = 0;
    for(size_t X = 0; X < Source.Num(); ++X){
        for(size_t Y = 0; Y < Source[X].Num(); ++Y){

            int Intensity = static_cast<int>(Source[X][Y]);

            FMath::Clamp(Intensity, 0, 255);
            
            Target[i] = Intensity;
            i += BPP;
        }
    }

    check(i == TargetSize);
}

// ------------------------------------ //
// FLayerData
ACompoundCloud::FLayerData::FLayerData(ECompoundID InID, int32_t InWidth, int32_t InHeight,
    float ResolutionFactor) :
    ID(InID), GridSize(ResolutionFactor), Width(InWidth), Height(InHeight)
{
    Density.Reserve(Height);
    Density.SetNum(Width);

    for(int32_t i = 0; i < Width; ++i){
        TArray<float> SecondDimension;
        SecondDimension.Reserve(Height);
        SecondDimension.SetNum(Height);
        Density[i] = std::move(SecondDimension);
    }
    
    LastDensity = Density;

    // Sanity checks (can be changed to checkSlow once this is correctly running) //
    check(LastDensity.Num() == Density.Num());
    check(LastDensity[0].Num() == Density[0].Num());

    check(Density.GetSlack() == 0);
}

ACompoundCloud::FLayerData::FLayerData(FLayerData&& MoveFrom) :
    ID(MoveFrom.ID), Density(std::move(MoveFrom.Density)),
    LastDensity(std::move(MoveFrom.LastDensity)), GridSize(MoveFrom.GridSize),
    Width(MoveFrom.Width), Height(MoveFrom.Height)
{
}

// ------------------------------------ //
void ACompoundCloud::FLayerData::Update(float DeltaTime, const FSharedCloudData &Velocities){
    
    // Compound clouds move from area of high concentration to area of low.
    Diffuse(.01f, DeltaTime);
    
    // Move the compound clouds about the velocity field.
    Advect(DeltaTime, Velocities);
}

void ACompoundCloud::FLayerData::Diffuse(float DiffRate, float DeltaTime){

    //DeltaTime = 1;
    float A = DeltaTime * DiffRate / GridSize;

    for (int x = 1; x < Width-1; x++)
    {
        for (int y = 1; y < Height-1; y++)
        {
            LastDensity[x][y] = (Density[x][y] + A*(LastDensity[x - 1][y] +
                    LastDensity[x + 1][y] + LastDensity[x][y-1] + LastDensity[x][y+1])) /
                (1 + 4 * A);
        }
    }
    
}
        
void ACompoundCloud::FLayerData::Advect(float DeltaTime, const FSharedCloudData &Velocities){

    //DeltaTime = 1;
    
    for (int x = 0; x < Width; x++)
	{
		for (int y = 0; y < Height; y++)
		{
			Density[x][y] = 0;
		}
	}

    float dx, dy;
    int x0, x1, y0, y1;
    float s1, s0, t1, t0;
	for (int x = 1; x < Width-1; x++)
	{
		for (int y = 1; y < Height-1; y++)
		{
		    if (LastDensity[x][y] > 1) {
                dx = x + (DeltaTime * std::get<0>(Velocities.Velocity[x][y])) / GridSize;
                dy = y + (DeltaTime * std::get<1>(Velocities.Velocity[x][y])) / GridSize;

                if (dx < 0.5)
                    dx = 0.5;
                
                if (dx > Width - 1.5)
                    dx = Width - 1.5f;

                if (dy < 0.5)
                    dy = 0.5;
                
                if (dy > Height - 1.5)
                    dy = Height - 1.5f;

                x0 = static_cast<int>(dx);
                x1 = x0 + 1;
                y0 = static_cast<int>(dy);
                y1 = y0 + 1;

                s1 = dx - x0;
                s0 = 1 - s1;
                t1 = dy - y0;
                t0 = 1 - t1;

                Density[x0][y0] += LastDensity[x][y] * s0 * t0;
                Density[x0][y1] += LastDensity[x][y] * s0 * t1;
                Density[x1][y0] += LastDensity[x][y] * s1 * t0;
                Density[x1][y1] += LastDensity[x][y] * s1 * t1;
		    }
		}
	}
}
// ------------------------------------ //


