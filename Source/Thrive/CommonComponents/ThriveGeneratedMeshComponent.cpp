// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "ThriveGeneratedMeshComponent.h"

#include "DynamicMeshBuilder.h"
#include "LocalVertexFactory.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "PhysicsEngine/BodySetup.h"


//////////////////////////////////////////////////////////////////////////
/** Vertex Buffer */
class FGeneratedMeshVertexBuffer : public FVertexBuffer {
public:
    TArray<FDynamicMeshVertex> Vertices;

    virtual void InitRHI() override {

        FRHIResourceCreateInfo CreateInfo;

        void* VertexBufferData = nullptr;

        VertexBufferRHI = RHICreateAndLockVertexBuffer(Vertices.Num() *
            sizeof(FDynamicMeshVertex), BUF_Static, 
            CreateInfo, VertexBufferData);

        // Copy the vertex data into the vertex buffer.		
        FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() *
            sizeof(FDynamicMeshVertex));
        RHIUnlockVertexBuffer(VertexBufferRHI);
    }
};

/** Index Buffer */
class FGeneratedMeshIndexBuffer : public FIndexBuffer {
public:
    TArray<int32> Indices;

    virtual void InitRHI() override {
        FRHIResourceCreateInfo CreateInfo;

        void* Buffer = nullptr;

        IndexBufferRHI = RHICreateAndLockIndexBuffer(sizeof(int32), Indices.Num() *
            sizeof(int32), BUF_Static, 
            CreateInfo, Buffer);

        // Write the indices to the index buffer.		
        FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
        RHIUnlockIndexBuffer(IndexBufferRHI);
    }
};

/** Vertex Factory */
class FGeneratedMeshVertexFactory : public FLocalVertexFactory {
public:

    FGeneratedMeshVertexFactory() {
    }


    /** Initialization */
    void Init_RenderThread(const FGeneratedMeshVertexBuffer* VertexBuffer) {
        check(IsInRenderingThread());

        FDataType NewData;
        NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer,
            FDynamicMeshVertex, Position, VET_Float3);
        NewData.TextureCoordinates.Add(
            FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex,
                    TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
        );
        NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer,
            FDynamicMeshVertex, TangentX, VET_PackedNormal);
        NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer,
            FDynamicMeshVertex, TangentZ, VET_PackedNormal);
        NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer,
            FDynamicMeshVertex, Color, VET_Color);

        SetData(NewData);
    }

    void Init(const FGeneratedMeshVertexBuffer* VertexBuffer) {
        if (IsInRenderingThread())
        {
            Init_RenderThread(VertexBuffer);
        } else
        {
            ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
                InitCustomMeshVertexFactory,
                FGeneratedMeshVertexFactory*, VertexFactory, this,
                const FGeneratedMeshVertexBuffer*, VertexBuffer, VertexBuffer,
                {
                    VertexFactory->Init_RenderThread(VertexBuffer);
                });
        }
    }
};
//////////////////////////////////////////////////////////////////////////
class FGeneratedMeshSceneProxy : public FPrimitiveSceneProxy {
public:

    FGeneratedMeshSceneProxy(UThriveGeneratedMeshComponent* Component)
        : FPrimitiveSceneProxy(Component)
          , MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
    {
        const FColor VertexColor(255, 255, 255);

        // Add each triangle to the vertex/index buffer
        for (int TriIdx = 0; TriIdx < Component->GeneratedMeshTris.Num(); TriIdx++)
        {
            const auto& Tri = Component->GeneratedMeshTris[TriIdx];

            const FVector Edge01 = (Tri.Vertex1 - Tri.Vertex0);
            const FVector Edge02 = (Tri.Vertex2 - Tri.Vertex0);

            const FVector TangentX = Edge01.GetSafeNormal();
            const FVector TangentZ = (Edge02 ^ Edge01).GetSafeNormal();
            const FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal();

            FDynamicMeshVertex Vert0;
            Vert0.Position = Tri.Vertex0;
            Vert0.Color = VertexColor;
            Vert0.TextureCoordinate = Tri.UV0;
            Vert0.SetTangents(TangentX, TangentY, TangentZ);
            int32 VIndex = VertexBuffer.Vertices.Add(Vert0);
            IndexBuffer.Indices.Add(VIndex);

            FDynamicMeshVertex Vert1;
            Vert1.Position = Tri.Vertex1;
            Vert1.Color = VertexColor;
            Vert1.TextureCoordinate = Tri.UV1;
            Vert1.SetTangents(TangentX, TangentY, TangentZ);
            VIndex = VertexBuffer.Vertices.Add(Vert1);
            IndexBuffer.Indices.Add(VIndex);

            FDynamicMeshVertex Vert2;
            Vert2.Position = Tri.Vertex2;
            Vert2.Color = VertexColor;
            Vert2.TextureCoordinate = Tri.UV2;
            Vert2.SetTangents(TangentX, TangentY, TangentZ);
            VIndex = VertexBuffer.Vertices.Add(Vert2);
            IndexBuffer.Indices.Add(VIndex);
        }

        // Init vertex factory
        VertexFactory.Init(&VertexBuffer);

        // Enqueue initialization of render resource
        BeginInitResource(&VertexBuffer);
        BeginInitResource(&IndexBuffer);
        BeginInitResource(&VertexFactory);

        // Grab material
        Material = Component->GetMaterial(0);
        if (Material == NULL)
        {
            Material = UMaterial::GetDefaultMaterial(MD_Surface);
        }
    }

