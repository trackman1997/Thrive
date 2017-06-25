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

	for (int i = -size / 2; i <= size / 2; i++)
		for (int j = -size / 2; j < size / 2; j++) {
			FVector2D point = FVector2D(i, j) + organellePosition;
			result.Add(point);
		}

	return result;
}
