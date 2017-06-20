// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CellPawnMovementComponent.h"


void UCellPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction)
{

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if(!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime) || !PushComponent)
        return;

    // Get and clear movement vector set in ACell::Tick
    FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) *
        DeltaTime * 150.0f;
    
    if(!DesiredMovementThisFrame.IsNearlyZero()){
        
        // FHitResult Hit;
        // SafeMoveUpdatedComponent(DesiredMovementThisFrame,
        //     UpdatedComponent->GetComponentRotation(), true, Hit);

        // // If we bumped into something, try to slide along it
        // if(Hit.IsValidBlockingHit()){
            
        //     SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
        // }

        // Force should always be applied
        PushComponent->AddForce(2000.0f * DesiredMovementThisFrame); 
    }


}


