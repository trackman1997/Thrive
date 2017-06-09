// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Actor.h"

#include "CompoundCloud.h"

#include "CompoundCloudManager.generated.h"

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:

    UPROPERTY()
    ACompoundCloud* TestCloud = nullptr;

    //! These are the clouds that are in use (alive near the player)
    //! these aren't individual clouds but regions where clouds exist
    //! Also each of these can only support 4 types of compounds so there
    //! maybe multiple overlapping clouds if more are needed
    UPROPERTY()
    TArray<ACompoundCloud*> AliveCloudRegions;
	
};
