// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "MicrobePlayerControllerBase.h"

#include "Sound/SoundWave.h"
#include "VictoryBPLibrary/Public/VictoryBPFunctionLibrary.h"


AMicrobePlayerControllerBase::AMicrobePlayerControllerBase() :
    Super()
{
    
}

// ------------------------------------ //
void AMicrobePlayerControllerBase::BeginPlay(){

    Super::BeginPlay();


    FString testSound = FPaths::GameContentDir() +
        "/ExtraContent/Music/microbe-theme-3.ogg";

    if(FPaths::FileExists(testSound) && IsLocalPlayerController()){

        DISPLAY_MESSAGE(TEXT("playing music track"));

        USoundWave* musicTrack = UVictoryBPFunctionLibrary::GetSoundWaveFromFile(testSound);

        // Make sure the SoundWave Object is Valid
        if(!musicTrack){

            LOG_ERROR("Failed to create new SoundWave Object");
            return;
        }

        ClientPlaySound(musicTrack);
        // TODO: add timer to play another track or get an event of
        // sound finished but then we would need to create a sound
        // player and attach it to our pawn
        
        //AudioComponent->SetSound(CompressedSoundWaveRef);
        //AudioComponent->Play();
    }
}