    virtual ~FGeneratedMeshSceneProxy() {
        VertexBuffer.ReleaseResource();
        IndexBuffer.ReleaseResource();
        VertexFactory.ReleaseResource();
    }

    virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
        const FSceneViewFamily& ViewFamily, uint32 VisibilityMap,
        FMeshElementCollector& Collector) const override
    {
        QUICK_SCOPE_CYCLE_COUNTER(STAT_CustomMeshSceneProxy_GetDynamicMeshElements);

        const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

        auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
            GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(
                IsSelected()) : NULL,
            FLinearColor(0, 0.5f, 1.f)
        );

        Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

        FMaterialRenderProxy* MaterialProxy = NULL;
        if (bWireframe)
        {
            MaterialProxy = WireframeMaterialInstance;
        } else
        {
            MaterialProxy = Material->GetRenderProxy(IsSelected());
        }

        for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
        {
            if (VisibilityMap & (1 << ViewIndex))
            {
                const FSceneView* View = Views[ViewIndex];
                // Draw the mesh.
                FMeshBatch& Mesh = Collector.AllocateMesh();
                FMeshBatchElement& BatchElement = Mesh.Elements[0];
                BatchElement.IndexBuffer = &IndexBuffer;
                Mesh.bWireframe = bWireframe;
                Mesh.VertexFactory = &VertexFactory;
                Mesh.MaterialRenderProxy = MaterialProxy;
                
                BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(
                    GetLocalToWorld(), GetBounds(), GetLocalBounds(), true,
                    UseEditorDepthTest());
                
                BatchElement.FirstIndex = 0;
                BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
                BatchElement.MinVertexIndex = 0;
                BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
                Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
                Mesh.Type = PT_TriangleList;
                Mesh.DepthPriorityGroup = SDPG_World;
                Mesh.bCanApplyViewModeOverrides = false;
                Collector.AddMesh(ViewIndex, Mesh);
            }
        }
    }

    virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override {
        FPrimitiveViewRelevance Result;
        Result.bDrawRelevance = IsShown(View);
        Result.bShadowRelevance = IsShadowCast(View);
        Result.bDynamicRelevance = true;
        Result.bRenderInMainPass = ShouldRenderInMainPass();
        Result.bRenderCustomDepth = ShouldRenderCustomDepth();
        MaterialRelevance.SetPrimitiveViewRelevance(Result);
        return Result;
    }

    virtual bool CanBeOccluded() const override {
        return !MaterialRelevance.bDisableDepthTest;
    }

    virtual uint32 GetMemoryFootprint(void) const override {
        return(sizeof(*this) + GetAllocatedSize());
    }

    uint32 GetAllocatedSize(void) const {
        return(FPrimitiveSceneProxy::GetAllocatedSize());
    }

private:

    UMaterialInterface* Material;
    FGeneratedMeshVertexBuffer VertexBuffer;
    FGeneratedMeshIndexBuffer IndexBuffer;
    FGeneratedMeshVertexFactory VertexFactory;

    FMaterialRelevance MaterialRelevance;
};
//////////////////////////////////////////////////////////////////////////
UThriveGeneratedMeshComponent::UThriveGeneratedMeshComponent() {

    PrimaryComponentTick.bCanEverTick = false;

    SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
}

