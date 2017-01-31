#include "microbe_component.h"

#include <luabind/iterator_policy.hpp>
#include <OgreMath.h>
#include <algorithm>
#include "engine/engine.h"
#include "engine/serialization.h"
#include "engine/component_factory.h"
#include "game.h"
#include "microbe_stage/compound_registry.h"

using namespace thrive;

luabind::scope
MicrobeComponent::luaBindings() {
	using namespace luabind;
	return class_<MicrobeComponent, Component>("MicrobeComponent")
		.enum_("ID") [
			value("TYPE_ID", MicrobeComponent::TYPE_ID)
		]
		.scope [
			def("TYPE_NAME", &MicrobeComponent::TYPE_NAME)
		]
		.def(constructor<bool, const std::string&>())

		// Attributes:
		.def_readwrite("speciesName",               &MicrobeComponent::speciesName)
		.def_readwrite("hitpoints",                 &MicrobeComponent::hitpoints)
		.def_readwrite("maxHitpoints",              &MicrobeComponent::maxHitpoints)
		.def_readwrite("dead",                      &MicrobeComponent::dead)
		.def_readwrite("deathTimer",                &MicrobeComponent::deathTimer)
		.def_readwrite("organelles",                &MicrobeComponent::organelles)
		.def_readwrite("specialStorageOrganelles",  &MicrobeComponent::specialStorageOrganelles)
		.def_readwrite("movementDirection",         &MicrobeComponent::movementDirection)
		.def_readwrite("facingTargetPoint",         &MicrobeComponent::facingTargetPoint)
		.def_readwrite("movementFactor",            &MicrobeComponent::movementFactor)
		.def_readwrite("capacity",                  &MicrobeComponent::capacity)
		.def_readwrite("stored",                    &MicrobeComponent::stored)
		.def_readwrite("compounds",                 &MicrobeComponent::compounds)
		.def_readwrite("initialized",               &MicrobeComponent::initialized)
		.def_readwrite("isPlayerMicrobe",           &MicrobeComponent::isPlayerMicrobe)
		.def_readwrite("maxBandwidth",              &MicrobeComponent::maxBandwidth)
		.def_readwrite("remainingBandwidth",        &MicrobeComponent::remainingBandwidth)
		.def_readwrite("compoundCollectionTimer",   &MicrobeComponent::compoundCollectionTimer)
		.def_readwrite("isCurrentlyEngulfing",      &MicrobeComponent::isCurrentlyEngulfing)
		.def_readwrite("isBeingEngulfed",           &MicrobeComponent::isBeingEngulfed)
		.def_readwrite("wasBeingEngulfed",          &MicrobeComponent::wasBeingEngulfed)
		.def_readwrite("hostileEngulfer",           &MicrobeComponent::hostileEngulfer)

		// Methods:
		.def("getBandwidth",        &MicrobeComponent::getBandwidth)
		.def("regenerateBandwidth", &MicrobeComponent::regenerateBandwidth)
		.def("load",                &MicrobeComponent::load)
		.def("storage",             &MicrobeComponent::storage)
	;
}

MicrobeComponent::MicrobeComponent(
    bool _isPlayerMicrobe,
    const std::string& _speciesName
):
    speciesName(_speciesName),
    isPlayerMicrobe(_isPlayerMicrobe) {

	lua_State* lua_state = Game::instance().engine().luaState();

    hitpoints = 0;
    maxHitpoints = 0;
    dead = false;
    deathTimer = 0;
    organelles = luabind::newtable(lua_state);
    specialStorageOrganelles = luabind::newtable(lua_state);
    movementDirection = Ogre::Vector3(0, 0, 0);
    facingTargetPoint = Ogre::Vector3(0, 0, 0);
    movementFactor = 1.0;
    capacity = 0;
    stored = 0;
    compounds = luabind::newtable(lua_state);
    initialized = false;
    maxBandwidth = 10.0 * BANDWIDTH_PER_ORGANELLE;
    remainingBandwidth = 0.0;
    compoundCollectionTimer = EXCESS_COMPOUND_COLLECTION_INTERVAL;
    isCurrentlyEngulfing = false;
    isBeingEngulfed = false;
    wasBeingEngulfed = false;
    //hostileEngulfer = nil
}

