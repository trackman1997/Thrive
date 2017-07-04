// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Components/ActorComponent.h"
#include "MicrobeCommon.h"
#include "CompoundBagComponent.generated.h"

// The minimum positive price a compound can have.
#define MIN_POSITIVE_COMPOUND_PRICE 0.00001

// The "willingness" of the compound prices to change.
// (between 0.0 and 1.0)
#define COMPOUND_PRICE_MOMENTUM 0.2

// How much the "important" compounds get their price inflated.
#define IMPORTANT_COMPOUND_BIAS 1000.0

// How important the storage space is considered.
#define STORAGE_SPACE_MULTIPLIER 2.0

// Used to soften the demand according to the process capacity.
#define PROCESS_CAPACITY_DEMAND_MULTIPLIER 15.0

// The initial variables of the system.
#define INITIAL_COMPOUND_PRICE 10.0
#define INITIAL_COMPOUND_DEMAND 1.0

struct CompoundStorageData {
	float Amount;
	float UninflatedPrice;
	float Price;
	float Demand;
	float PriceReductionPerUnit;
	float BreakEvenPoint;
	ECompoundID CompoundId;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THRIVE_API UCompoundBagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCompoundBagComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Thrive")
	float getStorageSpaceAvailable();

	UFUNCTION(BlueprintPure, Category = "Thrive")
	float getStorageSpaceTotal();

	UFUNCTION(BlueprintPure, Category = "Thrive")
	float getStorageSpaceOccupied();

	UFUNCTION(BlueprintCallable, Category = "Thrive")
	float getCompoundAmount(ECompoundID CompoundId);

	UFUNCTION(BlueprintPure, Category = "Thrive")
	float getPrice(ECompoundID CompoundId);

	UFUNCTION(BlueprintPure, Category = "Thrive")
	float getDemand(ECompoundID CompoundId);

	UFUNCTION(BlueprintCallable, Category = "Thrive")
	float takeCompound(ECompoundID CompoundId, float amount); // remove up to a certain amount of compound, returning how much was removed

	UFUNCTION(BlueprintCallable, Category = "Thrive")
	void giveCompound(ECompoundID CompoundId, float amount);


private:
	TMap<ECompoundID, CompoundStorageData> compounds;
	float storageSpaceTotal;
	float storageSpaceOccupied;
};
