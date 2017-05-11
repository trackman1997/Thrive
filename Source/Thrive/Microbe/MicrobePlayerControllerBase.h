// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/PlayerController.h"
#include "MicrobePlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class THRIVE_API AMicrobePlayerControllerBase : public APlayerController
{
    GENERATED_BODY()
    
public:

    AMicrobePlayerControllerBase();


    void BeginPlay() override;
    
};
