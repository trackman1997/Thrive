// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CellBase.h"

// Sets default values
ACellBase::ACellBase()
{
    // Set this pawn to call Tick() every frame.  You can turn this
    // off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    RuntimeMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Runtime Mesh"));    
    RootComponent = RuntimeMesh;

    // RuntimeMesh->SetCollisionProfileName(TEXT("Pawn"));

    if(!bUsingFloatingMovement){
        RuntimeMesh->SetSimulatePhysics(true);
    }
    
    RuntimeMesh->bUseComplexAsSimpleCollision = false;

    MembraneComponent = CreateDefaultSubobject<UMembraneComponent>(TEXT("Membrane"));
    //MembraneComponent->SetupAttachment(RuntimeMesh);
    
    if(!bUsingFloatingMovement){

        OurMovementComponent = CreateDefaultSubobject<UCellPawnMovementComponent>(
            TEXT("CustomMovementComponent"));
        OurMovementComponent->UpdatedComponent = RootComponent;
        OurMovementComponent->PushComponent = RuntimeMesh;        

    } else {
    
        FloatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(
            TEXT("Floating Movement"));
        FloatingMovementComponent->UpdatedComponent = RootComponent;

        // This prevents the collision mesh to fall on z axis, but only
        // when using floating pawn component, which would break the
        // collision
        FloatingMovementComponent->SetPlaneConstraintEnabled(true);
        FloatingMovementComponent->SetPlaneConstraintNormal(FVector(0, 0, 1));
    }

    // Extra constraint stuff
    // None of these seem to help with floating pawn movement
    PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(
        TEXT("Z Axis Constraint 2"));

    FConstraintInstance& Constraint = PhysicsConstraintComponent->ConstraintInstance;
    Constraint.SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.0f);

    //Constraint.SetLinearPositionDrive(false, false, true);
    //Constraint.SetLinearPositionTarget(FVector(0, 0, 60));
    
    PhysicsConstraintComponent->SetConstrainedComponents(RuntimeMesh, NAME_None, nullptr,
        NAME_None);
}

// Called when the game starts or when spawned
void ACellBase::BeginPlay()
{
    Super::BeginPlay();
}

void ACellBase::OnConstruction(const FTransform& Transform)
{
	TArray<FVector2D> points;
	TArray<UOrganelleContainerComponent*> aux;
	GetComponents(aux);

	if (aux.Num() > 0) {
		UOrganelleContainerComponent* organelleContainer = aux[0];
		points = organelleContainer->getOrganellePoints();
	}

    MembraneComponent->CreateMembraneMesh(RuntimeMesh, points);
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
    if(OurMovementComponent)
        return OurMovementComponent;
    if(FloatingMovementComponent)
        return FloatingMovementComponent;
    return nullptr;
}

void ACellBase::MoveForward(float AxisValue){

    if(OurMovementComponent)
        OurMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
    if(FloatingMovementComponent)
        FloatingMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
}

void ACellBase::MoveRight(float AxisValue){

    if(OurMovementComponent)
        OurMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
    if(FloatingMovementComponent)
        FloatingMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
}

void ACellBase::ToggleEngulf(){

    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue,
            TEXT("Toggle Engulf"));
    
}

