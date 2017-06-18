// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Actor.h"

#include "CompoundCloud.h"
#include "Common/Noise.h"

#include <memory>

#include "CompoundCloudManager.generated.h"

class FSharedCloudData{
public:
    
    //! Fills XVelocity and YVelocity
    FSharedCloudData(uint32_t Width, uint32_t Height);

    // Perlin noise added to the movement //
    /// The velocity of the fluid.
    TArray<TArray<std::tuple<float, float>>> Velocity;
};


/**
* Handles spawning compound cloud actors and filling them with individual clouds.
* Will also allow individual cells to receive compounds
*/
UCLASS()
class THRIVE_API ACompoundCloudManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACompoundCloudManager();

    // Called every frame
	virtual void Tick(float DeltaTime) override;

    //! Sets up perlin noise things. Needs to run before BeginPlay
    void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:

    UPROPERTY()
    ACompoundCloud* TestCloud = nullptr;

    //! These are the clouds that are in use (alive near the player)
    //! these aren't individual clouds but regions where clouds exist
    //! Also each of these can only support 4 types of compounds so there
    //! maybe multiple overlapping clouds if more are needed
    UPROPERTY()
    TArray<ACompoundCloud*> AliveCloudRegions;


private:

    // Data shared between all clouds
    std::shared_ptr<FSharedCloudData> SharedCloudData;
    
	
};
