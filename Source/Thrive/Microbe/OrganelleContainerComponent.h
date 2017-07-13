// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Components/SceneComponent.h"
#include "OrganelleComponent.h"
#include "OrganelleContainerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THRIVE_API UOrganelleContainerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrganelleContainerComponent();

	//UFUNCTION()
	//void InitializeOrganelles(ACellBase* Microbe);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FVector2D> getOrganellePoints();
};
