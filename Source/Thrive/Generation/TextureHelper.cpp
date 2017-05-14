// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"

#include "TextureHelper.h"

//! Updates dynamic texture.
//! \see https://wiki.unrealengine.com/Dynamic_Textures
void FTextureHelper::UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex,
    uint32 NumRegions, FUpdateTextureRegion2D* Regions,
    uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData
    //std::function<void()> Callback
) {
    if(Texture && Texture->Resource)
    {
        struct FUpdateTextureRegionsData {
            FTexture2DResource* Texture2DResource;
            int32 MipIndex;
            uint32 NumRegions;
            FUpdateTextureRegion2D* Regions;
            uint32 SrcPitch;
            uint32 SrcBpp;
            uint8* SrcData;
        };

        FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

        RegionData->Texture2DResource = (FTexture2DResource*)Texture->Resource;
        RegionData->MipIndex = MipIndex;
        RegionData->NumRegions = NumRegions;
        RegionData->Regions = Regions;
        RegionData->SrcPitch = SrcPitch;
        RegionData->SrcBpp = SrcBpp;
        RegionData->SrcData = SrcData;

        //ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
        ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
            UpdateTextureRegionsData,
            FUpdateTextureRegionsData*, RegionData, RegionData,
            bool, bFreeData, bFreeData,
                //std::function<void ()>, Callback, Callback,
            {
                for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
                {
                    int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
                    if (RegionData->MipIndex >= CurrentFirstMip)
                    {
                        RHIUpdateTexture2D(
                            RegionData->Texture2DResource->GetTexture2DRHI(),
                            RegionData->MipIndex - CurrentFirstMip,
                            RegionData->Regions[RegionIndex],
                            RegionData->SrcPitch,
                            RegionData->SrcData
                            + RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
                            + RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
                            );
                    }
                }

                if (bFreeData)
                {
                    FMemory::Free(RegionData->SrcData);
                }

                FMemory::Free(RegionData->Regions);
                delete RegionData;

                // if (Callback)
                //     Callback();
            });
    }
}
