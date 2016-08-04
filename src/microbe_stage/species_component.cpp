#include "species_component.h"

#include <OgreMath.h>
#include "engine/engine.h"
#include "engine/serialization.h"
#include "engine/component_factory.h"
#include "game.h"

#include <iostream>

using namespace thrive;

unsigned int SpeciesComponent::SPECIES_NUM = 0;

luabind::scope
SpeciesComponent::luaBindings() {
	using namespace luabind;
	return class_<SpeciesComponent, Component>("SpeciesComponent")
		.enum_("ID") [
			value("TYPE_ID", SpeciesComponent::TYPE_ID)
		]
		.scope [
			def("TYPE_NAME", &SpeciesComponent::TYPE_NAME)
		]
		.def(constructor<const std::string&>())
		.def_readwrite("name", &SpeciesComponent::name)
		.def_readwrite("colour", &SpeciesComponent::colour)
		.def("load", &SpeciesComponent::load)
		.def("storage", &SpeciesComponent::storage)
		.def("loadOrganelles", &SpeciesComponent::loadOrganelles)
		.def("getOrganelles", &SpeciesComponent::getOrganelles)
		.def("setAvgCompoundAmount", &SpeciesComponent::setAvgCompoundAmount)
		.def("getAvgCompoundAmounts", &SpeciesComponent::getAvgCompoundAmounts)
	;
}

SpeciesComponent::SpeciesComponent(const std::string& _name)
	: colour(1,0,1), name(_name), organelles(), avgCompoundAmounts()
{
	if (name == "") {
		name = "noname" + SPECIES_NUM;
		++SPECIES_NUM;
	}
}

void
SpeciesComponent::load(const StorageContainer& storage) {
	Component::load(storage);
	name = storage.get<std::string>("name");
	colour = storage.get<Ogre::Vector3>("colour");

	organelles.clear();
	StorageContainer orgs = storage.get<StorageContainer>("organelles");

	unsigned int i = 0;
	while (orgs.contains("" + i)) {
		StorageContainer org = orgs.get<StorageContainer>(std::to_string(i++));

		OrganelleData organelle;
		organelle.name = org.get<std::string>("name");
		organelle.q = org.get<int>("q");
		organelle.r = org.get<int>("r");
		organelle.rotation = org.get<float>("rotation");
		organelles.push_back(organelle);
	}

	avgCompoundAmounts.clear();
	StorageContainer amts = storage.get<StorageContainer>("avgCompoundAmounts");

	for (const std::string& k : amts.keys()) {
		avgCompoundAmounts.emplace(std::stoi(k), amts.get<float>(k));
	}
}


StorageContainer
SpeciesComponent::storage() const {
	StorageContainer storage = Component::storage();
	storage.set<std::string>("name", name);
	storage.set<Ogre::Vector3>("colour", colour);

	StorageContainer orgs;

	unsigned int i = 0;
	for (auto organelle : organelles)
    {
        StorageContainer org;
        org.set<std::string>("name", organelle.name);
        org.set<int>("q", organelle.q);
        org.set<int>("r", organelle.r);
        org.set<float>("rotation", organelle.rotation);

        orgs.set<StorageContainer>(std::to_string(i++), org);
	}

	storage.set<StorageContainer>("organelles", orgs);

	StorageContainer amts;
	for (auto amount : avgCompoundAmounts)
    {
        amts.set<float>(std::to_string(amount.first), amount.second);
	}

	storage.set<StorageContainer>("avgCompoundAmounts", amts);
	return storage;
}


void SpeciesComponent::loadOrganelles(const luabind::object& luaOrganelles)
{
    organelles.clear();
    for (luabind::iterator i(luaOrganelles), end; i != end; ++i)
    {
        OrganelleData organelle;
        luabind::object data = *i;
        organelle.name = luabind::object_cast<std::string>(data["name"]);
        organelle.q = luabind::object_cast<int>(data["q"]);
        organelle.r = luabind::object_cast<int>(data["r"]);
        organelle.rotation = luabind::object_cast<float>(data["rotation"]);

		organelles.push_back(organelle);
    }
}

luabind::object SpeciesComponent::getOrganelles()
{
    luabind::object organelleContainer = luabind::newtable(Game::instance().engine().luaState());

    unsigned int i = 0;
	for (auto organelle : organelles)
    {
        luabind::object orgdata = luabind::newtable(Game::instance().engine().luaState());
        orgdata["name"] = organelle.name;
        orgdata["q"] = organelle.q;
        orgdata["r"] = organelle.r;
        orgdata["rotation"] = organelle.rotation;

        organelleContainer[std::to_string(i++)] = orgdata;
	}

    return organelleContainer;
}

void SpeciesComponent::setAvgCompoundAmount(int compoundId, const luabind::object& compoundData)
{
    auto search = avgCompoundAmounts.find(compoundId);
    if (search != avgCompoundAmounts.end())
    {
        search->second = luabind::object_cast<float>(compoundData["amount"]);
    }
    else
    {
        avgCompoundAmounts.emplace(compoundId, luabind::object_cast<float>(compoundData["amount"]));
    }
}

luabind::object SpeciesComponent::getAvgCompoundAmounts()
{
    luabind::object amts = luabind::newtable(Game::instance().engine().luaState());

	for (auto amount : avgCompoundAmounts)
    {
        amts[amount.first] = amount.second;
	}

    return amts;
}

REGISTER_COMPONENT(SpeciesComponent)
