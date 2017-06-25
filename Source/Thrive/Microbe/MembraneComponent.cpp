// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "MembraneComponent.h"

#include <cmath>


UMembraneComponent::UMembraneComponent() : Super(){

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MembraneMaterialFinder(
        TEXT("/Game/Microbe/Materials/MembraneMaterial"));

    if(MembraneMaterialFinder.Object)
        MembraneMaterialBase = MembraneMaterialFinder.Object;
}

// ------------------------------------ //
void UMembraneComponent::BeginPlay(){

    Super::BeginPlay();

}

void UMembraneComponent::CreateMembraneMesh(URuntimeMeshComponent* GeometryReceiver, TArray<FVector2D>& points){

    if(OrganelleContainerComponent){

        // TODO: apply organelle data
        
    }

    // Dummy data
	/*
    for(int x = -4; x < 5; ++x){

        for(int y = -4; y < 5; ++y){

            OrganellePositions.Emplace(FVector2D(x, y));
        }
    }
	*/

	for (FVector2D const& point : points) {
		OrganellePositions.Emplace(point);
	}

    // OrganellePositions.Emplace(FVector2D(0, 6));
    // OrganellePositions.Emplace(FVector2D(0, 7));
    // OrganellePositions.Emplace(FVector2D(1, 7));
    // OrganellePositions.Emplace(FVector2D(1, 6));

    // Generate mesh data //

    MeshPoints.Empty();

    // This initializes the 2D points and then starts moving them towards organelles
    DoMembraneIterativeShrink();

    // One more iteration
    DrawMembrane();

    // Setup 3D points
    MakePrism();
    CalcUVCircle();


    // Create renderable from the mesh data //


    TArray<FVector> Normals;
    TArray<FRuntimeMeshTangent> Tangents;
    
    TArray<int32> Triangles;

    // URuntimeMeshLibrary::CreateBoxMesh(FVector(50, 50, 50), Vertices, Triangles, Normals,
    //     TextureCoordinates, Tangents);

    TArray<FVector>& Vertices = MeshPoints;
    TArray<FVector2D>& TextureCoordinates = UVs;

    Triangles.SetNum(Vertices.Num());

    for(size_t i = 0; i < Triangles.Num(); ++i){

        // 1 to 1 vertex mapping to traingles
        Triangles[i] = i;
        
    }

    // Create the mesh section specifying collision
    GeometryReceiver->CreateMeshSection(0, Vertices, Triangles, Normals, TextureCoordinates,
        // The membrane is animated using the vertex buffer (once
        // that's done) so this should be marked as frequently updated
        TArray<FColor>(), Tangents, true, EUpdateFrequency::Frequent);

    GeometryReceiver->AddCollisionConvexMesh(Vertices);

    if(GeometryReceiver->GetBodyInstance())
    {
        // All of this doesn't seem to be enough
        GeometryReceiver->GetBodyInstance()->bLockZTranslation = true;
        GeometryReceiver->GetBodyInstance()->bLockXRotation = true;
        GeometryReceiver->GetBodyInstance()->bLockYRotation = true;

        GeometryReceiver->GetBodyInstance()->SetDOFLock(EDOFMode::XYPlane);
    }


    if(!MembraneMaterialBase){

        LOG_ERROR("Membrane didn't find the base material for membrane");
        
    } else {

        // TODO: cache this for future updates where this can be reused
        // also this could be moved to a species class to share one instance between all cells
        // of the same species
        UMaterialInstanceDynamic* DynMaterial =
            UMaterialInstanceDynamic::Create(MembraneMaterialBase, this);

        // Set colour //
        // TODO: this also needs to be called only when the colour changes
        DynMaterial->SetVectorParameterValue("MembraneColourTint", MembraneColourTint);
        
        GeometryReceiver->SetMaterial(0, DynMaterial);
    }

}

// ------------------------------------ //
void UMembraneComponent::DoMembraneIterativeShrink(){

    // Reset dimensions before calculating the new required dimensions
    CellDimensions = 10;

    for (FVector2D pos : OrganellePositions) {
        if (std::fabs(pos.X) + 1 > CellDimensions) {
            CellDimensions = std::abs(pos.X) + 1;
        }
        if (std::fabs(pos.Y) + 1 > CellDimensions) {
            CellDimensions = std::abs(pos.Y) + 1;
        }
    }

    Vertices2D.Empty();

    for(int i = 0; i < MembraneResolution; i++)
    {
        Vertices2D.Emplace(-CellDimensions + 2*CellDimensions/MembraneResolution*i,
            -CellDimensions);
    }
    for(int i = 0; i < MembraneResolution; i++)
    {
        Vertices2D.Emplace(CellDimensions,
            -CellDimensions + 2*CellDimensions/MembraneResolution*i);
    }
    for(int i = 0; i < MembraneResolution; i++)
    {
        Vertices2D.Emplace(CellDimensions - 2*CellDimensions/MembraneResolution*i,
            CellDimensions);
    }
    for(int i = 0; i < MembraneResolution; i++)
    {
        Vertices2D.Emplace(-CellDimensions,
            CellDimensions - 2*CellDimensions/MembraneResolution*i);
    }

    for(int i = 0; i < 50*CellDimensions; i++)
    {
        DrawMembrane();
    }
}
    
