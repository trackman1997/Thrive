// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundBagComponent.h"


// Sets default values for this component's properties
UCompoundBagComponent::UCompoundBagComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	storageSpaceTotal = 0.0;
	storageSpaceOccupied = 0.0;
	// TODO: initialize the compoud array here.
}


// Called when the game starts
void UCompoundBagComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UCompoundBagComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UCompoundBagComponent::getStorageSpaceAvailable() {
	return storageSpaceTotal - storageSpaceOccupied;
}

float UCompoundBagComponent::getStorageSpaceTotal() {
	return storageSpaceTotal;
}

float UCompoundBagComponent::getStorageSpaceOccupied() {
	return storageSpaceOccupied;
}