Ogre::Real
MicrobeComponent::getBandwidth(
    Ogre::Real maxAmount,
    int compoundId
) {
    int compoundVolume = CompoundRegistry::getCompoundUnitVolume(compoundId);
    int amount = std::min(maxAmount * compoundVolume, remainingBandwidth);
    remainingBandwidth -= amount;
    return amount / compoundVolume;
}

void
MicrobeComponent::regenerateBandwidth(int logicTime) {
    Ogre::Real addedBandwidth = remainingBandwidth + logicTime * (maxBandwidth / BANDWIDTH_REFILL_DURATION);
    remainingBandwidth = std::min(addedBandwidth, maxBandwidth);
}


void
MicrobeComponent::load(const StorageContainer& storage) {
	Component::load(storage);
	lua_State* lua_state = Game::instance().engine().luaState();

	// Loading individual attributes.
	speciesName = storage.get<std::string>("speciesName");
    hitpoints = storage.get<int>("hitpoints");
    maxHitpoints = storage.get<int>("maxHitpoints");
    isPlayerMicrobe = storage.get<bool>("isPlayerMicrobe");
    maxBandwidth = storage.get<Ogre::Real>("maxBandwidth");
    remainingBandwidth = storage.get<Ogre::Real>("remainingBandwidth");

    // Loading the organelle information.
    organelles = luabind::newtable(lua_state);
    specialStorageOrganelles = luabind::newtable(lua_state);
	StorageList orgs = storage.get<StorageList>("organelles");

	for(unsigned i = 1; i <= orgs.size(); i++){
        StorageContainer org = orgs.get(i);

        //Creating the organelle object.
		//luabind::object organelle = luabind::newtable(lua_state);
		//organelle["name"] = org.get<std::string>("name");
		//int q = organelle["q"] = org.get<int>("q");
		//int r = organelle["r"] = org.get<int>("r");
		//organelle["rotation"] = org.get<Ogre::Real>("rotation");

		// Encode axial. TODO: replace with the actual function when hex.lua
		// gets moved to c++.
		//int OFFSET = 256;
		//int SHIFT = OFFSET * 10;
		//int s = (q + OFFSET) * SHIFT + (r + OFFSET);
		//organelles[std::to_string(s)] = organelle;
	}

	// Loading the stored compounds information.
	stored = 0;
    compounds = luabind::newtable(lua_state);
    StorageList storedCompounds = storage.get<StorageList>("storedCompounds");

    for(unsigned i = 1; i <= storedCompounds.size(); i++) {
        StorageContainer compound = storedCompounds.get(i);
        int amount = compound.get<int>("amount");
        compounds[compound.get<std::string>("compoundId")] = amount;
        stored += amount;
    }
}

StorageContainer
MicrobeComponent::storage() const {
	StorageContainer storage = Component::storage();

	// Storing individual attributes.
	storage.set<std::string>("speciesName", speciesName);
    storage.set<int>("hitpoints", hitpoints);
    storage.set<int>("maxHitpoints", maxHitpoints);
    storage.set<bool>("isPlayerMicrobe", isPlayerMicrobe);
    storage.set<Ogre::Real>("maxBandwidth", maxBandwidth);
    storage.set<Ogre::Real>("remainingBandwidth", remainingBandwidth);

    // Storing organelle information.
    StorageList orgs = StorageList();
    storage.set<StorageList>("organelles", orgs);

    // Storing compound information.
    StorageList storedCompounds = StorageList();
    storage.set<StorageList>("storedCompounds", storedCompounds);

	return storage;
}

REGISTER_COMPONENT(MicrobeComponent)
