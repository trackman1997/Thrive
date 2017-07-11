// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"
#include "BioProcessRegistry.h"

void UBioProcessRegistry::Init(UCompoundRegistry* ACompoundRegistry) {
	CompoundRegistry = ACompoundRegistry;
}

FBioProcessType UBioProcessRegistry::GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData) {
	FBioProcessType NewBioProcess;

	// Getting the input information.
	TSharedPtr<FJsonObject> BioProcessInputs = JsonData->GetObjectField("inputs");
	for (auto Input : BioProcessInputs->Values) {
		FName compoundName = *Input.Key;
		int Amount = Input.Value->AsNumber();
		ECompoundID CompoundId = CompoundRegistry->GetCompoundByName(compoundName);
		NewBioProcess.Inputs.Add(CompoundId, Amount);
	}

	// Getting the output information.
	TSharedPtr<FJsonObject> BioProcessOutputs = JsonData->GetObjectField("outputs");
	for (auto Output : BioProcessOutputs->Values) {
		FName compoundName = *Output.Key;
		int Amount = Output.Value->AsNumber();
		ECompoundID CompoundId = CompoundRegistry->GetCompoundByName(compoundName);
		NewBioProcess.Outputs.Add(CompoundId, Amount);
	}

	return NewBioProcess;
}

void UBioProcessRegistry::LoadDefaultBioProcesses(FString Path) {
	LoadDefaultTypes(Path);
}

bool UBioProcessRegistry::RegisterBioProcessType(FBioProcessType &Properties) {
	return RegisterType(Properties);
}

FBioProcessType const& UBioProcessRegistry::GetBioProcessData(const FName &BioProcessName) const {
	return GetTypeData(BioProcessName);
}
