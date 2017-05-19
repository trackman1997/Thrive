// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "ThriveVideoPlayer.h"


// Sets default values
AThriveVideoPlayer::AThriveVideoPlayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AThriveVideoPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThriveVideoPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ------------------------------------ //
bool AThriveVideoPlayer::PlayVideo(const FString &NewVideoFile){
    
    if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*NewVideoFile))
        return false;

    VideoFile = NewVideoFile;

    
    OnPlayBackEnded.Broadcast(VideoFile);
    return true;
}

void AThriveVideoPlayer::Close(){
    
}
// ------------------------------------ //
bool AThriveVideoPlayer::hasAudio() const{

    return false;
}


float AThriveVideoPlayer::getCurrentTime() const{

    return 0.f;
}
// ------------------------------------ //
// void AThriveVideoPlayer::OnVideoEnded_Implementation(){

// }
