// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Actor.h"
#include "Spawner.h"

#include "SpawnSystem.generated.h"

// Minimum time (in seconds) between spawn cycles.
#define SPAWN_INTERVAL 0.1

UCLASS()
class THRIVE_API ASpawnSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnSystem();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// An array of references to the spawned actors.
	TArray<AActor*> spawnedActors;

	// Used to decide when to do the next spawn cycle.
	float timeSinceLastUpdate;

	// The player position on the last spawn cycle, used to calculate
	// where to spawn new entities.
	FVector2D lastPlayerPosition;
};
