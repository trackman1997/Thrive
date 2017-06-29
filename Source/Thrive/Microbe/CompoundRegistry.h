// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "UObject/NoExportTypes.h"

#include "MicrobeCommon.h"

#include "CompoundRegistry.generated.h"


//! Data for a registered compound type
USTRUCT(BlueprintType)
struct FCompoundType{

    GENERATED_BODY()

    //! ID of this compound.
    //! If invalid the wanted compound type didn't exist
    UPROPERTY(BlueprintReadOnly)
    ECompoundID ID = ECompoundID::Invalid;
    
    UPROPERTY(BlueprintReadOnly)
    FName Name;
    
    UPROPERTY(BlueprintReadOnly)
    FLinearColor Colour;

	// How much space does this compound occupy per unit.
	UPROPERTY(BlueprintReadOnly)
	float volume;

	// Whether this compound has any functionality other
	// than a stepping stone to other compounds in the
	// process chain.
	UPROPERTY(BlueprintReadOnly)
	bool isUseful;
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

    //! Loads default compounds
    //! \todo Add a blueprint overrideable method for adding extra things
    void LoadDefaultCompounds();
    
        
    //! Returns compound's id based on the name
    //! \note This should be called once and the result stored for performance reasons
    UFUNCTION(BlueprintCallable)
    ECompoundID GetCompoundByName(const FName &CompoundName) const;

    //! Registers a new compound with the specified properties
    //! \note The ID should be left empty as it will be overwritten
    //! \returns True if succeeded. False if the name is already in use
    UFUNCTION(BlueprintCallable)
    bool RegisterCompoundType(FCompoundType &Properties);


    //! Returns the properties of a compound type. Or InvalidCompound if not found
    //! \note The returned value should NOT be changed
    UFUNCTION(BlueprintCallable)
    FCompoundType const& GetCompoundData(ECompoundID ID) const;
    
    //! Returns colour of a compound or black from the invalid compound
    UFUNCTION(BlueprintCallable)
    FLinearColor GetColour(ECompoundID ID) const{

        return GetCompoundData(ID).Colour;
    }
	
private:

    
    //! For returning references to invalid compounds
    UPROPERTY()
    FCompoundType InvalidCompound;

    //! Registered compounds
    UPROPERTY()
    TArray<FCompoundType> RegisteredCompounds;
};
