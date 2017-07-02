// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"


UCompoundRegistry::UCompoundRegistry(){

    InvalidCompound.InternalName = TEXT("Invalid Compound Type");
    InvalidCompound.Colour = FColor(0, 0, 0, 1);
}
// ------------------------------------ //
void UCompoundRegistry::LoadDefaultCompounds() {
	// Getting the JSON file where the data is.
	FString path = FPaths::GameDir() + "GameData/MicrobeStage/compounds.json";
	FString rawJsonFile = "No chance!";
	FFileHelper::LoadFileToString(rawJsonFile, *path, NULL);
	
	// Deserializing it.
	TSharedPtr<FJsonObject> compounds;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(rawJsonFile);
	FJsonSerializer::Deserialize(Reader, compounds);

	//Getting the data.
	for (auto compound : compounds->Values) {
		FString internalName = compound.Key;
		TSharedPtr<FJsonObject> compoundData = compound.Value->AsObject();
		FCompoundType newCompound;

		newCompound.InternalName = FName(*internalName);
		newCompound.DisplayName = compoundData->GetStringField("name");
		newCompound.volume = compoundData->GetNumberField("volume");
		newCompound.isUseful = compoundData->GetBoolField("isUseful");
		newCompound.isCloud = compoundData->GetBoolField("isCloud");

		// Colour.
		TSharedPtr<FJsonObject> colour = compoundData->GetObjectField("colour");
		float r = colour->GetNumberField("r");
		float g = colour->GetNumberField("g");
		float b = colour->GetNumberField("b");
		newCompound.Colour = FLinearColor(r, g, b, 1.0f);

		RegisterCompoundType(newCompound);
	}
}
// ------------------------------------ //
ECompoundID UCompoundRegistry::GetCompoundByName(const FName &CompoundName) const{

    for(const auto& Compound : RegisteredCompounds){

        if(Compound.InternalName == CompoundName)
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

        if(Compound.InternalName == Properties.InternalName){

            UE_LOG(ThriveLog, Error, TEXT("Compound internal name is already in use: %s"),
                *Properties.InternalName.ToString());
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
    
    //UE_LOG(ThriveLog, Warning, TEXT("Compound by id doesn't exist: %d"),
    //    ID);
    
    return InvalidCompound;
}


