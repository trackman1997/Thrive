// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"
#include "BiomeRegistry.h"


UBiomeRegistry::UBiomeRegistry() {
	InvalidBiome.InternalName = TEXT("Invalid Biome Type");
}

EBiomeID UBiomeRegistry::GetBiomeByName(const FName &BiomeName) const {

	for (const auto& Biome : RegisteredBiomes) {

		if (Biome.InternalName == BiomeName)
			return Biome.ID;
	}

	return EBiomeID::Invalid;
}

void UBiomeRegistry::LoadDefaultBiomes(UCompoundRegistry* CompoundRegistry) {
	// Getting the JSON file where the data is.
	FString PathToBiomes = FPaths::GameContentDir() +
		"GameData/MicrobeStage/Biomes.json";

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PathToBiomes)) {

		LOG_FATAL("GameData/MicrobeStage/Biomes.json file is missing");
		return;
	}

	FString RawJsonFile = "No chance!";
	FFileHelper::LoadFileToString(RawJsonFile, *PathToBiomes, 0);

	// Deserializing it.
	TSharedPtr<FJsonObject> Biomes;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(RawJsonFile);
	FJsonSerializer::Deserialize(Reader, Biomes);

	//Getting the data.
	for (auto Biome : Biomes->Values) {
		const FString& InternalName = Biome.Key;
		const TSharedPtr<FJsonObject>& BiomeData = Biome.Value->AsObject();
		FBiomeType NewBiome;

		NewBiome.InternalName = FName(*InternalName);
		NewBiome.DisplayName = BiomeData->GetStringField("name");

		// Getting the compounds in the biome.
		TSharedPtr<FJsonObject> BiomeCompounds = BiomeData->GetObjectField("compounds");
		for (auto Compound : BiomeCompounds->Values) {
			FName compoundName = *Compound.Key;
			int Amount = Compound.Value->AsNumber();
			ECompoundID CompoundId = CompoundRegistry->GetCompoundByName(compoundName);
			NewBiome.Compounds.Add(CompoundId, Amount);
		}

		RegisterBiomeType(NewBiome);
	}
}

bool UBiomeRegistry::RegisterBiomeType(FBiomeType &Properties) {

	// Verify the name is not used and find the highest biome id //
	int32_t HighestUsed = 1;

	for (const auto& Biome : RegisteredBiomes) {

		const auto IDAsInt = static_cast<decltype(HighestUsed)>(Biome.ID);
		if (HighestUsed < IDAsInt)
			HighestUsed = IDAsInt;

		if (Biome.InternalName == Properties.InternalName) {

			UE_LOG(ThriveLog, Error, TEXT("Biome internal name is already in use: %s"),
				*Properties.InternalName.ToString());
			return false;
		}
	}

	Properties.ID = static_cast<EBiomeID>(++HighestUsed);
	RegisteredBiomes.Add(Properties);
	return true;
}

FBiomeType const& UBiomeRegistry::GetBiomeData(EBiomeID ID) const {

	if (ID == EBiomeID::Invalid)
		return InvalidBiome;

	for (auto& Biome : RegisteredBiomes) {

		if (Biome.ID == ID)
			return Biome;
	}

	//UE_LOG(ThriveLog, Warning, TEXT("Bio process by id doesn't exist: %d"),
	//    ID);

	return InvalidBiome;
}
