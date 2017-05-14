// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"

#include "GeometryHelpers.h"

/*
    For some reason unreal procedural mesh wants all normals to be
    facing along positive x-axis (possibly signaling that the actual
    normal is away from the surface)
*/
//#define FORCE_FORWARDS_NORMALS
#define NRML_DUMMY NRML_VEC(1, 0, 0)


// Geometry definition helpers //
#define VRTX_VEC(x, y, z) Receiver.Vertices.Add(FVector(x, y, z));
#define NRML_VEC(x, y, z) Receiver.Normals.Add(FVector(x, y, z));
#define UV0C_VEC(x, y) Receiver.UV0.Add(FVector2D(x, y));

#define FACE_ARR(first, second, third) { Receiver.Triangles.Add(first); \
Receiver.Triangles.Add(second); Receiver.Triangles.Add(third);}



// Dummy definitions for when don't want to get world space normals
#ifdef FORCE_FORWARDS_NORMALS
#define NRML_LEFT NRML_DUMMY
#define NRML_RIGHT NRML_DUMMY
#define NRML_DOWN NRML_DUMMY
#define NRML_UP NRML_DUMMY
#define NRML_INWARDS NRML_DUMMY
#define NRML_OUTWARD NRML_DUMMY
#else
// These seem to work, maybe they are local space normals

//! Left. 
#define NRML_LEFT NRML_VEC(1, 0, 0)
//! Right. 
#define NRML_RIGHT NRML_VEC(1, 0, 0)
//! Down. 
#define NRML_DOWN NRML_VEC(1, 0, 0)
//! Up. 
#define NRML_UP NRML_VEC(1, 0, 0)
//! Into the screen. 
#define NRML_INWARDS NRML_VEC(1, 0, 0)
//! Out from the screen. 
#define NRML_OUTWARD NRML_VEC(1, 0, 0)
#endif // FORCE_FORWARDS_NORMALS

// These should be the actual world space normals //
#ifdef ACTUAL_NORMALS
//! Left. Along negative y-axis
#define NRML_LEFT NRML_VEC(0, -1, 0)
//! Right. Along positive y-axis
#define NRML_RIGHT NRML_VEC(0, 1, 0)
//! Down. Along negative z-axis
#define NRML_DOWN NRML_VEC(0, 0, -1)
//! Up. Along positive z-axis
#define NRML_UP NRML_VEC(0, 0, 1)
//! Into the screen. Along positive x-axis
#define NRML_INWARDS NRML_VEC(1, 0, 0)
//! Out from the screen. Along negative x-axis
#define NRML_OUTWARD NRML_VEC(-1, 0, 0)
#endif //ACTUAL_NORMALS



#define UV0C_0_0 UV0C_VEC(0, 0)
#define UV0C_0_1 UV0C_VEC(0, 1)
#define UV0C_1_0 UV0C_VEC(1, 0)
#define UV0C_1_1 UV0C_VEC(1, 1)

// Defines a group of 3 vertices with different normals //
#define VERTICES_3(x, y, z, AddUVFunc, Normal1Func, Normal2Func, Normal3Func) {\
VRTX_VEC(x, y, z);\
AddUVFunc;\
Normal1Func;\
VRTX_VEC(x, y, z);\
AddUVFunc;\
Normal2Func;\
VRTX_VEC(x, y, z);\
AddUVFunc;\
Normal3Func;\
}

//////////////////////////////////////////////////////////////////////////
void FGeometryGenerator::CreateBoxGeometry(int Size, TArray<FGeneratedTriangle> &Receiver, 
    FVector2D UVBottomLeft /*= FVector2D(0, 0)*/, FVector2D UVTopRight /*= FVector2D(1, 1)*/) 
{
    Receiver.Reserve(Receiver.Num() + 12);
    
    // Front face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, Size, Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, -Size)
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(-Size, -Size, -Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, -Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, Size)
    });

    // Bottom face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, Size, -Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, -Size)
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, -Size, -Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, -Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size)
    });

    // Top face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(Size, Size, Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(Size, -Size, Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, Size)
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(-Size, -Size, Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(Size, -Size, Size)
    });

    // Left face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, -Size, Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, -Size, Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size)
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, -Size, -Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, -Size, Size)
    });

    // Right face //
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, Size, -Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, Size, -Size)
    });
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, Size, Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, Size, -Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, Size)
    });

    // Back face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(Size, -Size, Size),
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(Size, Size, Size),
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, Size, -Size)
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, Size, -Size),
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, -Size, -Size),
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(Size, -Size, Size)
    });


}

void FGeometryGenerator::CreateBoxGeometry(int Size, TArray<FGeneratedTriangle> &Receiver,
    const FVector Offset, FVector2D UVBottomLeft /*= FVector2D(0, 0)*/,
    FVector2D UVTopRight /*= FVector2D(1, 1)*/) 
{
    // Use a more efficient version if not actually using an offset //
    if (Offset == FVector(0))
        return CreateBoxGeometry(Size, Receiver, UVBottomLeft, UVTopRight);

    Receiver.Reserve(Receiver.Num() + 12);

    // Front face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, -Size) + Offset
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(-Size, -Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, -Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, Size) + Offset
    });

    // Bottom face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, Size, -Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, -Size) + Offset
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, -Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, -Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size) + Offset
    });

    // Top face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(Size, Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(Size, -Size, Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, Size) + Offset
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(-Size, -Size, Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(-Size, Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(Size, -Size, Size) + Offset
    });

    // Left face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, -Size, Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, -Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size) + Offset
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, -Size, -Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, -Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, -Size, Size) + Offset
    });

    // Right face //
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, Size, -Size) + Offset
    });
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(-Size, Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(-Size, Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, Size, Size) + Offset
    });

    // Back face //
    Receiver.Add({
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(Size, -Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVTopRight.Y), FVector(Size, Size, Size) + Offset,
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, Size, -Size) + Offset
    });
    Receiver.Add({
        FVector2D(UVBottomLeft.X, UVBottomLeft.Y), FVector(Size, Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVBottomLeft.Y), FVector(Size, -Size, -Size) + Offset,
        FVector2D(UVTopRight.X, UVTopRight.Y), FVector(Size, -Size, Size) + Offset
    });

}

