#include "fluid_sim.h"

#include "bullet/collision_system.h"
#include "bullet/rigid_body_system.h"
#include "engine/component_factory.h"
#include "engine/engine.h"
#include "engine/entity_filter.h"
#include "engine/entity.h"
#include "engine/game_state.h"
#include "engine/player_data.h"
#include "engine/serialization.h"
#include "game.h"
#include "scripting/luabind.h"
#include "util/make_unique.h"

#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreRoot.h>
#include <OgreSubMesh.h>
#include <OgreParticleSystem.h>
#include <stdexcept>

using namespace thrive;

////////////////////////////////////////////////////////////////////////////////
// FluidComponent
////////////////////////////////////////////////////////////////////////////////

FluidComponent::FluidComponent(int width, int height, float noiseScale)
 : xGrid(width, height, 1),
   yGrid(width, height, 1)
{
	nxScale = noiseScale;
	nyScale = nxScale * float(xGrid.m_height) / float(xGrid.m_width);

    xGrid.move(-xGrid.m_width *xGrid.m_resolution/2,
               -xGrid.m_height*xGrid.m_resolution/2);
    yGrid.move(-xGrid.m_width *xGrid.m_resolution/2,
               -xGrid.m_height*xGrid.m_resolution/2);

	CreateVelocityField();
}

void FluidComponent::CreateVelocityField()
{
	for (int x=xGrid.m_x; x<xGrid.m_x + xGrid.m_width*xGrid.m_resolution; x++) {
		for (int y=xGrid.m_y; y<xGrid.m_y + xGrid.m_height*xGrid.m_resolution; y++) {
		    dx =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
            dy =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
            fx1 = ((float(x - 1) + dx) / float(xGrid.m_width*xGrid.m_resolution)) * nxScale;
            fy1 = ((float(y - 1) + dy) / float(xGrid.m_height*xGrid.m_resolution)) * nyScale;
            dx =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
            dy =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
            fx2 = ((float(x + 1) + dx) / float(xGrid.m_width*xGrid.m_resolution)) * nxScale;
            fy2 = ((float(y + 1) + dy) / float(xGrid.m_height*xGrid.m_resolution)) * nyScale;

            n1 = fieldPotential.noise(fx1, fy1, 0);
            n2 = fieldPotential.noise(fx2, fy1, 0);
            ny = n1 - n2;
            n1 = fieldPotential.noise(fx1, fy1, 0);
            n2 = fieldPotential.noise(fx1, fy2, 0);
            nx = n2 - n1;

            xGrid.set(x,y,nx*10000);
            yGrid.set(x,y,ny*10000);
		}
	}
}

void FluidComponent::UpdateVelocityField(Ogre::Vector3 playerPos)
{
    if(static_cast<int>(xGrid.m_x + xGrid.m_width *xGrid.m_resolution/2) != static_cast<int>(playerPos.x) ||
       static_cast<int>(xGrid.m_y + xGrid.m_height*xGrid.m_resolution/2) != static_cast<int>(playerPos.y)) {
        xGrid.move(playerPos.x - xGrid.m_x - xGrid.m_width *xGrid.m_resolution/2,
                   playerPos.y - xGrid.m_y - xGrid.m_height*xGrid.m_resolution/2);
        yGrid.move(playerPos.x - yGrid.m_x - yGrid.m_width *yGrid.m_resolution/2,
                   playerPos.y - yGrid.m_y - yGrid.m_height*yGrid.m_resolution/2);
    }

	for(int x=xGrid.m_x; x<xGrid.m_x + xGrid.m_width*xGrid.m_resolution; x++) {
		for(int y=xGrid.m_y; y<xGrid.m_y + xGrid.m_height*xGrid.m_resolution; y++) {
            if(xGrid(x,y) == 0 || yGrid(x,y) == 0) {
                dx =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
                dy =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
                fx1 = ((float(x - 1) + dx) / float(xGrid.m_width*xGrid.m_resolution)) * nxScale;
                fy1 = ((float(y - 1) + dy) / float(xGrid.m_height*xGrid.m_resolution)) * nyScale;
                dx =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
                dy =  (Ogre::Math::RangeRandom(0,100) * 0.005) - 0.25;
                fx2 = ((float(x + 1) + dx) / float(xGrid.m_width*xGrid.m_resolution)) * nxScale;
                fy2 = ((float(y + 1) + dy) / float(xGrid.m_height*xGrid.m_resolution)) * nyScale;

                n1 = fieldPotential.noise(fx1, fy1, 0);
                n2 = fieldPotential.noise(fx2, fy1, 0);
                ny = n1 - n2;
                n1 = fieldPotential.noise(fx1, fy1, 0);
                n2 = fieldPotential.noise(fx1, fy2, 0);
                nx = n2 - n1;

                xGrid.set(x,y,nx*10000);
                yGrid.set(x,y,ny*10000);
            }
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// FluidSystem
////////////////////////////////////////////////////////////////////////////////

luabind::scope
FluidSystem::luaBindings() {
    using namespace luabind;
    return class_<FluidSystem, System>("FluidSystem")
        .def(constructor<>())
    ;
}


struct FluidSystem::Implementation {

    EntityFilter<
        CompoundCloudComponent,
        OgreSceneNodeComponent,
        RigidBodyComponent
    > m_entities;

    Ogre::SceneManager* m_sceneManager = nullptr;

};


FluidSystem::FluidSystem()
  : fluid(new FluidComponent(128, 128, 2)),
    m_impl(new Implementation())
{
}


FluidSystem::~FluidSystem() {}


void
FluidSystem::init(
    GameState* gameState
) {
    System::init(gameState);
    m_impl->m_entities.setEntityManager(&gameState->entityManager());
    m_impl->m_sceneManager = gameState->sceneManager();
    this->gameState = gameState;
    //playerSceneNode = static_cast<OgreSceneNodeComponent*>(gameState->entityManager().getComponent(1, OgreSceneNodeComponent::TYPE_ID));
}


void
FluidSystem::shutdown() {
    m_impl->m_entities.setEntityManager(nullptr);
    m_impl->m_sceneManager = nullptr;
    System::shutdown();
}


void
FluidSystem::update(int, int) {
    m_impl->m_entities.clearChanges();

    OgreSceneNodeComponent* playerNode = static_cast<OgreSceneNodeComponent*>(gameState->entityManager().getComponent(Entity(gameState->engine().playerData().playerName(), gameState).id(), OgreSceneNodeComponent::TYPE_ID));
    if(playerNode != NULL) {
        fluid->UpdateVelocityField(playerNode->m_transform.position);
    }

    for (auto& value : m_impl->m_entities) {
        CompoundCloudComponent* cloud = std::get<0>(value.second);
        OgreSceneNodeComponent* component = std::get<1>(value.second);
        RigidBodyComponent* body = std::get<2>(value.second);
        Ogre::Vector3& position = component->m_transform.position;

        Ogre::Vector3 intVelocity(0,0,0);
        intVelocity = Ogre::Vector3(fluid->xGrid.get(position.x,position.y)/200.0f,
                                    fluid->yGrid.get(position.x,position.y)/200.0f, 0);
        body->applyCentralImpulse(intVelocity/cloud->density);
    }
}

