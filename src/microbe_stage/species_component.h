#pragma once

#include <luabind/object.hpp>
#include <luabind/iterator_policy.hpp>
#include "scripting/luabind.h"
#include "engine/component.h"
#include <OgreVector3.h>
#include <string>
#include <vector>
#include <map>

namespace thrive {

class SpeciesComponent : public Component {
	COMPONENT(SpeciesComponent)

struct OrganelleData
{
    std::string name;
    int q;
    int r;
    float rotation;
};

public:
	static luabind::scope
	luaBindings();

	SpeciesComponent(const std::string& _name = "");

	Ogre::Vector3 colour;
	std::string name;
	std::vector<OrganelleData> organelles;
	std::map<int, float> avgCompoundAmounts;

    void
    load(
        const StorageContainer& storage
    ) override;

    StorageContainer
    storage() const override;

    void loadOrganelles(const luabind::object& organelles);
    luabind::object getOrganelles();
    void setAvgCompoundAmount(int compoundId, const luabind::object& compoundAmt);
    luabind::object getAvgCompoundAmounts();

private:
	static unsigned int SPECIES_NUM;
};

}
