// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "StartingCompoundsRegistry.h"

void UStartingCompoundsRegistry::LoadStartingCompounds(UCompoundRegistry* CompoundRegistry) {
	// Getting the JSON file where the data is.
	FString PathToStartingCompounds = FPaths::GameContentDir() +
		"GameData/MicrobeStage/StartingCompounds.json";

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PathToStartingCompounds)) {

		LOG_FATAL("GameData/MicrobeStage/StartingCompounds.json file is missing");
		return;
	}

	FString RawJsonFile = "No chance!";
	FFileHelper::LoadFileToString(RawJsonFile, *PathToStartingCompounds, 0);

	// Deserializing it.
	TSharedPtr<FJsonObject> StartingCompoundsObject;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(RawJsonFile);
	FJsonSerializer::Deserialize(Reader, StartingCompoundsObject);

	//Getting the data.
	for (auto Compound : StartingCompoundsObject->Values) {
		const FString& InternalNameString = Compound.Key;
		FName InternalName = *InternalNameString;
		float Amount = Compound.Value->AsNumber();

		StartingCompounds.Add(InternalName, Amount);
	}
}
