// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "ProceduralMeshComponent.h"

#include "Generation/GeometryHelpers.h"


#include "ThriveGeneratedMeshComponent.generated.h"

/**
 * Helper for dynamic meshes.
 * hh: taken from another project of mine with fixes from:
 * https://wiki.unrealengine.com/Procedural_Mesh_Generation
 */
UCLASS()
class THRIVE_API UThriveGeneratedMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:

    UThriveGeneratedMeshComponent();

    //! Can only be called in parent actor constructor (before data is filled)
    void SetUsesPhysics(bool CreatePhysics);

    //! Sets up this with geometry
    UFUNCTION(BlueprintCallable, Category = "Thrive|Components|DynamicMesh")
    bool SetGeneratedMeshTriangles(const TArray<FGeneratedTriangle> &Triangles);

    /** Description of collision */
    UPROPERTY(BlueprintReadOnly, Category = "Collision")
    UBodySetup* ModelBodySetup;

    // Begin UMeshComponent interface.
    virtual int32 GetNumMaterials() const override;
    // End UMeshComponent interface.
	
    // Begin Interface_CollisionDataProvider Interface
    virtual bool GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData,
        bool InUseAllTriData) override;
    virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
    virtual bool WantsNegXTriMesh() override {
        return false;
    }
    // End Interface_CollisionDataProvider Interface

    // Begin UPrimitiveComponent interface.
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    virtual class UBodySetup* GetBodySetup() override;
    // End UPrimitiveComponent interface.

    void UpdateBodySetup();
    void UpdateCollision();
    
private:

    // Begin USceneComponent interface.
    virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
    // Begin USceneComponent interface.

    //! This is the mesh data that has been generated
    TArray<FGeneratedTriangle> GeneratedMeshTris;

    //! True if should generate physics bodies
    bool bUsesPhysics = false;

    friend class FGeneratedMeshSceneProxy;
	
};
