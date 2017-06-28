// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "OrganelleComponent.h"


// Sets default values for this component's properties
UOrganelleComponent::UOrganelleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOrganelleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOrganelleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FVector2D> UOrganelleComponent::getPoints() {
	TArray<FVector2D> result;
	FVector2D organellePosition(GetComponentLocation());
	int organelleX = FMath::CeilToInt(organellePosition.X);
	int organelleY = FMath::CeilToInt(organellePosition.Y);
	result.Emplace(organelleX - size, organelleY - size);
	result.Emplace(organelleX + size, organelleY + size);
	return result;
}
