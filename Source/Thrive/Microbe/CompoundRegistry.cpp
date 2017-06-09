// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"


UCompoundRegistry::UCompoundRegistry(){

    InvalidCompound.Name = TEXT("Invalid Compound Type");
    InvalidCompound.Colour = FColor(0, 0, 0, 1);
}
// ------------------------------------ //
ECompoundID UCompoundRegistry::GetOrRegisterType(const FName &CompoundName){

    unimplemented();
    return ECompoundID::Invalid;
}

FLinearColor UCompoundRegistry::GetColour(ECompoundID ID) const{

    // Not found
    UE_LOG(ThriveLog, Warning, TEXT("Compound not defined: %d while getting colour"), ID);
    return InvalidCompound.Colour;
}