void UMembraneComponent::DrawMembrane(){

    // Stores the temporary positions of the membrane.
    NewPositions = Vertices2D;

    // Loops through all the points in the membrane and relocates them as necessary.
    for(size_t i = 0, end = NewPositions.Num(); i < end; i++)
    {
        const auto ClosestOrganelle = FindClosestOrganelles(Vertices2D[i]);
        
        if(!std::get<1>(ClosestOrganelle))
        {
            NewPositions[i] = (Vertices2D[(end+i-1)%end] + Vertices2D[(i+1)%end])/2;
        }
        else
        {
            const auto movementDirection = GetMovement(Vertices2D[i],
                std::get<0>(ClosestOrganelle));
            
            NewPositions[i].X -= movementDirection.X;
            NewPositions[i].Y -= movementDirection.Y;
        }
    }

    // Allows for the addition and deletion of points in the membrane.
    for(size_t i = 0; i < NewPositions.Num() - 1; i++)
    {
        // Check to see if the gap between two points in the membrane is too big.
        if(FVector2D::Distance(NewPositions[i], NewPositions[(i+1)%NewPositions.Num()]) >
            CellDimensions/MembraneResolution)
        {
            // Add an element after the ith term that is the average of the i and i+1 term.
            const auto tempPoint = (NewPositions[(i + 1) % NewPositions.Num()] +
                NewPositions[i])/2;
            
            NewPositions.Insert(tempPoint, i+1);

            i++;
        }

        // Check to see if the gap between two points in the membrane is too small.
        if(FVector2D::Distance(NewPositions[(i+1)%NewPositions.Num()], (NewPositions[(i-1) %
                        NewPositions.Num()])) < CellDimensions/MembraneResolution)
        {
            // Delete the ith term.
            NewPositions.RemoveAt(i);
        }
    }

    Vertices2D = NewPositions;    
}

void UMembraneComponent::MakePrism(){

    MeshPoints.Empty();

    for(size_t i = 0, end = Vertices2D.Num(); i < end; i++)
    {
        MeshPoints.Emplace(Vertices2D[i%end].X * ScaleUp, Vertices2D[i%end].Y * ScaleUp,
            +Height/2);
        
        MeshPoints.Emplace(Vertices2D[(i+1)%end].X * ScaleUp,
            Vertices2D[(i+1)%end].Y * ScaleUp,
            -Height/2);
        
        MeshPoints.Emplace(Vertices2D[i%end].X * ScaleUp, Vertices2D[i%end].Y * ScaleUp,
            -Height/2);
        
        MeshPoints.Emplace(Vertices2D[i%end].X * ScaleUp, Vertices2D[i%end].Y * ScaleUp,
            +Height/2);
        
        MeshPoints.Emplace(Vertices2D[(i+1)%end].X * ScaleUp,
            Vertices2D[(i+1)%end].Y * ScaleUp,
            +Height/2);
        
        MeshPoints.Emplace(Vertices2D[(i+1)%end].X * ScaleUp,
            Vertices2D[(i+1)%end].Y * ScaleUp,
            -Height/2);
    }

    for(size_t i = 0, end = Vertices2D.Num(); i < end; i++)
    {
        MeshPoints.Emplace(Vertices2D[i%end].X * ScaleUp, Vertices2D[i%end].Y * ScaleUp,
            +Height/2);
        
        MeshPoints.Emplace(0,0,Height/2);
        
        MeshPoints.Emplace(Vertices2D[(i+1)%end].X * ScaleUp, Vertices2D[(i+1)%end].Y * ScaleUp,
            +Height/2);

        MeshPoints.Emplace(Vertices2D[i%end].X * ScaleUp, Vertices2D[i%end].Y * ScaleUp,
            -Height/2);
        
        MeshPoints.Emplace(Vertices2D[(i+1)%end].X * ScaleUp,
            Vertices2D[(i+1)%end].Y * ScaleUp,
            -Height/2);
        
        MeshPoints.Emplace(0,0,-Height/2);
    }
}

void UMembraneComponent::CalcUVCircle(){

    UVs.Empty();

    for(size_t i = 0, end = MeshPoints.Num(); i < end; i++)
    {
        double x, y, z, a, b, c;
        x = MeshPoints[i].X;
        y = MeshPoints[i].Y;
        z = MeshPoints[i].Z;

        double ray = x*x + y*y + z*z;

        double t = std::sqrt(ray)/(2.0*ray);
        a = t*x;
        b = t*y;
        c = t*z;

        UVs.Emplace(a+0.5,b+0.5// ,c+0.5
        );
    }
}

// ------------------------------------ //
std::tuple<FVector2D, bool> UMembraneComponent::FindClosestOrganelles(FVector2D Target){
    
    // The distance we want the membrane to be from the organelles squared.
    double closestSoFar = 4;
    int closestIndex = -1;

    for (size_t i = 0, end = OrganellePositions.Num(); i < end; i++)
    {
        double lenToObject =  FVector2D::DistSquared(Target, OrganellePositions[i]);

        if(lenToObject < 4 && lenToObject < closestSoFar)
        {
            closestSoFar = lenToObject;

            closestIndex = i;
        }
    }

    if(closestIndex != -1)
        return std::make_tuple(OrganellePositions[closestIndex], true);
    else
        return std::make_tuple(FVector2D(0, 0), false);
    
}

FVector2D UMembraneComponent::GetMovement(FVector2D Target, FVector2D ClosestOrganelle){

    double power = pow(2.7, FVector2D::Distance(-Target, ClosestOrganelle)/10)/50;

    return (ClosestOrganelle - Target)*power;
}

