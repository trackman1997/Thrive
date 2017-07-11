// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Actor.h"

#include "MicrobeCommon.h"
#include <tuple>
#include <memory>

#include "CompoundCloud.generated.h"

class UCompoundRegistry;
class FSharedCloudData;

UCLASS()
class THRIVE_API ACompoundCloud : public AActor
{
	GENERATED_BODY()

    //! Contains data for single compound type's layer
    class FLayerData final{
    public:
        FLayerData(FName InName, int32_t InWidth, int32_t InHeight,
            float ResolutionFactor);

        FLayerData(FLayerData&& MoveFrom);

        FLayerData(const FLayerData &Other) = delete;
        FLayerData& operator=(const FLayerData &Other) = delete;

        //! Performs a single update step
        void Update(float DeltaTime, const FSharedCloudData &Velocities);

        const FName Name;

        // Colour can be retrieved from UCompoundRegistry
        
        
        //! The 2D array that contains the current compound clouds and those from last frame.
        TArray<TArray<float>> Density;
        TArray<TArray<float>> LastDensity;

        //! Controls the "distance" between cells in the grid
        const float GridSize;

        const int32_t Width;
        const int32_t Height;

    private:
        
        void Diffuse(float DiffRate, float DeltaTime);
        
        void Advect(float DeltaTime, const FSharedCloudData &Velocities);
    };
        
public:	
	// Sets default values for this actor's properties
	ACompoundCloud();

    //! Initializes this cloud for compounds
    //! All compound types that will be used during the lifetime of this object need to be
    //! specified here
    void Initialize(FName Compound1, FName Compound2,
        FName Compound3, FName Compound4);

    //! Sets up the materials to look nice in the editor
    void OnConstruction(const FTransform& Transform) override;

    //! Sets up the shared data for this cloud. Needs to be called after spawning
    void SetShared(const std::shared_ptr<FSharedCloudData> &Data);

    // Called every frame
	virtual void Tick(float DeltaTime) override;


    //! Adds compound to position.
    //! The coordinates are between 0 and 1
    //! \returns True if the coordinates where valid and the compound was added
    bool AddCloud(FName Compound, float Density, float X, float Y);

    //! Takes specified compound at location.
    //! \param Rate Controls how much is taken. 1 means all at position. Use less than 1
    //! \returns The amount taken
    int TakeCompound(FName Compound, float X, float Y, float Rate);

    //! Returns in the Result array how much different compounds are available at position
    //! \returns True if the coordinates where valid and result was filled. False if invalid
    bool AmountAvailable(TArray<std::tuple<FName, int>> &Result, float X, float Y);
    

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    //! Returns the layer matching id or null
    FLayerData* GetLayerForCompound(FName Compound);

    //! Updates the density texture
    void UpdateTexture();

private:
    //! Helper for Initialize
    void _SetupCompound(FName Compound, UCompoundRegistry* Registry);

    //! Calculate final grid position from 0-1.f range coordinates
    //! \returns False if out of range
    FORCEINLINE bool GetTargetCoordinates(float InX, float InY, uint32_t &X, uint32_t &Y)
        const
    {
        // This isn't needed as the output cannot be negative so it's enough to check that it's
        // less than TextureSize
        // if(InX < 0.f || InY < 0.0f)
        //     return false;
        X = static_cast<uint32_t>(FMath::RoundToInt(InX * TextureSize));
        Y = static_cast<uint32_t>(FMath::RoundToInt(InY * TextureSize));

        if(Y >= TextureSize || X >= TextureSize)
            return false;
        
        return true;
    }

    //! Copies layer data to memory. Helper for UpdateTexture
    void CopyLayerDataToMemory(uint8_t* Target, size_t TargetSize,
        const TArray<TArray<float>> &Source, int BPP) const;
    

protected:

    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UMaterialInterface* CloudMaterialBase = nullptr;

    // Textures for the density
    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UTexture2D* DensityMaterial = nullptr;

    //! Material instance with the right colours and our DensityMaterial applied
    UPROPERTY()
    UMaterialInstanceDynamic* DynMaterial = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UStaticMeshComponent* PlaneMesh;

    TArray<FLayerData> CompoundLayers;

    //! Causes the texture to be updated on next tick. Set to true when compounds have moved
    bool bIsDirty = true;

    // Performance / Accuracy tweaking //
    // The size of the texture
    uint32_t TextureSize = COMPOUND_CLOUD_SIMULATE_SIZE;

    //! The speed of the flow, needs to be adjusted based on the TextureSize and
    //! the size of this cloud in the world
    float CloudScale = 1.0f;


    std::shared_ptr<FSharedCloudData> SharedCloudData;

};
