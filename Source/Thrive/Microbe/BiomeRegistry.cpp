// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"
#include "BiomeRegistry.h"

void UBiomeRegistry::Init(UCompoundRegistry* ACompoundRegistry) {
	CompoundRegistry = ACompoundRegistry;
}

FBiomeType UBiomeRegistry::GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData) {
	FBiomeType NewBiome;

	// Getting the compounds in the biome.
	TSharedPtr<FJsonObject> BiomeCompounds = JsonData->GetObjectField("compounds");
	for (auto Compound : BiomeCompounds->Values) {
		FName compoundName = *Compound.Key;
		int Amount = Compound.Value->AsNumber();
		ECompoundID CompoundId = CompoundRegistry->GetCompoundByName(compoundName);
		NewBiome.Compounds.Add(CompoundId, Amount);
	}

	return NewBiome;
}

void UBiomeRegistry::LoadDefaultBiomes(FString Path) {
	LoadDefaultTypes(Path);
}

bool UBiomeRegistry::RegisterBiomeType(FBiomeType &Properties) {
	return RegisterType(Properties);
}

FBiomeType const& UBiomeRegistry::GetBiomeData(const FName &BiomeName) const {
	return GetTypeData(BiomeName);
}
