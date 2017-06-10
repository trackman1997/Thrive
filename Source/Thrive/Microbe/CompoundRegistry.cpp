// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"


UCompoundRegistry::UCompoundRegistry(){

    InvalidCompound.Name = TEXT("Invalid Compound Type");
    InvalidCompound.Colour = FColor(0, 0, 0, 1);
}
// ------------------------------------ //
ECompoundID UCompoundRegistry::GetCompoundByName(const FName &CompoundName) const{

    for(const auto& Compound : RegisteredCompounds){

        if(Compound.Name == CompoundName)
            return Compound.ID;
    }
    
    return ECompoundID::Invalid;
}

bool UCompoundRegistry::RegisterCompoundType(FCompoundType &Properties){

    // Verify the name is not used and find the highest compound id //
    int32_t HighestUsed = 0;
    
    for(const auto& Compound : RegisteredCompounds){

        const auto IDAsInt = static_cast<decltype(HighestUsed)>(Compound.ID);
        if(HighestUsed < IDAsInt)
            HighestUsed = IDAsInt;

        if(Compound.Name == Properties.Name){

            UE_LOG(ThriveLog, Error, TEXT("Compound name is already in use: %s"),
                *Properties.Name.ToString());
            return false;
        }
    }

    Properties.ID = static_cast<ECompoundID>(++HighestUsed);
    RegisteredCompounds.Add(Properties);
    return true;
}
// ------------------------------------ //
FCompoundType const& UCompoundRegistry::GetCompoundData(ECompoundID ID) const{

    if(ID == ECompoundID::Invalid)
        return InvalidCompound;

    for(auto& Compound : RegisteredCompounds){

        if(Compound.ID == ID)
            return Compound;
    }
    
    UE_LOG(ThriveLog, Warning, TEXT("Compound by id doesn't exist: %d"),
        ID);
    
    return InvalidCompound;
}


