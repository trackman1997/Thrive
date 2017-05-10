// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Pawn.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "CellPawnMovementComponent.h"

#include "RuntimeMeshComponent.h"

#include "Components/AudioComponent.h"
#include "MembraneComponent.h"
#include "OrganelleComponent.h"

#include "CellBase.generated.h"

UCLASS(Blueprintable)
class THRIVE_API ACellBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACellBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


    void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
        override;

    virtual UPawnMovementComponent* GetMovementComponent() const override;

    // Player input control functions
    void MoveForward(float AxisValue);
    void MoveRight(float AxisValue);
    void ToggleEngulf();
    
protected:

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
        UCellPawnMovementComponent* OurMovementComponent;
        //UFloatingPawnMovement* OurMovementComponent;

    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    UMembraneComponent* MembraneComponent;

    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    URuntimeMeshComponent* RuntimeMesh;

    // UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    // TSubclassOf<UOrganelleComponent> VacuoleClass;

    //UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    //USoundVisComponent* SoundVisComponent;

    // UPROPERTY(BlueprintReadOnly, Category="CellParts")
    // UOrganelleComponent* Vacuole;

    //UPROPERTY(BlueprintReadOnly, Category = "Thrive|Sound Player")
    //UAudioComponent* AudioComponent;

    //UPROPERTY()
    //USoundWave* CompressedSoundWaveRef;

    //UPROPERTY()
    //UBoxComponent* BoxComponent;
};
