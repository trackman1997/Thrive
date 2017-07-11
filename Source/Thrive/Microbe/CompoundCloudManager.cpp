// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundCloudManager.h"
#include "MicrobeGameModeBase.h"

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

    AMicrobeGameModeBase* GameMode = Cast<AMicrobeGameModeBase>(GetWorld()->GetAuthGameMode());

    if(!GameMode){

        LOG_FATAL("Compound cloud manager couldn't get the gamemode");
        return;
    }

    auto* Registry = GameMode->GetCompoundRegistry();

    TestCloud->Initialize("oxygen", "invalid", "invalid", "invalid");

    TestCloud->AddCloud("oxygen", 2500.f, 0.53f, 0.53f);
    TestCloud->AddCloud("oxygen", 2500.f, 0.54f, 0.54f);
    TestCloud->AddCloud("oxygen", 2500.f, 0.55f, 0.55f);
    TestCloud->AddCloud("oxygen", 2500.f, 0.56f, 0.56f);
    
}



// Called every frame
void ACompoundCloudManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ------------------------------------ //
// FSharedCloudData
FSharedCloudData::FSharedCloudData(uint32_t Width, uint32_t Height){
    
    FPerlinNoise Noise(Width * COMPOUND_CLOUD_NOISE_SCALE,
        Height * COMPOUND_CLOUD_NOISE_SCALE);

    //PerlinNoise fieldPotential;
    
    Velocity.Reserve(Width);
    Velocity.SetNum(Width);

    for(uint32_t i = 0; i < Width; ++i){

        TArray<std::tuple<float, float>> SecondDimension;
        SecondDimension.Reserve(Height);
        SecondDimension.SetNum(Height);
        Velocity[i] = std::move(SecondDimension);
    }

    // This scale needs to be Width - 1 or less to not access outside the generated
    // perlin noise map
    float nxScale = COMPOUND_CLOUD_NOISE_SCALE;
	float nyScale = nxScale * float(Width) / float(Height);

    // Taken from the old CreateVelocitYField with slight modifications
	float X0, Y0, X1, Y1, N0, N1, NX, NY;

	for (unsigned X = 0; X < Width; X++)
	{
		for (unsigned Y = 0; Y < Height; Y++)
		{
            // If we used the Width and Height as scale this probably
            // outputs the same thing
			X0 = (float(X - 1) / float(Width)) * Width * nxScale;
			Y0 = (float(Y - 1) / float(Height)) * Height * nyScale;
			X1 = (float(X + 1) / float(Width)) * Width * nxScale;
			Y1 = (float(Y + 1) / float(Height)) * Height * nyScale;

            // Integer only approach
			// X0 = X != 0 ? X - 1 : Width - 1;
			// Y0 = Y != 0 ? Y - 1 : Height - 1;
			// X1 = X + 1 < Width ? X + 1 : 0;
			// Y1 = Y + 1 < Height ? Y + 1 : 0;

			N0 = Noise.Perlin(X0, Y0);
			N1 = Noise.Perlin(X1, Y0);
			NY = N0 - N1;
			N0 = Noise.Perlin(X0, Y0);
			N1 = Noise.Perlin(X0, Y1);
			NX = N1 - N0;


            // N0 = fieldPotential.noise(X0, Y0, 0);
			// N1 = fieldPotential.noise(X1, Y0, 0);
			// NY = N0 - N1;
			// N0 = fieldPotential.noise(X0, Y0, 0);
			// N1 = fieldPotential.noise(X0, Y1, 0);
            // NX = N1 - N0;

            Velocity[X][Y] = std::make_tuple(NX/2, NY/2);
		}
	}
}



