// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Pawn.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "CellPawnMovementComponent.h"

#include "RuntimeMeshComponent.h"

#include "Components/AudioComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "MembraneComponent.h"
#include "OrganelleComponent.h"
#include "CompoundBagComponent.h"

#include "CellBase.generated.h"

class UMembraneComponent;
class UOrganelleContainerComponent;
class UOrganelleComponent;
class UCompoundBagComponent;

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

    //! Creates the membrane
    void OnConstruction(const FTransform& Transform) override;

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
        override;

    virtual UPawnMovementComponent* GetMovementComponent() const override;

    UFUNCTION(BlueprintCallable)
    bool IsUsingFloatingMovement() const;
        

    // Player input control functions
    void MoveForward(float AxisValue);
    void MoveRight(float AxisValue);
    void ToggleEngulf();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	UCompoundBagComponent* CompoundBag = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	UOrganelleContainerComponent* OrganelleContainerComponent = nullptr;

protected:
    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    UCellPawnMovementComponent* OurMovementComponent = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    UFloatingPawnMovement* FloatingMovementComponent = nullptr;

    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    UMembraneComponent* MembraneComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	int minMembraneSize = 40;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float Health = 100;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float MaxHealth = 100;

    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    URuntimeMeshComponent* RuntimeMesh;

    //! As it seems like cells really don't like being constrained to an axis
    //! this should do something
    //! But this doesn't work with floating pawn movement so if that is used
    //! we probably need to teleport the cell to the right z position every now and then
    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    UPhysicsConstraintComponent* PhysicsConstraintComponent;

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

    //! If true uses a UFloatingPawnMovement instead of applying force to move
    //! \warning If this is changed the whole editor needs to be restarted
    const bool bUsingFloatingMovement = true;
};
