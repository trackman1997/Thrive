// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "SpawnSystem.h"

// Sets default values
ASpawnSystem::ASpawnSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	timeSinceLastUpdate = 0.0;
	lastPlayerPosition = FVector2D(0, 0);
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
			

			for (auto const& spawner : spawners) {
				float spawnFrequency = spawner->getSpawnFrequency();
				unsigned numAttempts = FMath::Max(1, FMath::CeilToInt(spawnFrequency * 2));

				for (unsigned i = 0; i < numAttempts; i++) {
					if (FMath::RandRange((float)0.0, (float)1.0) < spawnFrequency / numAttempts) {
						float xDist = FMath::RandRange(-spawner->spawnRadius, spawner->spawnRadius);
						float yDist = FMath::RandRange(-spawner->spawnRadius, spawner->spawnRadius);

						FVector2D newPosition = FVector2D(xDist, yDist) + playerCoords;

						if (FVector2D::DistSquared(playerCoords, newPosition) <= spawner->spawnRadiusSqr &&
							FVector2D::DistSquared(lastPlayerPosition, newPosition) > spawner->spawnRadiusSqr) {
							FString debugtext = "Spawning: " + spawner->name + " in: " + newPosition.ToString();
							GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, debugtext);
						}
					}
				}
			}

			lastPlayerPosition = playerCoords;
		}
	}
}
