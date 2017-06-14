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

    CompoundRegistry->LoadDefaultCompounds();
    

    // Create compounds spawn manager //
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Name = FName("World's_compound_manager");
    SpawnParameters.Owner = this;
    
    CompoundCloudManager = static_cast<ACompoundCloudManager*>(GetWorld()->SpawnActor(
            ACompoundCloudManager::StaticClass(), nullptr, SpawnParameters));

    check(CompoundCloudManager);
   
    
}
