// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "VideoPlayerSoundWave.h"
#include "PlayerSoundWaveParent.h"

#include "ThriveVideoPlayer.h"

UVideoPlayerSoundWave::UVideoPlayerSoundWave(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCanProcessAsync = true;
}

DECLARE_CYCLE_STAT(TEXT("Thrive Video Player generate PCM data"), STAT_VideoPlayerGeneratePCMData, STATGROUP_Audio );

int32 UVideoPlayerSoundWave::GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded){

    SCOPE_CYCLE_COUNTER(STAT_VideoPlayerGeneratePCMData);

    //LOG_WARNING("UVideoPlayerSoundWave::GeneratePCMData is not done");

    // We don't use the queue sound method, AThriveVideoPlayer manages buffering the audio
    //int32 BytesGenerated = Super::GeneratePCMData(PCMData, SamplesNeeded);
    int32 BytesGenerated = 0;
	int32 BytesDesired = (SamplesNeeded * sizeof(int16_t)) - BytesGenerated;

    std::lock_guard<std::mutex> Lock(Parent->GetMutex());

    AThriveVideoPlayer* SoundSource = Parent->GetDataAccess(Lock);
    
    // Return if playback has been stopped //
    if(!SoundSource)
        return 0;

    // We return the number of bytes read
    return SoundSource->ReadAudioData(PCMData, BytesDesired);
}

