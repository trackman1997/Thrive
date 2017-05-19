// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Actor.h"
#include "ThriveVideoPlayer.generated.h"

UCLASS()
class THRIVE_API AThriveVideoPlayer : public AActor
{
	GENERATED_BODY()

    //! This has to be on a single line to work
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVideoPlaybackEndedDelegate, FString, VideoFile);
        
public:	
	// Sets default values for this actor's properties
	AThriveVideoPlayer();


    //! Starts playing a video the playback
    //! \pre The target texture 
    //! \returns False if the file is invalid and playback didn't start
    UFUNCTION(BlueprintCallable, Category="Thrive|VideoPlayer")
    bool PlayVideo(const FString &NewVideoFile);

    //! Closes the video stream
    UFUNCTION(BlueprintCallable, Category="Thrive|VideoPlayer")
    void Close();
    
    //! \returns True if currently loaded file has an audio stream
    bool hasAudio() const;

    //! \returns Current playback position, in seconds
    //! The return value is directly read from the last decoded frame timestamp
    float getCurrentTime() const;

    UFUNCTION(BlueprintImplementableEvent, Category="Thrive|VideoPlayer")
    void OnVideoEnded();

public:

    UPROPERTY(BlueprintAssignable, Category="Thrive|VideoPlayer")
    FVideoPlaybackEndedDelegate OnPlayBackEnded;
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
protected:

    FString VideoFile; 

};
