// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundBagComponent.h"
#include "MicrobeGameModeBase.h"


// Sets default values for this component's properties
UCompoundBagComponent::UCompoundBagComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	storageSpaceTotal = 0.0;
	storageSpaceOccupied = 0.0;

	// DONT DO THIS!!!!!
	/*
	AMicrobeGameModeBase* GameMode = Cast<AMicrobeGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode) {

		LOG_FATAL("Compound cloud manager couldn't get the gamemode");
		return;
	}

	auto* Registry = GameMode->GetCompoundRegistry();
	*/
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

float UCompoundBagComponent::getCompoundAmount(ECompoundID CompoundId) {
	return compounds[CompoundId].amount;
}

float UCompoundBagComponent::getPrice(ECompoundID CompoundId) {
	return compounds[CompoundId].price;
}

float UCompoundBagComponent::getDemand(ECompoundID CompoundId) {
	return compounds[CompoundId].demand;
}

float UCompoundBagComponent::takeCompound(ECompoundID CompoundId, float amount) {
	if (compounds[CompoundId].amount > amount) {
		compounds[CompoundId].amount -= amount;
		return amount;
	}

	else {
		float amountTaken = compounds[CompoundId].amount;
		compounds[CompoundId].amount = 0;
		return amountTaken;
	}
}

void UCompoundBagComponent::giveCompound(ECompoundID CompoundId, float amount) {
	compounds[CompoundId].amount += amount;
}
