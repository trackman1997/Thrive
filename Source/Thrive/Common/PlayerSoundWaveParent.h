// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Serialization/BulkData.h"
#include "Sound/SoundBase.h"

#include <mutex>

#include "PlayerSoundWaveParent.generated.h"

struct FActiveSound;
struct FSoundParseParameters;


class AThriveVideoPlayer;
class UVideoPlayerSoundWave;

/**
* Wrapper object like the MOD sound plugin does
*/
UCLASS(hidecategories=Object, MinimalAPI, BlueprintType)
class UPlayerSoundWaveParent : public USoundBase
{
	GENERATED_UCLASS_BODY()

    friend AThriveVideoPlayer;
    friend UVideoPlayerSoundWave;
public:	
	//~ Begin UObject Interface. 
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface. 

	//~ Begin USoundBase Interface.
	virtual bool IsPlayable() const override;
	virtual void Parse(class FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash,
        FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams,
        TArray<FWaveInstance*>& WaveInstances) override;
	virtual float GetMaxAudibleDistance() override;
	//~ End USoundBase Interface.

protected:

    //! Removes SoundSource from this sound
    void ClearSource();

    //! Removes SoundSource from this sound
    void SetSource(AThriveVideoPlayer* DataSource);

    // Data access for UVideoPlayerSoundWave //

    std::mutex& GetMutex();

    AThriveVideoPlayer* GetDataAccess(std::lock_guard<std::mutex> &Lock) const;

private:
    
    AThriveVideoPlayer* SoundSource = nullptr;

    //! Mutex for touching SoundSource to prevent issues between sound on main threads
    std::mutex SoundSourceMutex;
    
};
