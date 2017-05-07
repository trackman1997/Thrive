// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CellBase.h"


// Sets default values
ACellBase::ACellBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this
 	// off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    OurMovementComponent = CreateDefaultSubobject<UCellPawnMovementComponent>(
        TEXT("CustomMovementComponent"));
    OurMovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void ACellBase::BeginPlay()
{
	Super::BeginPlay();

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

