#include "compound_clouds.h"

#include "bullet/collision_system.h"
#include "bullet/rigid_body_system.h"
#include "engine/component_factory.h"
#include "engine/engine.h"
#include "engine/entity_filter.h"
#include "engine/game_state.h"
#include "engine/serialization.h"
#include "game.h"
#include "ogre/scene_node_system.h"
#include "scripting/luabind.h"
#include "util/make_unique.h"

#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreRoot.h>
#include <OgreSubMesh.h>
#include <OgreParticleSystemManager.h>
#include <OgreParticleSystem.h>
#include <stdexcept>

using namespace thrive;


////////////////////////////////////////////////////////////////////////////////
// CompoundCloudComponent
////////////////////////////////////////////////////////////////////////////////

luabind::scope
CompoundCloudComponent::luaBindings() {
    using namespace luabind;
    return class_<CompoundCloudComponent, Component>("CompoundCloudComponent")
        .enum_("ID") [
            value("TYPE_ID", CompoundCloudComponent::TYPE_ID)
        ]
        .scope [
            def("TYPE_NAME", &CompoundCloudComponent::TYPE_NAME)
        ]
        .def(constructor<>())
        .def("initialize",&CompoundCloudComponent::initialize)
    ;
}




void
CompoundCloudComponent::load(
    const StorageContainer& storage
) {
    Component::load(storage);
    //m_emissionRadius = storage.get<Ogre::Real>("emissionRadius", 0.0);
}

StorageContainer
CompoundCloudComponent::storage() const {
    StorageContainer storage = Component::storage();
   // storage.set<Ogre::Real>("emissionRadius", m_emissionRadius);
    return storage;
}

void
CompoundCloudComponent::initialize(int particleAmount, int x, int y)
{
	amount = particleAmount;
	density = 2.5f;
	totalAge = 10000;

	particles.emplace_back(x,y,0);
	age.emplace_back(0);
	for (int i=1; i<amount; i++) {
		particles.emplace_back(particles[0].x + Ogre::Math::RangeRandom(0, 100), particles[0].y + Ogre::Math::RangeRandom(0, 100),0);
		age.push_back(Ogre::Math::RangeRandom(0, 50) * density);
	}
}
REGISTER_COMPONENT(CompoundCloudComponent)



////////////////////////////////////////////////////////////////////////////////
// CompoundCloudSystem
////////////////////////////////////////////////////////////////////////////////

luabind::scope
CompoundCloudSystem::luaBindings() {
    using namespace luabind;
    return class_<CompoundCloudSystem, System>("CompoundCloudSystem")
        .def(constructor<>())
    ;
}


struct CompoundCloudSystem::Implementation {


    EntityFilter<
        CompoundCloudComponent,
        OgreSceneNodeComponent
    > m_entities = {true};

    Ogre::SceneManager* m_sceneManager = nullptr;


};


CompoundCloudSystem::CompoundCloudSystem()
  : m_impl(new Implementation())
{
}


CompoundCloudSystem::~CompoundCloudSystem() {}


void
CompoundCloudSystem::init(
    GameState* gameState
) {
    System::init(gameState);
    m_impl->m_entities.setEntityManager(&gameState->entityManager());
    m_impl->m_sceneManager = gameState->sceneManager();
}


void
CompoundCloudSystem::shutdown() {
    m_impl->m_entities.setEntityManager(nullptr);
    m_impl->m_sceneManager = nullptr;
    System::shutdown();
}


void
CompoundCloudSystem::update(int, int) {
    for (auto& value : m_impl->m_entities.addedEntities()) {
        CompoundCloudComponent* cloud = std::get<0>(value.second);
        OgreSceneNodeComponent* component = std::get<1>(value.second);

        cloud->cloud = m_impl->m_sceneManager->createParticleSystem("CompoundCloud");
        component->m_sceneNode->attachObject(cloud->cloud);
        cloud->cloud->fastForward(5.0);
        cloud->cloud->setSpeedFactor(0.0);
        cloud->cloud = m_impl->m_sceneManager->createParticleSystem("CompoundCloud2");
        component->m_sceneNode->attachObject(cloud->cloud);
        cloud->cloud->fastForward(5.0);
        cloud->cloud->setSpeedFactor(0.0);
    }
    m_impl->m_entities.clearChanges();
}


