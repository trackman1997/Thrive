// Copyright (C) 2013-2017  Revolutionary Games

#include "Thrive.h"
#include "OrganelleStorage.h"

void UOrganelleStorage::Init(float AStorageCapacity) {
	this->StorageCapacity = AStorageCapacity;
}

//void UOrganelleStorage::OnAddedToMicrobe(ACellBase* AttachedMicrobe, UOrganelleComponent* Organelle) {
	//AttachedMicrobe->CompoundBag->ChangeStorageSpace(StorageCapacity);
//}

void UOrganelleStorage::OnRemovedFromMicrobe() {
	//Microbe->CompoundBag->ChangeStorageSpace(-StorageCapacity);
}

