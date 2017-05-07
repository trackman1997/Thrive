// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "GameFramework/PawnMovementComponent.h"
#include "CellPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class THRIVE_API UCellPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
    //! Update movement
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
        FActorComponentTickFunction *ThisTickFunction) override; 
	
	
};
