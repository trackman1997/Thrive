// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CellBase.h"

// Sets default values
ACellBase::ACellBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this
 	// off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


    // BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    // RootComponent = BoxComponent;
    // BoxComponent->InitBoxExtent(FVector(50, 50, 50));
    // BoxComponent->SetCollisionProfileName(TEXT("Pawn"));
    //BoxComponent->SetCollisionProfileName(TEXT("BlockAll"));

    // USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(
    //     TEXT("RootComponent"));
    // RootComponent = SphereComponent;
    // SphereComponent->InitSphereRadius(40.0f);
    // SphereComponent->SetCollisionProfileName(TEXT("Pawn"));
    


    RuntimeMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("Runtime Mesh"));    
    RootComponent = RuntimeMesh;
    
    //MembraneComponent->SetupAttachment(RootComponent);
    //RootComponent = MembraneComponent;

    if(!bUsingFloatingMovement){
        RuntimeMesh->SetSimulatePhysics(true);
    }
    
    RuntimeMesh->bUseComplexAsSimpleCollision = false;

    MembraneComponent = CreateDefaultSubobject<UMembraneComponent>(TEXT("Membrane"));
    //MembraneComponent->SetupAttachment(RuntimeMesh);

    //MembraneComponent->SetSimulatePhysics(true);
    //MembraneComponent->bUseComplexAsSimpleCollision = false;
    
    if(!bUsingFloatingMovement){

        OurMovementComponent = CreateDefaultSubobject<UCellPawnMovementComponent>(
            TEXT("CustomMovementComponent"));
        OurMovementComponent->UpdatedComponent = RootComponent;
        OurMovementComponent->PushComponent = RuntimeMesh;        

    } else {
    
        FloatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(
            TEXT("Floating Movement"));
        FloatingMovementComponent->UpdatedComponent = RootComponent;
    }

    // This prevents the collision mesh to fall on z axis, but only
    // when using floating pawn component, which would break the
    // collision
    //OurMovementComponent->SetPlaneConstraintEnabled(true);
    //OurMovementComponent->SetPlaneConstraintNormal(FVector(0, 0, 1));
    
    
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


    //AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    //AudioComponent->SetupAttachment(RootComponent);
    
}

// Called when the game starts or when spawned
void ACellBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACellBase::OnConstruction(const FTransform& Transform)
{
    // if(VacuoleClass){

    //     Vacuole = NewObject<UOrganelleComponent>(this, *VacuoleClass);
    //     AddInstanceComponent(Vacuole);

    //     if(GEngine)
    //         GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue,
    //             TEXT("Vacuole spawned"));
    // }
    
    MembraneComponent->CreateMembraneMesh(RuntimeMesh);
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

    // if(OurMovementComponent
    //     && (OurMovementComponent->UpdatedComponent == RootComponent)
    // ){
        // This GetActorForwardVector() call makes the movement
        // relative to the cell heading

        if(OurMovementComponent)
            OurMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
        if(FloatingMovementComponent)
            FloatingMovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
        //}
}

void ACellBase::MoveRight(float AxisValue){

    // if(OurMovementComponent
    //     && (OurMovementComponent->UpdatedComponent == RootComponent)
    // ){
        if(OurMovementComponent)
            OurMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
        if(FloatingMovementComponent)
            FloatingMovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
        //}
}

void ACellBase::ToggleEngulf(){

    if(GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Blue,
            TEXT("Toggle Engulf"));
    
}

