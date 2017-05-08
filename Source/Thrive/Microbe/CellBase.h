// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Pawn.h"

#include "Components/SphereComponent.h"

#include "OrganelleComponent.h"

#include "CellPawnMovementComponent.h"

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

    UPROPERTY(BlueprintReadOnly)
    UCellPawnMovementComponent* OurMovementComponent;

    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    USphereComponent* SphereComponent;

    // UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    // TSubclassOf<UOrganelleComponent> VacuoleClass;

    // UPROPERTY(BlueprintReadOnly, Category="CellParts")
    // UOrganelleComponent* Vacuole;
	
};
