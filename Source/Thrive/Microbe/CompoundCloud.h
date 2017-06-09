// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/Actor.h"

#include "MicrobeCommon.h"

#include "CompoundCloud.generated.h"

UCLASS()
class THRIVE_API ACompoundCloud : public AActor
{
	GENERATED_BODY()

    //! Contains data for single compound type's layer
    struct FLayerData{

        ECompoundID ID;

        // Colour can be retrieved from UCompoundRegistry
        
        
        //! The 2D array that contains the current compound clouds and those from last frame.
        TArray<TArray<float>> Blob1;
        TArray<TArray<float>> Blob2;

        //! Access with these to allow swapping easily
        //! And define local references to these to avoid all chances of redirects eating up
        //! performance
        TArray<TArray<float>>* Density = &Blob1;
        TArray<TArray<float>>* LastDensity = &Blob2;
        
    };
        
public:	
	// Sets default values for this actor's properties
	ACompoundCloud();

    //! Initializes this cloud for compounds
    //! All compound types that will be used during the lifetime of this object need to be
    //! specified here
    void Initialize(ECompoundID Compound1, ECompoundID Compound2,
        ECompoundID Compound3, ECompoundID Compound4);

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

    // Textures for the density
    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UTexture2D* DensityMaterial = nullptr;

    //! Material instance with the right colours and our DensityMaterial applied
    UPROPERTY()
    UMaterialInstanceDynamic* DynMaterial = nullptr;

    UPROPERTY(BlueprintReadOnly, EditAnyWhere)
    UStaticMeshComponent* PlaneMesh;
    
};
