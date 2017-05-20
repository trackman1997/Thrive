// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Sound/SoundWaveProcedural.h"
#include "VideoPlayerSoundWave.generated.h"

/**
 * 
 */
UCLASS()
class THRIVE_API UVideoPlayerSoundWave : public USoundWaveProcedural
{
	GENERATED_BODY()
	
    friend class AThriveVideoPlayer;

public:
    
    UVideoPlayerSoundWave();
    
protected:
    UPROPERTY()
    class AThriveVideoPlayer* SoundSource;
    
    //~ Begin USoundWave Interface.
    virtual int32 GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded) override;
    //~ End USoundWave Interface.
	
};
