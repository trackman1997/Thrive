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
	FString PathToCompounds = FPaths::GameContentDir() +
        "GameData/MicrobeStage/Compounds.json";

    if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PathToCompounds)){

        LOG_FATAL("GameData/MicrobeStage/Compounds.json file is missing");
        return;
    }
    
	FString RawJsonFile = "No chance!";
	FFileHelper::LoadFileToString(RawJsonFile, *PathToCompounds, 0);
	
	// Deserializing it.
	TSharedPtr<FJsonObject> Compounds;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(RawJsonFile);
	FJsonSerializer::Deserialize(Reader, Compounds);

	//Getting the data.
	for (auto Compound : Compounds->Values) {
		const FString& InternalName = Compound.Key;
		const TSharedPtr<FJsonObject>& CompoundData = Compound.Value->AsObject();
		FCompoundType NewCompound;

		NewCompound.InternalName = FName(*InternalName);
		NewCompound.DisplayName = CompoundData->GetStringField("name");
		NewCompound.Volume = CompoundData->GetNumberField("volume");
		NewCompound.bUseful = CompoundData->GetBoolField("isUseful");
		NewCompound.bCloud = CompoundData->GetBoolField("isCloud");

		// Colour.
		TSharedPtr<FJsonObject> Colour = CompoundData->GetObjectField("colour");
		float R = Colour->GetNumberField("r");
		float G = Colour->GetNumberField("g");
		float B = Colour->GetNumberField("b");
		NewCompound.Colour = FLinearColor(R, G, B, 1.0f);

		RegisterCompoundType(NewCompound);
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


