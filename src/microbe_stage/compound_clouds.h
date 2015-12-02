#pragma once

#include "engine/component.h"
#include "engine/system.h"
#include "engine/touchable.h"
#include "scripting/luabind.h"
#include "engine/typedefs.h"

#include <luabind/object.hpp>
#include <memory>
#include <OgreCommon.h>
#include <OgreMath.h>
#include <OgreVector3.h>
#include <unordered_set>
#include <vector>

namespace thrive {

class FluidSystem;

/**
* @brief Emitter for compound particles
*/
class CompoundCloudComponent : public Component {
    COMPONENT(CompoundCloudComponent)

public:

    /**
    * @brief Lua bindings
    *
    * Exposes:
    * - CompoundCloudComponent()
    * - CompoundCloudComponent::m_emissionRadius
    *
    * @return
    */
    static luabind::scope
    luaBindings();


    void
    load(
        const StorageContainer& storage
    ) override;

    StorageContainer
    storage() const override;

	bool initialized = false;

    // Decide where to place the compound clouds and how many particles.
    void initialize(int amount, int x, int y);


private:
    friend class FluidSystem;
    friend class CompoundCloudSystem;

    // Cloud stats.
	int amount; // Number of particles in cloud.
	int totalAge; // The total age of the cloud.
	float density; // How fast the cloud moves in relation to the velocity field.

	// Individual particle stats.
	std::vector<Ogre::Vector3> particles;
	std::vector<int> age;

	Ogre::ParticleSystem* cloud;
};

/**
* @brief Creates particles for the compound cloud
*/
class CompoundCloudSystem : public System {

public:

    /**
    * @brief Lua bindings
    *
    * Exposes:
    * - CompoundCloudSystem()
    *
    * @return
    */
    static luabind::scope
    luaBindings();

    /**
    * @brief Constructor
    */
    CompoundCloudSystem();

    /**
    * @brief Destructor
    */
    ~CompoundCloudSystem();

    /**
    * @brief Initializes the system
    *
    * @param gameState
    */
    void init(GameState* gameState) override;

    /**
    * @brief Shuts the system down
    */
    void shutdown() override;

    /**
    * @brief Updates the system
    */
    void update(int, int) override;

private:

    struct Implementation;
    std::unique_ptr<Implementation> m_impl;
};

}

