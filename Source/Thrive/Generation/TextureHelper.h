// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "Engine/Texture2D.h"
#include <functional>
#include "RHI.h"


class FTextureHelper{
public:

    static void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, 
        uint32 NumRegions, FUpdateTextureRegion2D* Regions,
        uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);
};
