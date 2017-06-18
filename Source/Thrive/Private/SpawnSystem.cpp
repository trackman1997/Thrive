// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "SpawnSystem.h"


// Sets default values
ASpawnSystem::ASpawnSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	timeSinceLastUpdate = 0.0;
}

// Called when the game starts or when spawned
void ASpawnSystem::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Spawn system initialized!"));
}

// Called every frame
void ASpawnSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timeSinceLastUpdate += DeltaTime;
	if (timeSinceLastUpdate > SPAWN_INTERVAL) {
		timeSinceLastUpdate -= SPAWN_INTERVAL;

		// Getting the spawners attached to the system.
		TArray<USpawner*> spawners;
		GetComponents<USpawner>(spawners); // This should probably be done somewhere else.

		// Getting the player position.
		APawn* player = GetWorld()->GetFirstPlayerController()->GetPawnOrSpectator();
		if (player) {
			FVector2D playerCoords(player->GetActorLocation());
			FString debugtext = playerCoords.ToString();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, debugtext);

			for (auto const& spawner : spawners) {
				FString stype = spawner->name;
			}
		}
	}
}
