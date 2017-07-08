// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/GameModeBase.h"

#include "CompoundCloudManager.h"
#include "CompoundRegistry.h"
#include "BioProcessRegistry.h"
#include "StartingCompoundsRegistry.h"
#include "BiomeRegistry.h"

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


    //! Called immediately before gameplay begins. This sets up all spawn systems and stuff
    void PreInitializeComponents() override;


    ACompoundCloudManager* GetCompoundCloudManager(){
        return CompoundCloudManager;
    }

    UCompoundRegistry* GetCompoundRegistry(){
        return CompoundRegistry;
    }

	UBioProcessRegistry* GetBioProcessRegistry() {
		return BioProcessRegistry;
	}

	UStartingCompoundsRegistry* GetStartingCompoundsRegistry() {
		return StartingCompoundsRegistry;
	}

	UBiomeRegistry* GetBiomeRegistry() {
		return BiomeRegistry;
	}

protected:
    
    //! Called when spawning a player for the first time
    //virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    //! Init hook that is called before PreInitializeComponents
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
        override;

    //! Player respawning allowed
    //bool PlayerCanRestart_Implementation(APlayerController* Player) override;


protected:

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    ACompoundCloudManager* CompoundCloudManager = nullptr;

    //! \todo Expose a Subclass of this typed property to allow
    //! blueprint derived gamemode set the actual class to a blueprint
    //! one to easily allow setting the compounds
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    UCompoundRegistry* CompoundRegistry = nullptr;
    
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBioProcessRegistry* BioProcessRegistry = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStartingCompoundsRegistry* StartingCompoundsRegistry = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBiomeRegistry* BiomeRegistry = nullptr;
};
