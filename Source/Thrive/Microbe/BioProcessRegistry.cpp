// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "CompoundRegistry.h"
#include "BioProcessRegistry.h"

UBioProcessRegistry::UBioProcessRegistry() {
	InvalidBioProcess.InternalName = TEXT("Invalid Bio Process Type");
}

void UBioProcessRegistry::LoadDefaultBioProcesses(UCompoundRegistry* CompoundRegistry) {
	// Getting the JSON file where the data is.
	FString PathToBioProcesses = FPaths::GameContentDir() +
		"GameData/MicrobeStage/BioProcesses.json";

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PathToBioProcesses)) {

		LOG_FATAL("GameData/MicrobeStage/BioProcesses.json file is missing");
		return;
	}

	FString RawJsonFile = "No chance!";
	FFileHelper::LoadFileToString(RawJsonFile, *PathToBioProcesses, 0);

	// Deserializing it.
	TSharedPtr<FJsonObject> BioProcesses;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(RawJsonFile);
	FJsonSerializer::Deserialize(Reader, BioProcesses);

	//Getting the data.
	for (auto BioProcess : BioProcesses->Values) {
		const FString& InternalName = BioProcess.Key;
		const TSharedPtr<FJsonObject>& BioProcessData = BioProcess.Value->AsObject();
		FBioProcessType NewBioProcess;

		NewBioProcess.InternalName = FName(*InternalName);
		NewBioProcess.DisplayName = BioProcessData->GetStringField("name");

		// Getting the input information.
		TSharedPtr<FJsonObject> BioProcessInputs = BioProcessData->GetObjectField("inputs");
		for (auto Input : BioProcessInputs->Values) {
			FName compoundName = *Input.Key;
			int Amount = Input.Value->AsNumber();
			ECompoundID CompoundId = CompoundRegistry->GetCompoundByName(compoundName);
			NewBioProcess.Inputs.Add(CompoundId, Amount);
		}

		// Getting the output information.
		TSharedPtr<FJsonObject> BioProcessOutputs = BioProcessData->GetObjectField("outputs");
		for (auto Output : BioProcessOutputs->Values) {
			FName compoundName = *Output.Key;
			int Amount = Output.Value->AsNumber();
			ECompoundID CompoundId = CompoundRegistry->GetCompoundByName(compoundName);
			NewBioProcess.Outputs.Add(CompoundId, Amount);
		}

		RegisterBioProcessType(NewBioProcess);
	}
}

EBioProcessID UBioProcessRegistry::GetBioProcessByName(const FName &BioProcessName) const {

	for (const auto& BioProcess : RegisteredBioProcesses) {

		if (BioProcess.InternalName == BioProcessName)
			return BioProcess.ID;
	}

	return EBioProcessID::Invalid;
}

bool UBioProcessRegistry::RegisterBioProcessType(FBioProcessType &Properties) {

	// Verify the name is not used and find the highest bio process id //
	int32_t HighestUsed = 0;

	for (const auto& BioProcess : RegisteredBioProcesses) {

		const auto IDAsInt = static_cast<decltype(HighestUsed)>(BioProcess.ID);
		if (HighestUsed < IDAsInt)
			HighestUsed = IDAsInt;

		if (BioProcess.InternalName == Properties.InternalName) {

			UE_LOG(ThriveLog, Error, TEXT("Bio process internal name is already in use: %s"),
				*Properties.InternalName.ToString());
			return false;
		}
	}

	Properties.ID = static_cast<EBioProcessID>(++HighestUsed);
	RegisteredBioProcesses.Add(Properties);
	return true;
}

FBioProcessType const& UBioProcessRegistry::GetBioProcessData(EBioProcessID ID) const {

	if (ID == EBioProcessID::Invalid)
		return InvalidBioProcess;

	for (auto& BioProcess : RegisteredBioProcesses) {

		if (BioProcess.ID == ID)
			return BioProcess;
	}

	//UE_LOG(ThriveLog, Warning, TEXT("Bio process by id doesn't exist: %d"),
	//    ID);

	return InvalidBioProcess;
}