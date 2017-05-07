// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/GameModeBase.h"
#include "MicrobeGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class THRIVE_API AMicrobeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
    
public:
        
    AMicrobeGameModeBase();
    
    //    //! Initialize default settings. Called during PreInitializeComponents or Reset
    //    virtual void InitGameState() override;
    
    //! Tick for gametime
    virtual void TickActor(float DeltaTime, enum ELevelTick TickType,
        FActorTickFunction& ThisTickFunction) override;
    
    //    //! Called when gameplay ends
    //    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    //    //! Respawn a player
    //    void RestartPlayer(AController* NewPlayer) override;

protected:
    
    //! Called when spawning a player for the first time
    //virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    //! Init hook that is called before PreInitializeComponents
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
        override;

    //! Player respawning allowed
    //bool PlayerCanRestart_Implementation(APlayerController* Player) override;

    
};