void UThriveGeneratedMeshComponent::SetUsesPhysics(bool CreatePhysics) {

    LOG_LOG("uses physics called");
    bUsesPhysics = CreatePhysics;
}
//////////////////////////////////////////////////////////////////////////
bool UThriveGeneratedMeshComponent::SetGeneratedMeshTriangles(
    const TArray<FGeneratedTriangle> &Triangles)
{
    LOG_LOG("mesh updated");
    GeneratedMeshTris = Triangles;

    if(bUsesPhysics){

        // Physics thing
        FKConvexElem NewConvexElem;
        
        // Copy in vertex info
        TArray<FVector> ConvexVerts;

        ConvexVerts.SetNum(Triangles.Num() * 3);

        for(size_t i = 0; i < Triangles.Num(); ++i){

            ConvexVerts[(i * 3)] = Triangles[i].Vertex1;
            ConvexVerts[(i * 3) + 1] = Triangles[i].Vertex2;
            ConvexVerts[(i * 3) + 2] = Triangles[i].Vertex2;
        }
        
        NewConvexElem.VertexData = ConvexVerts;
        // Update bounding box
        NewConvexElem.ElemBox = FBox(NewConvexElem.VertexData);
        // Add to array of convex elements
        OurCollisionConvexElems.SetNum(1);
        OurCollisionConvexElems[0] = NewConvexElem;
        
        UpdateCollision();
    }

    // Need to recreate scene proxy to send it over
    MarkRenderStateDirty();

    return true;
}
//////////////////////////////////////////////////////////////////////////
FPrimitiveSceneProxy* UThriveGeneratedMeshComponent::CreateSceneProxy() {

    /** Scene proxy defined only inside the scope of this one function */
    //Only create if have enough tris
    if (GeneratedMeshTris.Num() > 0)
    {
        return new FGeneratedMeshSceneProxy(this);
    } else
    {
        return nullptr;
    }
}

int32 UThriveGeneratedMeshComponent::GetNumMaterials() const {
    return 1;
}

FBoxSphereBounds UThriveGeneratedMeshComponent::CalcBounds(const FTransform & LocalToWorld)
    const
{
    if (GeneratedMeshTris.Num() < 1)
        return FBoxSphereBounds(FBox(FVector(0), FVector(0)));

    // Minimum Vector: It's set to the first vertex's position initially
    // (NULL == FVector::ZeroVector might be required and a known vertex vector has
    // intrinsically valid values)
    FVector vecMin = GeneratedMeshTris[0].Vertex0;

    // Maximum Vector: It's set to the first vertex's position initially
    // (NULL == FVector::ZeroVector might be required and a known vertex vector has
    // intrinsically valid values)
    FVector vecMax = GeneratedMeshTris[0].Vertex0;

    // Get maximum and minimum X, Y and Z positions of vectors
    for (int32 TriIdx = 0; TriIdx < GeneratedMeshTris.Num(); TriIdx++)
    {
        vecMin.X = (vecMin.X > GeneratedMeshTris[TriIdx].Vertex0.X) ? 
            GeneratedMeshTris[TriIdx].Vertex0.X : vecMin.X;
        vecMin.X = (vecMin.X > GeneratedMeshTris[TriIdx].Vertex1.X) ? 
            GeneratedMeshTris[TriIdx].Vertex1.X : vecMin.X;
        vecMin.X = (vecMin.X > GeneratedMeshTris[TriIdx].Vertex2.X) ? 
            GeneratedMeshTris[TriIdx].Vertex2.X : vecMin.X;

        vecMin.Y = (vecMin.Y > GeneratedMeshTris[TriIdx].Vertex0.Y) ? 
            GeneratedMeshTris[TriIdx].Vertex0.Y : vecMin.Y;
        vecMin.Y = (vecMin.Y > GeneratedMeshTris[TriIdx].Vertex1.Y) ? 
            GeneratedMeshTris[TriIdx].Vertex1.Y : vecMin.Y;
        vecMin.Y = (vecMin.Y > GeneratedMeshTris[TriIdx].Vertex2.Y) ? 
            GeneratedMeshTris[TriIdx].Vertex2.Y : vecMin.Y;

        vecMin.Z = (vecMin.Z > GeneratedMeshTris[TriIdx].Vertex0.Z) ? 
            GeneratedMeshTris[TriIdx].Vertex0.Z : vecMin.Z;
        vecMin.Z = (vecMin.Z > GeneratedMeshTris[TriIdx].Vertex1.Z) ? 
            GeneratedMeshTris[TriIdx].Vertex1.Z : vecMin.Z;
        vecMin.Z = (vecMin.Z > GeneratedMeshTris[TriIdx].Vertex2.Z) ? 
            GeneratedMeshTris[TriIdx].Vertex2.Z : vecMin.Z;

        vecMax.X = (vecMax.X < GeneratedMeshTris[TriIdx].Vertex0.X) ? 
            GeneratedMeshTris[TriIdx].Vertex0.X : vecMax.X;
        vecMax.X = (vecMax.X < GeneratedMeshTris[TriIdx].Vertex1.X) ? 
            GeneratedMeshTris[TriIdx].Vertex1.X : vecMax.X;
        vecMax.X = (vecMax.X < GeneratedMeshTris[TriIdx].Vertex2.X) ? 
            GeneratedMeshTris[TriIdx].Vertex2.X : vecMax.X;

        vecMax.Y = (vecMax.Y < GeneratedMeshTris[TriIdx].Vertex0.Y) ? 
            GeneratedMeshTris[TriIdx].Vertex0.Y : vecMax.Y;
        vecMax.Y = (vecMax.Y < GeneratedMeshTris[TriIdx].Vertex1.Y) ? 
            GeneratedMeshTris[TriIdx].Vertex1.Y : vecMax.Y;
        vecMax.Y = (vecMax.Y < GeneratedMeshTris[TriIdx].Vertex2.Y) ? 
            GeneratedMeshTris[TriIdx].Vertex2.Y : vecMax.Y;

        vecMax.Z = (vecMax.Z < GeneratedMeshTris[TriIdx].Vertex0.Z) ? 
            GeneratedMeshTris[TriIdx].Vertex0.Z : vecMax.Z;
        vecMax.Z = (vecMax.Z < GeneratedMeshTris[TriIdx].Vertex1.Z) ? 
            GeneratedMeshTris[TriIdx].Vertex1.Z : vecMax.Z;
        vecMax.Z = (vecMax.Z < GeneratedMeshTris[TriIdx].Vertex2.Z) ? 
            GeneratedMeshTris[TriIdx].Vertex2.Z : vecMax.Z;
    }

    FVector vecOrigin = ((vecMax - vecMin) / 2) + vecMin;
    /* Origin = ((Max Vertex's Vector - Min Vertex's Vector) / 2 ) + Min Vertex's Vector */
    
    FVector BoxPoint = vecMax - vecMin;
    /* The difference between the "Maximum Vertex" and the "Minimum Vertex" is our
    actual Bounds Box */
    
    return FBoxSphereBounds(vecOrigin, BoxPoint, BoxPoint.Size()).TransformBy(LocalToWorld);
}


