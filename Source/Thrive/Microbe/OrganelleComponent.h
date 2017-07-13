// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

//#include "CellBase.h"
#include "OrganelleComponentFunctionality.h"
#include "Components/StaticMeshComponent.h"
#include "OrganelleComponent.generated.h"

class UOrganelleComponentFunctionality;
class ACellBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THRIVE_API UOrganelleComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrganelleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//void OnAddedToMicrobe(ACellBase* Microbe);

	void OnRemovedFromMicrobe();

	// The size of the membrane it generates.
	// It should probaby be unsigned but it's not supported yet.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int size = 15;

	TArray<FVector2D> getPoints();

	float Health = 1.0;
	TArray<UOrganelleComponentFunctionality*> Functionalities;
};
