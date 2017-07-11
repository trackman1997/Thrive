// Copyright (C) 2013-2017  Revolutionary Games

#pragma once

#include "UObject/NoExportTypes.h"
#include "JsonRegistry.generated.h"

//! Data for a registered bio process type.
USTRUCT(BlueprintType)
struct FJsonRegistryType {

	GENERATED_BODY()

	//! Used to search by name.
	UPROPERTY(BlueprintReadOnly)
	FName InternalName;

	//! Used by the GUI and stuff.
	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;
};

template<class T> class TJsonRegistry {
protected:
	TJsonRegistry();

	//! Loads default types
	//! \todo Add a blueprint overrideable method for adding extra things
	void LoadDefaultTypes(FString Path);

	//! Registers a new type with the specified properties
	//! \returns True if succeeded. False if the name is already in use
	bool RegisterType(T &Properties);

	//! Returns the properties of a type. Or InvalidType if not found
	//! \note The returned value should NOT be changed
	T const& GetTypeData(const FName &TypeName) const;

	//! Registered processes
	TArray<T> RegisteredTypes;

	virtual T GetTypeFromJsonObject(const TSharedPtr<FJsonObject>& JsonData) = 0;

	//! For returning references to invalid bio processes
	T InvalidType;
};



template<class T> TJsonRegistry<T>::TJsonRegistry() {
	InvalidType.InternalName = TEXT("invalid");
	InvalidType.DisplayName = TEXT("Invalid type");
}

template<class T> void TJsonRegistry<T>::LoadDefaultTypes(FString Path) {
	// Getting the JSON file where the data is.
	FString PathToFile = FPaths::GameContentDir() + Path;

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PathToFile)) {

		LOG_FATAL("File missing"); //TODO: add the path to the error message.
		return;
	}

	FString RawJsonFile = "No chance!";
	FFileHelper::LoadFileToString(RawJsonFile, *PathToFile, 0);

	// Deserializing it.
	TSharedPtr<FJsonObject> Types;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(RawJsonFile);
	FJsonSerializer::Deserialize(Reader, Types);

	//Getting the data.
	for (auto Type : Types->Values) {
		const FString& InternalName = Type.Key;
		const TSharedPtr<FJsonObject>& TypeData = Type.Value->AsObject();

		T NewType = GetTypeFromJsonObject(TypeData);
		NewType.InternalName = FName(*InternalName);
		NewType.DisplayName = TypeData->GetStringField("name");

		RegisterType(NewType);
	}
}

template<class T> bool TJsonRegistry<T>::RegisterType(T &Properties) {
	for (const auto& Type : RegisteredTypes) {

		if (Type.InternalName == Properties.InternalName) {

			UE_LOG(ThriveLog, Error, TEXT("Type internal name is already in use: %s"),
				*Properties.InternalName.ToString());
			return false;
		}
	}

	RegisteredTypes.Add(Properties);
	return true;
}

template<class T> T const& TJsonRegistry<T>::GetTypeData(const FName &TypeName) const {

	if (TypeName == TEXT("invalid"))
		return InvalidType;

	for (auto& Type : RegisteredTypes) {

		if (Type.InternalName == TypeName)
			return Type;
	}

	//UE_LOG(ThriveLog, Warning, TEXT("Type by name doesn't exist);

	return InvalidType;
}
