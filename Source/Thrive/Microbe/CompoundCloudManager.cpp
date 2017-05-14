// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundCloudManager.h"


// Sets default values
ACompoundCloudManager::ACompoundCloudManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACompoundCloudManager::BeginPlay()
{
	Super::BeginPlay();

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Name = FName("test cloud");
    SpawnParameters.Owner = this;

    FTransform Position(FVector(0, 0, 0));
    
    TestCloud = static_cast<ACompoundCloud*>(GetWorld()->SpawnActor(
            ACompoundCloud::StaticClass(), &Position, SpawnParameters));

    if(!TestCloud){

        LOG_FATAL("Failed to spawn test cloud");
    }
    
}

// Called every frame
void ACompoundCloudManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

