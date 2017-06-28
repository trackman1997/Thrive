// Copyright (C) 2013-2017  Revolutionary Games

#pragma once


#include "OrganelleContainerComponent.h"

#include "RuntimeMeshComponent.h"

#include "Materials/MaterialInterface.h"

#include <tuple>

#include "MembraneComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Thrive), meta=(BlueprintSpawnableComponent))
class THRIVE_API UMembraneComponent : public UActorComponent
{
    GENERATED_BODY()
    
public:

    UMembraneComponent();

    /**
    * Builds the membrane based on the set UOrganelleContainerComponent
    */
    void BeginPlay() override;

    //! Creates the mesh, needs to be called in actor OnConstruction
    void CreateMembraneMesh(URuntimeMeshComponent* GeometryReceiver, FBox2D& membraneBox);


    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float ScaleUp = 10.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Height = 3.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UMaterialInterface* MembraneMaterialBase = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector MembraneColourTint = FVector(1.0f, 1.2f, 1.0f);
    
protected:

    // Methods adapted from the old membrane system

    //! \brief Renamed from the old Initialize method. Calculates the required size
    //!
    //! usually followed by calls to DrawMembrane and MakePrism.
    //! Called automatically by CreateMembraneMesh
    void DoMembraneIterativeShrink();
    
    //! Creates the 2D points in the membrane by looking at the positions of the organelles.
    void DrawMembrane();

    //! Creates a 3D prism from the 2D vertices.
    void MakePrism();

    //! Finds the UV coordinates be projecting onto a plane and stretching to fit a circle.
    void CalcUVCircle();


    // Helpers for the current "iterative shrink" method of making the
    // membrane fit the organelles

    //! Return the position of the closest organelle to the target
    //! point if it is less then a certain threshold away.
    //! \returns Second element is false if nothing was found
    std::tuple<FVector2D, bool> FindClosestOrganelles(FVector2D Target);

    //! Decides where the point needs to move based on the position of the closest organelle.
    FVector2D GetMovement(FVector2D Target, FVector2D ClosestOrganelle);

protected:

    UPROPERTY(BlueprintReadOnly)
    UOrganelleContainerComponent* OrganelleContainerComponent = nullptr;

    //! The length in pixels of a side of the square that bounds the membrane.
    //! Half the side length of the original square that is compressed to make the membrane.
    UPROPERTY(BlueprintReadOnly)
    int32 CellDimensions = 10;

    //! The amount of points on the side of the membrane.
    //! Amount of segments on one side of the above described square.
    UPROPERTY(BlueprintReadOnly)
    int32 MembraneResolution = 10;

    

    // The mesh data that is written when generating and then used to
    // update the RuntimeMeshComponent

    //! Stores the Mesh in a vector such that every 3 points make up a triangle.
    TArray<FVector> MeshPoints;

    //! Stores the UV coordinates for the MeshPoints.
    TArray<FVector2D> UVs;

    //! These are initially placed far away and shrink towards the organelles
    TArray<FVector2D> Vertices2D;

    //! Temporary for increasing copying efficiency of Vertices2D in DoMembraneIterativeShrink
    TArray<FVector2D> NewPositions;

    // Temporary data for generation //

    //! Stores organelle data during generation
    TArray<FVector2D> OrganellePositions;
};
