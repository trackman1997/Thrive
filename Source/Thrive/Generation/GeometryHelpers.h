// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GeometryHelpers.generated.h"


//! \todo Add members in here to reduce work in UThriveGeneratedMeshComponent
//! types, needs all FDynamicMeshVertex members to accomplish that
USTRUCT(BlueprintType)
struct FGeneratedTriangle {

    GENERATED_BODY()

public:

    UPROPERTY()
    FVector2D UV0;
    UPROPERTY()
    FVector Vertex0;

    UPROPERTY()
    FVector2D UV1;
    UPROPERTY()
    FVector Vertex1;

    UPROPERTY()
    FVector2D UV2;
    UPROPERTY()
    FVector Vertex2;
};


/*
    When specifying faces vertices need to be in clockwise order
*/
class FGeometryGenerator{
public:

    static void CreateBoxGeometry(int Size, TArray<FGeneratedTriangle> &Receiver, 
        FVector2D UVBottomLeft = FVector2D(0, 0), FVector2D UVTopRight = FVector2D(1, 1));

    static void CreateBoxGeometry(int Size, TArray<FGeneratedTriangle> &Receiver,
        const FVector Offset, FVector2D UVBottomLeft = FVector2D(0, 0),
        FVector2D UVTopRight = FVector2D(1, 1));

};
