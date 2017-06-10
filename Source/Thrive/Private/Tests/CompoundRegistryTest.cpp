// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"

#include "TestHelpers.h"

#include "Microbe/CompoundRegistry.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCompoundRegistryRegister,
    "Thrive.Microbe.CompoundRegistry.Register",
     THRIVE_UNITTEST)

bool FCompoundRegistryRegister::RunTest(const FString &Parameters){

    UCompoundRegistry* Registry = NewObject<UCompoundRegistry>();

    if(Registry->GetCompoundByName(TEXT("Oxygen")) != ECompoundID::Invalid)
        AddError(TEXT("GetCompoundByName('Oxygen') should return invalid before creation"));

    FCompoundType OxygenType;
    OxygenType.Name = TEXT("Oxygen");
    
    TestTrue(TEXT("Register result"), Registry->RegisterCompoundType(OxygenType));

    TestNotEqual(TEXT("Oxygen id changed after register"), OxygenType.ID,
        ECompoundID::Invalid);

    if(Registry->GetCompoundByName(TEXT("Oxygen")) == ECompoundID::Invalid)
        AddError(TEXT("GetCompoundByName('Oxygen') doesn't exist after creation"));

    TestEqual(TEXT("Retrieved and object used for creation have different ids"),
        Registry->GetCompoundData(Registry->GetCompoundByName(TEXT("Oxygen"))).ID,
        OxygenType.ID);

    TestNotSame(TEXT("Retrieved and object used for creation shouldn't be at the "
            "same memory location"), OxygenType,
        Registry->GetCompoundData(Registry->GetCompoundByName(TEXT("Oxygen"))));


    if(Registry->GetCompoundByName(TEXT("Glucose")) != ECompoundID::Invalid)
        AddError(TEXT("GetCompoundByName('Glucose') should return invalid after "
                "creating a different compound"));

    
    return true;
}

