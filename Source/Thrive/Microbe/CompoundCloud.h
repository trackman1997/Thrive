// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Actor.h"
#include "CompoundCloud.generated.h"

UCLASS()
class THRIVE_API ACompoundCloud : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACompoundCloud();


    //! Updates the density texture
    void UpdateTexture();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //! Sets up the materials to look nice in the editor
    void OnConstruction(const FTransform& Transform) override;


protected:

    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UMaterialInterface* CloudMaterialBase = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UTexture2D* DensityMaterial = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UStaticMeshComponent* PlaneMesh;
    
};
