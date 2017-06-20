// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Components/ActorComponent.h"
#include "Spawner.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THRIVE_API USpawner : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawner();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** The name of the spawner type, used for debugging purposes */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString name;

	/** The distance from the player that the spawned actor can spawn or despawn. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spawnRadius;

	/** On average, the number of entities of the given type per square unit. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spawnDensity;

	// spawnRadius ** 2, stored for performance reasons.
	float spawnRadiusSqr;

	float getSpawnFrequency();

	/** The function called by the SpawnSystem to create the entity. It should have a two
	dimensional vector as a parameter, and it should return the new entity. */
	UFUNCTION(BlueprintImplementableEvent)
	void onSpawn(FVector2D location, AActor*& spawnedActor);

private:
	/*
	spawnFrequency is on average how many entities should pass the first condition
	in each spawn cycle(See _doSpawnCycle).spawnRadius ^ 2 * 4 is used because
	that is the area of the square region where entities attempt to spawn.
	*/
	float spawnFrequency;
};