bool UThriveGeneratedMeshComponent::GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData,
    bool InUseAllTriData)
{
    FTriIndices Triangle;

    for (int32 i = 0; i < GeneratedMeshTris.Num(); i++) {
        const auto& tri = GeneratedMeshTris[i];

        Triangle.v0 = CollisionData->Vertices.Add(tri.Vertex0);
        Triangle.v1 = CollisionData->Vertices.Add(tri.Vertex1);
        Triangle.v2 = CollisionData->Vertices.Add(tri.Vertex2);

        CollisionData->Indices.Add(Triangle);
        CollisionData->MaterialIndices.Add(i);
    }

    CollisionData->bFlipNormals = true;
    CollisionData->bDeformableMesh = true;
    CollisionData->bFastCook = true;

    return true;
}

bool UThriveGeneratedMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const {
    return (GeneratedMeshTris.Num() > 0 && bUsesPhysics);
}

void UThriveGeneratedMeshComponent::UpdateBodySetup() {

    if (ModelBodySetup == NULL) {

        LOG_LOG("updating body setup");
        
        ModelBodySetup = NewObject<UBodySetup>(this);
        ModelBodySetup->BodySetupGuid = FGuid::NewGuid();

        ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
        ModelBodySetup->bMeshCollideAll = true;

        ModelBodySetup->bGenerateMirroredCollision = false;
        //ModelBodySetup->bDoubleSidedGeometry = true;
    }
}

void UThriveGeneratedMeshComponent::UpdateCollision() {

    LOG_LOG("update collision called");

    bool bCreatePhysState = false; // Should we create physics state at the end of this function?

    if (bPhysicsStateCreated)
    {
        DestroyPhysicsState();
        bCreatePhysState = true;
    }

    // Ensure we have a BodySetup
    UpdateBodySetup();

    ModelBodySetup->AggGeom.ConvexElems = OurCollisionConvexElems;

    // New stuff to hopefully fix pawn movement like in UProceduralMeshComponent
    ModelBodySetup->bHasCookedCollisionData = true;

    // New GUID, maybe this updates something
    ModelBodySetup->BodySetupGuid = FGuid::NewGuid();

#if WITH_RUNTIME_PHYSICS_COOKING || WITH_EDITOR

    // Clear current mesh data
    ModelBodySetup->InvalidatePhysicsData();
    // Create new mesh data
    ModelBodySetup->CreatePhysicsMeshes();

#endif // WITH_RUNTIME_PHYSICS_COOKING || WITH_EDITOR

    // Create new instance state if desired
    if (bCreatePhysState)
    {
        CreatePhysicsState();
    }
}

UBodySetup* UThriveGeneratedMeshComponent::GetBodySetup() {

    LOG_LOG("Get body setup called");

    // if (!bUsesPhysics)
    //     return nullptr;

    UpdateBodySetup();
    return ModelBodySetup;
}


