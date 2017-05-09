// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CellBase.h"



#include "Sound/SoundWave.h"


#include "VictoryBPLibrary/Public/VictoryBPFunctionLibrary.h"

//#include "AudioDevice.h"
//#include "Runtime/Engine/Public/VorbisAudioInfo.h"
//#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"


//#include <memory>

// Sets default values
ACellBase::ACellBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this
 	// off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    MembraneComponent = CreateDefaultSubobject<UMembraneComponent>(TEXT("RootMembrane"));
    RootComponent = MembraneComponent;

    
    //MembraneComponent->InitSphereRadius(40.0f);
    //MembraneComponent->SetCollisionProfileName(TEXT("Pawn"));

    OurMovementComponent = CreateDefaultSubobject<UCellPawnMovementComponent>(
        TEXT("CustomMovementComponent"));
    OurMovementComponent->UpdatedComponent = RootComponent;


    //SoundVisComponent = CreateDefaultSubobject<USoundVisComponent>(TEXT("TestMusic"));
    
    
    // UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(
    //     TEXT("VisualRepresentation"));
    
    // SphereVisual->SetupAttachment(RootComponent);
    
    // static ConstructorHelpers::FObjectFinder<UStaticMesh>
    //     SphereVisualAsset(TEXT("/Engine/BasicShapes/Sphere"));
    
    // if(SphereVisualAsset.Succeeded()){
        
    //     SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
    //     //SphereVisual->SetMaterial(0, class UMaterialInterface *Material)
    //     SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    //     SphereVisual->SetWorldScale3D(FVector(0.8f));
    // }


    AudioComponent = CreateDefaultSubobject<UAudioComponent>(FName("AudioComponent"));
    
}

