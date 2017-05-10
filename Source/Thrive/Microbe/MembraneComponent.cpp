// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "MembraneComponent.h"



UMembraneComponent::UMembraneComponent() : Super(){
    
}

// ------------------------------------ //
void UMembraneComponent::BeginPlay(){

    Super::BeginPlay();

}

void UMembraneComponent::CreateMembraneMesh(URuntimeMeshComponent* GeometryReceiver){

    if(OrganelleContainerComponent){
        
        
    } else {

        TArray<FVector> Vertices;
        TArray<FVector> Normals;
        TArray<FRuntimeMeshTangent> Tangents;
        TArray<FVector2D> TextureCoordinates;
        TArray<int32> Triangles;

        URuntimeMeshLibrary::CreateBoxMesh(FVector(50, 50, 50), Vertices, Triangles, Normals,
            TextureCoordinates, Tangents);

        // Create the mesh section specifying collision
        GeometryReceiver->CreateMeshSection(0, Vertices, Triangles, Normals, TextureCoordinates,
            TArray<FColor>(), Tangents, true, EUpdateFrequency::Infrequent);

        GeometryReceiver->AddCollisionConvexMesh(Vertices);

        if(GeometryReceiver->GetBodyInstance())
        {
            GeometryReceiver->GetBodyInstance()->bLockXRotation = true;
            GeometryReceiver->GetBodyInstance()->bLockYRotation = true;

            GeometryReceiver->GetBodyInstance()->SetDOFLock(EDOFMode::XYPlane);
        }
    }
}


