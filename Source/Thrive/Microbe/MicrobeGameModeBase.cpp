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
