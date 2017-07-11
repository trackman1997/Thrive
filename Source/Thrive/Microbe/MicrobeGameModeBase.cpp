// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "MicrobeGameModeBase.h"

AMicrobeGameModeBase::AMicrobeGameModeBase() :
    Super()
{
    PrimaryActorTick.bCanEverTick = true;
    
}


void AMicrobeGameModeBase::TickActor(float DeltaTime, enum ELevelTick TickType,
    FActorTickFunction& ThisTickFunction)
{
    Super::TickActor(DeltaTime, TickType, ThisTickFunction);

    
}

// ------------------------------------ //
void AMicrobeGameModeBase::InitGame(const FString& MapName, const FString& Options,
    FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
}

void AMicrobeGameModeBase::PreInitializeComponents(){

    Super::PreInitializeComponents();

    CompoundRegistry = NewObject<UCompoundRegistry>();
    check(CompoundRegistry);
    CompoundRegistry->LoadDefaultCompounds("GameData/MicrobeStage/Compounds.json");

	BioProcessRegistry = NewObject<UBioProcessRegistry>();
	check(BioProcessRegistry);
	BioProcessRegistry->Init(CompoundRegistry);
	BioProcessRegistry->LoadDefaultBioProcesses("GameData/MicrobeStage/Compounds.json");

	StartingCompoundsRegistry = NewObject<UStartingCompoundsRegistry>();
	check(StartingCompoundsRegistry);
	StartingCompoundsRegistry->LoadStartingCompounds(CompoundRegistry);

	BiomeRegistry = NewObject<UBiomeRegistry>();
	check(BiomeRegistry);
	BiomeRegistry->Init(CompoundRegistry);
	BiomeRegistry->LoadDefaultBiomes("GameData/MicrobeStage/Biomes.json");

	OrganelleRegistry = NewObject<UOrganelleRegistry>();
	check(OrganelleRegistry);
	OrganelleRegistry->Init(CompoundRegistry);
	OrganelleRegistry->LoadDefaultOrganelles("GameData/MicrobeStage/Organelles.json");
    

    // Create compounds spawn manager //
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Name = FName("World's_compound_manager");
    SpawnParameters.Owner = this;
    
    CompoundCloudManager = static_cast<ACompoundCloudManager*>(GetWorld()->SpawnActor(
            ACompoundCloudManager::StaticClass(), nullptr, SpawnParameters));

    check(CompoundCloudManager);
}
