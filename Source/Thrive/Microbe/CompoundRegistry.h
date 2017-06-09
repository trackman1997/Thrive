// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "UObject/NoExportTypes.h"

#include "MicrobeCommon.h"

#include "CompoundRegistry.generated.h"


//! Data for a registered compound type
USTRUCT()
struct FCompoundType{

    GENERATED_BODY()

    //! ID of this compound.
    //! If invalid the wanted compound type didn't exist
    ECompoundID ID = ECompoundID::Invalid;
    FName Name;
    FLinearColor Colour;
};

/**
 * 
 */
UCLASS(BlueprintType)
class THRIVE_API UCompoundRegistry : public UObject
{
	GENERATED_BODY()


        
public:
        
    UCompoundRegistry();
        

    //! Returns or registers a new compound with CompoundName
    ECompoundID GetOrRegisterType(const FName &CompoundName);

    //! Returns colour of compound or black
    FLinearColor GetColour(ECompoundID ID) const;
	
private:

    
    //! For returning references to invalid compounds
    FCompoundType InvalidCompound;
};
