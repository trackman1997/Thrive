// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundCloudManager.h"

#include "MicrobeCommon.h"


ACompoundCloudManager::ACompoundCloudManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACompoundCloudManager::OnConstruction(const FTransform& Transform){

    SharedCloudData = std::make_shared<FSharedCloudData>(COMPOUND_CLOUD_SIMULATE_SIZE,
        COMPOUND_CLOUD_SIMULATE_SIZE);
}

// Called when the game starts or when spawned
void ACompoundCloudManager::BeginPlay()
{
	Super::BeginPlay();

    check(SharedCloudData);

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Name = FName("test cloud");
    SpawnParameters.Owner = this;

    FTransform Position(FVector(0, 0, 0));
    
    TestCloud = static_cast<ACompoundCloud*>(GetWorld()->SpawnActor(
            ACompoundCloud::StaticClass(), &Position, SpawnParameters));

    if(!TestCloud){

        LOG_FATAL("Failed to spawn test cloud");
    }

    TestCloud->SetShared(SharedCloudData);
}



// Called every frame
void ACompoundCloudManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ------------------------------------ //
// FSharedCloudData
FSharedCloudData::FSharedCloudData(uint32_t Width, uint32_t Height) :
    Noise(Width, Height)
{
    Velocity.SetNum(Width);

    for(uint32_t i = 0; i < Width; ++i){

        TArray<std::tuple<float, float>> SecondDimension;
        SecondDimension.SetNum(Height);
        Velocity[i] = std::move(SecondDimension);
    }

    // Taken from the old CreateVelocitYField with slight modifications
	float X0, Y0, X1, Y1, N0, N1, NX, NY;

	for (int X = 0; X < Width; X++)
	{
		for (int Y = 0; Y < Height; Y++)
		{
			X0 = X != 0 ? X - 1 : Width - 1;
			Y0 = Y != 0 ? Y - 1 : Height - 1;
			X1 = X + 1 < Width ? X + 1 : 0;
			Y1 = Y + 1 < Height ? Y + 1 : 0;

			N0 = Noise.Perlin(X0, Y0);
			N1 = Noise.Perlin(X1, Y0);
			NY = N0 - N1;
			N0 = Noise.Perlin(X0, Y0);
			N1 = Noise.Perlin(X0, Y1);
			NX = N1 - N0;

            Velocity[X][Y] = std::make_tuple(NX/2, NY/2);
		}
	}
}



