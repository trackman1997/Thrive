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

/*
For each entity type, spawns the appropriate number of entities within the spawn
radius of the player at its current location but outside of the spawn radius of the player
at its location during the previous spawn cycle.
*/
void ASpawnSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timeSinceLastUpdate += DeltaTime;
	if (timeSinceLastUpdate > SPAWN_INTERVAL) {
		timeSinceLastUpdate -= SPAWN_INTERVAL; // = 0?

		// Getting the spawners attached to the system.
		TArray<USpawner*> spawners;
		GetComponents<USpawner>(spawners); // This should probably be done somewhere else.

		// Getting the player position.
		APawn* player = GetWorld()->GetFirstPlayerController()->GetPawnOrSpectator();
		if (player) {
			FVector2D playerCoords(player->GetActorLocation());

			// Despawning faraway actors.
			for (auto& actorRef : spawnedActors) {
				if (actorRef.actor == nullptr) actorRef.markedForRemoval = true;
				else {
					FVector2D actorPosition(actorRef.actor->GetActorLocation());
					if (FVector2D::DistSquared(actorPosition, playerCoords) > actorRef.spawner->spawnRadiusSqr) {
						actorRef.markedForRemoval = true;
						actorRef.spawner->onDespawn(actorRef.actor);
					}
				}
			}

			// Removing the despawned actor references.
			spawnedActors.RemoveAll([](const SpawnedActorReference actorRef) {
				return actorRef.markedForRemoval;
			});

			// Spawning new actors.
			for (auto const& spawner : spawners) {
				/*
				To actually spawn a given entity for a given attempt, two conditions should be met.
				The first condition is a random chance that adjusts the spawn frequency to the approprate
				amount.The second condition is whether the entity will spawn in a valid position.
				It is checked when the first condition is met and a position
				for the entity has been decided.

				To allow more than one entity of each type to spawn per spawn cycle, the SpawnSystem
				attempts to spawn each given entity multiple times depending on the spawnFrequency.
				numAttempts stores how many times the SpawnSystem attempts to spawn the given entity.
				*/
				float spawnFrequency = spawner->getSpawnFrequency();
				unsigned numAttempts = FMath::Max(1, FMath::CeilToInt(spawnFrequency * 2));

				for (unsigned i = 0; i < numAttempts; i++) {
					/*
					First condition passed.Choose a location for the entity.

					A random location in the square of sidelength 2 * spawnRadius
					centered on the player is chosen.The corners
					of the square are outside the spawning region, but they
					will fail the second condition, so entities still only
					spawn within the spawning region.
					*/
					if (FMath::RandRange((float)0.0, (float)1.0) < spawnFrequency / numAttempts) {
						float xDist = FMath::RandRange(-spawner->spawnRadius, spawner->spawnRadius);
						float yDist = FMath::RandRange(-spawner->spawnRadius, spawner->spawnRadius);

						FVector2D newPosition = FVector2D(xDist, yDist) + playerCoords;

						if (FVector2D::DistSquared(playerCoords, newPosition) <= spawner->spawnRadiusSqr &&
							FVector2D::DistSquared(lastPlayerPosition, newPosition) > spawner->spawnRadiusSqr) {
							// Second condition passed. Spawn the entity.
							AActor* spawnedActor = nullptr;
							spawner->onSpawn(newPosition, spawnedActor);

							// Saves a reference to the spawned entity in order to despawn
							// it when it gets too far from the player.
							if (spawnedActor != nullptr) {
								spawnedActors.Emplace(spawnedActor, spawner);
							}
						}
					}
				}
			}
			// Updating the last player position.
			lastPlayerPosition = playerCoords;
		}
	}
}
