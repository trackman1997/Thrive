// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Components/ActorComponent.h"
#include "Spawner.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spawnRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spawnDensity;

	float spawnRadiusSqr;

	float getSpawnFrequency();

private:
	/*
	spawnFrequency is on average how many entities should pass the first condition
	in each spawn cycle(See _doSpawnCycle).spawnRadius ^ 2 * 4 is used because
	that is the area of the square region where entities attempt to spawn.
	*/
	float spawnFrequency;
};
