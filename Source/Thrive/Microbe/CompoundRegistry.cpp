// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"


FCompoundType UCompoundRegistry::GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData) {
	FCompoundType NewCompound;

	NewCompound.Volume = JsonData->GetNumberField("volume");
	NewCompound.bUseful = JsonData->GetBoolField("isUseful");
	NewCompound.bCloud = JsonData->GetBoolField("isCloud");

	// Colour.
	TSharedPtr<FJsonObject> Colour = JsonData->GetObjectField("colour");
	float R = Colour->GetNumberField("r");
	float G = Colour->GetNumberField("g");
	float B = Colour->GetNumberField("b");
	NewCompound.Colour = FLinearColor(R, G, B, 1.0f);

	return NewCompound;
}

void UCompoundRegistry::LoadDefaultCompounds(FString Path) {
	LoadDefaultTypes(Path);
}

bool UCompoundRegistry::RegisterCompoundType(FCompoundType &Properties){
	return RegisterType(Properties);
}

FCompoundType const& UCompoundRegistry::GetCompoundData(const FName &CompoundName) const{
	return GetTypeData(CompoundName);
}

TArray<FCompoundType> const& UCompoundRegistry::GetRegisteredCompounds() const {
	return RegisteredTypes;
}
