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

	storageSpaceTotal = 100.0;
	storageSpaceOccupied = 0.0;
}


// Called when the game starts
void UCompoundBagComponent::BeginPlay()
{
	Super::BeginPlay();

	AMicrobeGameModeBase* GameMode = Cast<AMicrobeGameModeBase>(GetWorld()->GetAuthGameMode());
	if (!GameMode) {

		LOG_FATAL("Compound cloud manager couldn't get the gamemode");
		return;
	}

	UCompoundRegistry*  Registry = GameMode->GetCompoundRegistry();

	for (auto Compound : Registry->GetRegisteredCompounds()) {
		CompoundStorageData Data;
		Data.Amount = 0;
		Data.UninflatedPrice = INITIAL_COMPOUND_PRICE;
		Data.Price = INITIAL_COMPOUND_PRICE;
		Data.Demand = INITIAL_COMPOUND_DEMAND;
		Data.PriceReductionPerUnit = 0;
		Data.BreakEvenPoint = 0;
		Data.CompoundName = Compound.InternalName;
		compounds.Add(Data.CompoundName, Data);
	}
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

float UCompoundBagComponent::getCompoundAmount(const FName &CompoundName) {
	if (!compounds.Contains(CompoundName)) return 0.0;
	return compounds[CompoundName].Amount;
}

float UCompoundBagComponent::getPrice(const FName &CompoundName) {
	if (!compounds.Contains(CompoundName)) return 0.0;
	return compounds[CompoundName].Price;
}

float UCompoundBagComponent::getDemand(const FName &CompoundName) {
	if (!compounds.Contains(CompoundName)) return 0.0;
	return compounds[CompoundName].Demand;
}

float UCompoundBagComponent::takeCompound(const FName &CompoundName, float amount) {
	if (!compounds.Contains(CompoundName)) return 0.0;
	if (compounds[CompoundName].Amount > amount) {
		compounds[CompoundName].Amount -= amount;
		return amount;
	}

	else {
		float amountTaken = compounds[CompoundName].Amount;
		compounds[CompoundName].Amount = 0;
		return amountTaken;
	}
}

void UCompoundBagComponent::giveCompound(const FName &CompoundName, float amount) {
	compounds[CompoundName].Amount += amount;
}
