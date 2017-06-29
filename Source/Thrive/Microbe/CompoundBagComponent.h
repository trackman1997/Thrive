// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Components/ActorComponent.h"
#include "MicrobeCommon.h"
#include "CompoundBagComponent.generated.h"

struct CompoundStorageData {
	float amount;
	float uninflatedPrice;
	float price;
	float demand;
	float priceReductionPerUnit;
	float breakEvenPoint;
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

private:
	TMap<ECompoundID, CompoundStorageData> compounds;
	float storageSpaceTotal;
	float storageSpaceOccupied;
};