// Called when the game starts or when spawned
void ACellBase::BeginPlay()
{
	Super::BeginPlay();

    FString testSound = FPaths::GameContentDir() +
        "/ExtraContent/Music/main-menu-theme-1.ogg";

    if(FPaths::FileExists(testSound)){

        DISPLAY_MESSAGE(TEXT("playing stuff"));

        //CompressedSoundWaveRef = NewObject<USoundWave>(USoundWave::StaticClass());

        CompressedSoundWaveRef = UVictoryBPFunctionLibrary::GetSoundWaveFromFile(testSound);

        // Make sure the SoundWave Object is Valid
        if(!CompressedSoundWaveRef){

            LOG_ERROR("Failed to create new SoundWave Object");
            return;
        }

    //     TArray<uint8> rawFile;

    //     // Load file into RawFile Array
    //     bool loadSuccess = FFileHelper::LoadFileToArray(rawFile,
    //         testSound.GetCharArray().GetData());

    //     if(!loadSuccess){

    //         LOG_ERROR("failed to load sound");
    //         return;
    //     }

    //     if(rawFile.Num() > 0){

	// 		//bLoaded = FillSoundWaveInfo(CompressedSoundWaveRef, &RawFile);
    //         FSoundQualityInfo SoundQualityInfo;
    //         FVorbisAudioInfo VorbisAudioInfo;
	
    //         // Save the Info into SoundQualityInfo
    //         if(!VorbisAudioInfo.ReadCompressedInfo(rawFile.GetData(), rawFile.Num(),
    //                 &SoundQualityInfo))
    //         {
    //             LOG_ERROR("failed to read vorbis compressed audio info");
    //             return;
    //         }

    //         // Fill in all the Data we have
    //         //CompressedSoundWaveRef->DecompressionType = EDecompressionType::DTYPE_RealTime;
    //         CompressedSoundWaveRef->DecompressionType = EDecompressionType::DTYPE_Native;
    //         CompressedSoundWaveRef->SoundGroup = ESoundGroup::SOUNDGROUP_Default;
    //         CompressedSoundWaveRef->NumChannels = SoundQualityInfo.NumChannels;
    //         CompressedSoundWaveRef->Duration = SoundQualityInfo.Duration;
    //         CompressedSoundWaveRef->RawPCMDataSize = SoundQualityInfo.SampleDataSize;
    //         CompressedSoundWaveRef->SampleRate = SoundQualityInfo.SampleRate;
            
	// 	} else {

	// 		LOG_ERROR("Loaded sound data is empty");

	// 		return;
	// 	}

	// 	// Get Pointer to the Compressed OGG Data
	// 	FByteBulkData* BulkData = &CompressedSoundWaveRef->CompressedFormatData.GetFormat(
    //         FName("OGG"));

	// 	// Set the Lock of the BulkData to ReadWrite
	// 	BulkData->Lock(LOCK_READ_WRITE);

	// 	// Copy compressed RawFile Data to the Address of the OGG Data of the SW File
	// 	FMemory::Memmove(BulkData->Realloc(rawFile.Num()), rawFile.GetData(), rawFile.Num());

	// 	// Unlock the BulkData again
	// 	BulkData->Unlock();


    //     //GetPCMDataFromFile(CompressedSoundWaveRef);
    //     if(CompressedSoundWaveRef->NumChannels < 1 ||
    //         CompressedSoundWaveRef->NumChannels > 2)
    //     {
    //         LOG_ERROR("sound has invalid number of channels");
    //         return;
    //     }

    //     if (GEngine)
    //     {
    //         // Get a Pointer to the Main Audio Device
    //         FAudioDevice* AudioDevice = GEngine->GetMainAudioDevice();

    //         if(AudioDevice){

    //             CompressedSoundWaveRef->InitAudioResource(
    //                 AudioDevice->GetRuntimeFormat(CompressedSoundWaveRef));

    //             // Creates a new DecompressWorker and starts it
    //             //InitNewDecompressTask(CompressedSoundWaveRef);

    //             //CompressedSoundWaveRef->Play();

                
    //             // Decompressing the audio //
    //             {
    //                 std::unique_ptr<ICompressedAudioInfo> AudioInfo(
    //                     GEngine->GetMainAudioDevice()->CreateCompressedAudioInfo(
    //                         CompressedSoundWaveRef));
                    
    //                 if(!AudioInfo){

    //                     LOG_ERROR("Failed to create audio info");
    //                     return;
    //                 }
                    
                    
    //                 FSoundQualityInfo QualityInfo = { 0 };

    //                 // Parse the audio header for the relevant information
    //                 if(!(CompressedSoundWaveRef->ResourceData))
    //                 {
    //                     LOG_ERROR("empty soundwave resource data");
    //                     return;
    //                 }

    //                 if(AudioInfo->ReadCompressedInfo(CompressedSoundWaveRef->ResourceData,
    //                         CompressedSoundWaveRef->ResourceSize, &QualityInfo))
    //                 {
    //                     FScopeCycleCounterUObject WaveObject(CompressedSoundWaveRef);

    //                     // Extract the data
    //                     CompressedSoundWaveRef->SampleRate = QualityInfo.SampleRate;
    //                     CompressedSoundWaveRef->NumChannels = QualityInfo.NumChannels;

    //                     if(QualityInfo.Duration > 0.0f)
    //                     {
    //                         CompressedSoundWaveRef->Duration = QualityInfo.Duration;
    //                     }

    //                     const uint32 PCMBufferSize = CompressedSoundWaveRef->Duration *
    //                         CompressedSoundWaveRef->SampleRate *
    //                         CompressedSoundWaveRef->NumChannels;

    //                     CompressedSoundWaveRef->CachedRealtimeFirstBuffer =
    //                         new uint8[PCMBufferSize * 2];

    //                     AudioInfo->SeekToTime(0.0f);
    //                     AudioInfo->ReadCompressedData(
    //                         CompressedSoundWaveRef->CachedRealtimeFirstBuffer, false,
    //                         PCMBufferSize * 2);
                        
    //                 } else {

    //                     LOG_ERROR("failed to read compressed audio header");
    //                     return;
    //                 }
    //             }

    //             LOG_LOG("done with setup");
    //             AudioComponent->SetSound(CompressedSoundWaveRef);
    //             AudioComponent->Play();
    //         }
    //         else {

    //             LOG_ERROR("failed to get pointer to AudioDevice");
    //             return;
    //         }
    //     }

        LOG_LOG("done with setup");
        AudioComponent->SetSound(CompressedSoundWaveRef);
        AudioComponent->Play();
    }


    // if(VacuoleClass){

    //     Vacuole = NewObject<UOrganelleComponent>(this, *VacuoleClass);
    //     AddInstanceComponent(Vacuole);

    //     if(GEngine)
    //         GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue,
    //             TEXT("Vacuole spawned"));
    // }
}

// Called every frame
void ACellBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACellBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("ToggleEngulf", IE_Pressed, this,
        &ACellBase::ToggleEngulf);
    

    
    PlayerInputComponent->BindAxis("MoveForward", this, &ACellBase::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ACellBase::MoveRight);
}

// ------------------------------------ //
// Movement code

UPawnMovementComponent* ACellBase::GetMovementComponent() const
{
    return OurMovementComponent;
}

void ACellBase::MoveForward(float AxisValue){

    if(OurMovementComponent
        && (OurMovementComponent->UpdatedComponent == RootComponent)
    ){
        // This GetActorForwardVector() call makes the movement
        // relative to the cell heading
        OurMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
    }
}

void ACellBase::MoveRight(float AxisValue){

    if(OurMovementComponent
        && (OurMovementComponent->UpdatedComponent == RootComponent)
    ){
        OurMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
    }
}

void ACellBase::ToggleEngulf(){

    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue,
            TEXT("Toggle Engulf"));
    
}

