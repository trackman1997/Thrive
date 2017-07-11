// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "OrganelleRegistry.h"

void UOrganelleRegistry::Init(UCompoundRegistry* ACompoundRegistry) {
	CompoundRegistry = ACompoundRegistry;
}

FOrganelleType UOrganelleRegistry::GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData) {
	FOrganelleType NewOrganelle;

	NewOrganelle.Mass = JsonData->GetNumberField("mass");
	NewOrganelle.Gene = JsonData->GetStringField("gene");
	NewOrganelle.MPCost = JsonData->GetNumberField("mpCost");

	// Getting the organelle composition.
	TSharedPtr<FJsonObject> Composition = JsonData->GetObjectField("composition");
	for (auto const& Compound : Composition->Values) {
		FName CompoundName = *Compound.Key;
		int Amount = Compound.Value->AsNumber();
		NewOrganelle.Composition.Add(CompoundName, Amount);
	}

	return NewOrganelle;
}

void UOrganelleRegistry::LoadDefaultOrganelles(FString Path) {
	LoadDefaultTypes(Path);
}

bool UOrganelleRegistry::RegisterOrganelleType(FOrganelleType &Properties) {
	return RegisterType(Properties);
}

FOrganelleType const& UOrganelleRegistry::GetOrganelleData(const FName &OrganelleName) const {
	return GetTypeData(OrganelleName);
}

TArray<FOrganelleType> const& UOrganelleRegistry::GetRegisteredOrganelles() const {
	return RegisteredTypes;
}
