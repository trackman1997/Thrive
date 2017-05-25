// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "PlayerSoundWaveParent.h"

#include "EngineDefines.h"
#include "ActiveSound.h"

#include "VideoPlayerSoundWave.h"
#include "ThriveVideoPlayer.h"

UPlayerSoundWaveParent::UPlayerSoundWaveParent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPlayerSoundWaveParent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

    // We probably cannot serialize our pointer
	//RawData.Serialize(Ar, this);
}
// ------------------------------------ //
bool UPlayerSoundWaveParent::IsPlayable() const{

    return true;
}

void UPlayerSoundWaveParent::Parse(class FAudioDevice* AudioDevice,
    const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound,
    const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances)
{
    FWaveInstance* WaveInstance = ActiveSound.FindWaveInstance(NodeWaveInstanceHash);

	// Create a new WaveInstance if this SoundWave doesn't already have one associated with it.
	if (WaveInstance == NULL)
	{
        std::lock_guard<std::mutex> Lock(SoundSourceMutex);
        check(SoundSource);
        
		const int32 SampleRate = SoundSource->GetAudioSampleRate();

		// Create a new wave instance and associate with the ActiveSound
		WaveInstance = new FWaveInstance(&ActiveSound);
		WaveInstance->WaveInstanceHash = NodeWaveInstanceHash;
		ActiveSound.WaveInstances.Add(NodeWaveInstanceHash, WaveInstance);

		// Create streaming wave object
		UVideoPlayerSoundWave* OurWave = NewObject<UVideoPlayerSoundWave>();
		OurWave->SampleRate = SampleRate;
		OurWave->NumChannels = SoundSource->GetAudioChannelCount();
        // TODO: we could use the actual duration
		OurWave->Duration = INDEFINITELY_LOOPING_DURATION;
		OurWave->bLooping = false;
        //OurWave->DecompressionType = DTYPE_Native;

        OurWave->Parent = this;

		WaveInstance->WaveData = OurWave;
	}

	WaveInstance->WaveData->Parse(AudioDevice, NodeWaveInstanceHash, ActiveSound,
        ParseParams, WaveInstances);
}

float UPlayerSoundWaveParent::GetMaxAudibleDistance(){

    return (AttenuationSettings ? AttenuationSettings->Attenuation.GetMaxDimension() :
        WORLD_MAX);
}
// ------------------------------------ //
void UPlayerSoundWaveParent::ClearSource(){

    std::lock_guard<std::mutex> Lock(SoundSourceMutex);
    SoundSource = nullptr;
}

void UPlayerSoundWaveParent::SetSource(AThriveVideoPlayer* DataSource){
    // This is done during construction (before playing) so we don't lock here
    check(DataSource);
    SoundSource = DataSource;
}

std::mutex& UPlayerSoundWaveParent::GetMutex(){

    return SoundSourceMutex;
}

AThriveVideoPlayer* UPlayerSoundWaveParent::GetDataAccess(std::lock_guard<std::mutex> &Lock)
const
{
    return SoundSource;
}
