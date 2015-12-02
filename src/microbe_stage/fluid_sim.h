#pragma once

//-----------------------------------------------------------------------
//
//  Name: fluid_sim.h
//
//  Author: Michael Silver, 2015
//
//  Desc: Creates realistic fluid flow from curl noise.
//
//------------------------------------------------------------------------
#include "engine/component.h"
#include "engine/system.h"
#include "engine/touchable.h"
#include "scripting/luabind.h"
#include "engine/typedefs.h"
#include "engine/rolling_grid.h"
#include "ogre/scene_node_system.h"

#include <luabind/object.hpp>
#include <memory>
#include <OgreCommon.h>
#include <OgreMath.h>
#include <OgreVector3.h>
#include <unordered_set>
#include <vector>
#include "compound_clouds.h"
#include "PerlinNoise.h"


namespace thrive {

class FluidComponent {

private:
    // Noise module.
	PerlinNoise fieldPotential;

	float nxScale, nyScale;
	float fx1, fy1, fx2, fy2, n1, n2, nx, ny, dx, dy;

public:
    // Sets all the parameters.
	FluidComponent(int width, int height, float noiseScale);

	// A dynamically moving array of velocities.
	RollingGrid xGrid;
	RollingGrid yGrid;

	// Calculates the velocity at each point on the screen using the curl of a noise field.
	void CreateVelocityField();
	// Re-calculates the velocity at each point on the screen so you can move the player.
	void UpdateVelocityField(Ogre::Vector3 playerPos);

};



/**
* @brief Moves fluid particles.
*/
class FluidSystem : public System {

public:

    /**
    * @brief Lua bindings
    *
    * Exposes:
    * - FluidSystem()
    *
    * @return
    */
    static luabind::scope
    luaBindings();

    /**
    * @brief Constructor
    */
    FluidSystem();

    /**
    * @brief Destructor
    */
    ~FluidSystem();

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
    FluidComponent* fluid;

    struct Implementation;
    std::unique_ptr<Implementation> m_impl;

    GameState* gameState;
    OgreSceneNodeComponent* playerSceneNode;
};

}
