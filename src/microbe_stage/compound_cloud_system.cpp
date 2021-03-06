#include "microbe_stage/compound_cloud_system.h"
//#include "microbe_stage/membrane_system.h"
#include "microbe_stage/simulation_parameters.h"

// #include "bullet/collision_system.h"
// #include "bullet/rigid_body_system.h"
// #include "engine/component_factory.h"
// #include "engine/engine.h"
// #include "engine/entity_filter.h"
// #include "engine/entity.h"
// #include "engine/game_state.h"
// #include "engine/player_data.h"
// #include "engine/serialization.h"
// #include "game.h"
// #include "ogre/scene_node_system.h"
// #include "scripting/luajit.h"
// #include "util/make_unique.h"

#include "ThriveGame.h"

#include "engine/player_data.h"

#include "generated/cell_stage_world.h"

#include <OgreMeshManager2.h>
#include <OgreMeshManager.h>
#include <OgreMaterialManager.h>
// #include <OgreMaterial.h>
#include <OgreTextureManager.h>
#include <OgreTechnique.h>
#include <OgreRoot.h>
// #include <OgreSubMesh.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreSceneManager.h>
#include <OgrePlane.h>
#include <OgreMesh2.h>

#include <iostream>
#include <errno.h>
#include <stdio.h>

#include <string.h>

#include <chrono>
#include <atomic>

using namespace thrive;

constexpr auto YOffset = -1;

////////////////////////////////////////////////////////////////////////////////
// CompoundCloudComponent
////////////////////////////////////////////////////////////////////////////////
CompoundCloudComponent::CompoundCloudComponent(
    CompoundId id,
    float red,
    float green,
    float blue
) : Leviathan::Component(TYPE),
    color(red, green, blue), m_compoundId(id)
{
    m_textureName = "cloud_" + SimulationParameters::compoundRegistry.
        getInternalName(m_compoundId);
}

void
CompoundCloudComponent::addCloud(float dens, int x, int y) {
    if ((x-offsetX)/gridSize+width/2 >= 0 && (x-offsetX)/gridSize+width/2 < width &&
        (y-offsetZ)/gridSize+height/2 >= 0 && (y-offsetZ)/gridSize+height/2 < height)
    {
        density[(x-offsetX)/gridSize+width/2][(y-offsetZ)/gridSize+height/2] += dens;
    }
}

int
CompoundCloudComponent::takeCompound(int x, int y, float rate) {

    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        int amountToGive = static_cast<int>(density[x][y])*rate;
        density[x][y] -= amountToGive;
        if (density[x][y] < 1) density[x][y] = 0;

        return amountToGive;
    }

    return -1;

}

int
CompoundCloudComponent::amountAvailable(int x, int y, float rate) {

    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        int amountToGive = static_cast<int>(density[x][y])*rate;

        return amountToGive;
    }

    return -1;

}

////////////////////////////////////////////////////////////////////////////////
// CompoundCloudSystem
////////////////////////////////////////////////////////////////////////////////
CompoundCloudSystem::CompoundCloudSystem() :
    playerEntity(0),
    noiseScale(5),
    width(120),
    height(120),
    offsetX(0),
    offsetZ(0),
    gridSize(2),
    xVelocity(width, std::vector<float>(height, 0)),
    yVelocity(width, std::vector<float>(height, 0))
{

}

CompoundCloudSystem::~CompoundCloudSystem() {

}

static std::atomic<int> CloudMeshNumberCounter = {0};

void CompoundCloudSystem::Init(CellStageWorld &world){

    // Use the curl of a Perlin noise field to create a turbulent velocity field.
    CreateVelocityField();

    // Create a background plane on which the fluid clouds will be drawn.
    //Ogre::Plane plane(Ogre::Vector3::UNIT_Z, -1.0);
    Ogre::Plane plane(1, 1, 1, 1);

    const auto meshName = "CompoundCloudSystem_Plane_" + std::to_string(
        ++CloudMeshNumberCounter);

    const auto mesh = Ogre::v1::MeshManager::getSingleton().createPlane(
        meshName + "_v1",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
        width*gridSize, height*gridSize,
        1, 1,
        // Normals
        true,
        1,
        1.0f, 1.0f, Ogre::Vector3::UNIT_Y,
        Ogre::v1::HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
        Ogre::v1::HardwareBuffer::HBU_STATIC_WRITE_ONLY,
        false, false);

    m_planeMesh = Ogre::MeshManager::getSingleton().createManual(
        meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

    // Fourth true is qtangent encoding which is not needed if we don't do normal mapping
    m_planeMesh->importV1(mesh.get(), true, true, true );

    Ogre::v1::MeshManager::getSingleton().remove(mesh);

    // Need to edit the render queue (for when the item is created)
    world.GetScene()->getRenderQueue()->setRenderQueueMode(2, Ogre::RenderQueue::FAST);

    m_sceneNode = world.GetScene()->getRootSceneNode()->createChildSceneNode();
    
    // Set initial position and the rotation that is preserved
    m_sceneNode->setPosition(0, YOffset, 0);
    // Stolen from the old background rotation
    m_sceneNode->setOrientation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_Z) *
        Ogre::Quaternion(Ogre::Degree(45), Ogre::Vector3::UNIT_Y));
}

void CompoundCloudSystem::Release(CellStageWorld &world){

    // Destroy the plane
    if(compoundCloudsPlane){
        world.GetScene()->destroyItem(compoundCloudsPlane);
        compoundCloudsPlane = nullptr;
    }

    if(m_sceneNode){
        world.GetScene()->destroySceneNode(m_sceneNode);
        m_sceneNode = nullptr;
    }

    Ogre::MeshManager::getSingleton().remove(m_planeMesh);
}


void CompoundCloudSystem::Run(CellStageWorld &world,
    std::unordered_map<ObjectID, CompoundCloudComponent*> &index)
{
    const int renderTime = Leviathan::TICKSPEED;

    // Game::instance().engine().playerData().playerName()
    
    auto playerEntity = ThriveGame::instance()->playerData().activeCreature();

	Float3 position;

    if(playerEntity == NULL_OBJECT){

        LOG_WARNING("CompoundCloudSystem: Run: playerData().activeCreature() is NULL_OBJECT. "
            "Using default position");
    } else {

        try{
            // Get the player's position.
            const Leviathan::Position& posEntity = world.GetComponent_Position(playerEntity);
            position = posEntity.Members._Position;
            
        } catch(const Leviathan::NotFound &){
            LOG_WARNING("CompoundCloudSystem: Run: playerEntity has no position");
        }
    }
    
    // If the player moves out of the current grid, move the grid.
    if (position.X > offsetX + width/3*gridSize/2  ||
        position.Z > offsetZ + height/3*gridSize/2 ||
        position.X < offsetX - width/3*gridSize/2  ||
        position.Z < offsetZ - height/3*gridSize/2)
    {
        if (position.X > offsetX + width/3*gridSize/2 )
            offsetX += width/3*gridSize;
        if (position.Z > offsetZ + height/3*gridSize/2)
            offsetZ += height/3*gridSize;
        if (position.X < offsetX - width/3*gridSize/2 )
            offsetX -= width/3*gridSize;
        if (position.Z < offsetZ - height/3*gridSize/2)
            offsetZ -= height/3*gridSize;

        m_sceneNode->setPosition(offsetX, -YOffset, offsetZ);
    }

    bool changedMaterial = false;

    // TODO: could do something fancy with CellStageWorld::HandleAdded
    // For all newly created entities, initialize their parameters.
    for (auto& value : index) {

        if(value.second->initialized)
            continue;

        LOG_INFO("Initializing cloud for: " + std::to_string(value.second->m_compoundId));

        value.second->initialized = true;
        
        CompoundCloudComponent* compoundCloud = value.second;

        // Set the size of each grid tile and its position.
        compoundCloud->width = width;
        compoundCloud->height = height;
        compoundCloud->offsetX = offsetX;
        compoundCloud->offsetZ = offsetZ;
        compoundCloud->gridSize = gridSize;

        // TODO: switch to a single dimensional vector as this vector
        // of vectors is not the most efficient
        compoundCloud->density.resize(width, std::vector<float>(height, 0));
        compoundCloud->oldDens.resize(width, std::vector<float>(height, 0));

        // Modifies the material to draw this compound cloud in addition to the others.
        Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingleton().getByName(
            "CompoundClouds", "General");

        // TODO: destroy these layers when no longer used. This might
        // cause crashes if playing for a long time and new ones are
        // created
        Ogre::Pass* pass = materialPtr->getTechnique(0)->createPass();

        // Set blendblock
        Ogre::HlmsBlendblock blendblock;
        //blendblock.mAlphaToCoverageEnabled = false;

        // This is the old setting
        //pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        // And according to Ogre source code (OgrePass.cpp Pass::_getBlendFlags) it matches this:
        // source = SBF_SOURCE_ALPHA;
        // dest = SBF_ONE_MINUS_SOURCE_ALPHA;
        
        blendblock.mSourceBlendFactor = Ogre::SBF_SOURCE_ALPHA;
        blendblock.mDestBlendFactor = Ogre::SBF_ONE_MINUS_SOURCE_ALPHA;
        
        pass->setBlendblock(blendblock);
        pass->setVertexProgram("CompoundCloud_VS");
        pass->setFragmentProgram("CompoundCloud_PS");

        // Set colour parameter //
        pass->getFragmentProgramParameters()->setNamedConstant("cloudColour",
            compoundCloud->color);

        Ogre::TexturePtr texturePtr = Ogre::TextureManager::getSingleton().createManual(
            compoundCloud->m_textureName,
            "General", Ogre::TEX_TYPE_2D, width, height,
            0, Ogre::PF_BYTE_A, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

        // TODO: switch to Ogre 2.1 way
        Ogre::v1::HardwarePixelBufferSharedPtr cloud = texturePtr->getBuffer();
        cloud->lock(Ogre::v1::HardwareBuffer::HBL_DISCARD);
        const Ogre::PixelBox& pixelBox = cloud->getCurrentLock();

        // Fill with zeroes
        std::memset(static_cast<uint8_t*>(pixelBox.data), 0, cloud->getSizeInBytes());

        // Unlock the pixel buffer
        cloud->unlock();
        pass->createTextureUnitState()->setTexture(texturePtr);

        texturePtr = Ogre::TextureManager::getSingleton().load("PerlinNoise.jpg", "General");
        auto* noiseState = pass->createTextureUnitState();
        noiseState->setTexture(texturePtr);
        Ogre::HlmsSamplerblock wrappedBlock;

        // Make sure it wraps to make the borders also look good
        wrappedBlock.setAddressingMode(Ogre::TextureAddressingMode::TAM_WRAP);
        noiseState->setSamplerblock(wrappedBlock);

        changedMaterial = true;

        // TODO: multipass materials don't work in Ogre 2.1 so only
        // one cloud can work with this old approach. This needs to be split to multiple 
        break;
    }

    if(changedMaterial){

        LOG_INFO("CompoundCloudSystem: (re-)creating compound cloud plane");

        Ogre::Vector4 offset = {0, 0, 0, 0};

        if(compoundCloudsPlane){

            offset = compoundCloudsPlane->getSubItem(0)->getCustomParameter(1);
            
            world.GetScene()->destroyItem(compoundCloudsPlane);
            compoundCloudsPlane = nullptr;
        }

        // The render item needs to be recreated to render all the new layers
        compoundCloudsPlane = world.GetScene()->createItem(m_planeMesh);

        // This needs to be add to an early render queue 
        // But after the background
        compoundCloudsPlane->setRenderQueueGroup(2);

        m_sceneNode->attachObject(compoundCloudsPlane);

        // This loads the material first time this is called. This needs
        // to be called AFTER the first compound cloud has been created
        compoundCloudsPlane->setMaterialName("CompoundClouds");

        // Reset the offset
        compoundCloudsPlane->getSubItem(0)->setCustomParameter(1, offset);
    }

    // For all types of compound clouds...
    for (auto& value : index)
    {
        ProcessCloud(*value.second, renderTime);

        // TODO: multipass materials don't work in Ogre 2.1 so only
        // one cloud can work with this old approach. This needs to be split to multiple 
        break;
    }    
}

void
CompoundCloudSystem::ProcessCloud(
    CompoundCloudComponent &compoundCloud, int renderTime
) {
    // If the offset of the compound cloud is different from the fluid systems offset,
    // then the player must have moved, so we need to adjust the texture.
    if (compoundCloud.offsetX != offsetX || compoundCloud.offsetZ != offsetZ)
    {
        // If we moved up.
        if (compoundCloud.offsetX == offsetX && compoundCloud.offsetZ < offsetZ)
        {
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height/3; y++)
                {
                    compoundCloud.density[x][y] = compoundCloud.density[x][y+height/3];
                    compoundCloud.density[x][y+height/3] =
                        compoundCloud.density[x][y+height*2/3];
                    compoundCloud.density[x][y+height*2/3] = 0.0;
                }
            }
            Ogre::Vector4 offset = compoundCloudsPlane->getSubItem(0)->getCustomParameter(1);
            compoundCloudsPlane->getSubItem(0)->setCustomParameter(1,
                Ogre::Vector4(offset.x, offset.y-1.0f/3, 0.0f, 0.0f));
        }
        // If we moved right.
        else if (compoundCloud.offsetX < offsetX && compoundCloud.offsetZ == offsetZ)
        {
            for (int x = 0; x < width/3; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    compoundCloud.density[x][y] = compoundCloud.density[x+height/3][y];
                    compoundCloud.density[x+height/3][y] =
                        compoundCloud.density[x+height*2/3][y];
                    compoundCloud.density[x+height*2/3][y] = 0.0;
                }
            }
            Ogre::Vector4 offset = compoundCloudsPlane->getSubItem(0)->getCustomParameter(1);
            compoundCloudsPlane->getSubItem(0)->setCustomParameter(1,
                Ogre::Vector4(offset.x-1.0f/3, offset.y, 0.0f, 0.0f));
        }
        // If we moved left.
        else if (compoundCloud.offsetX > offsetX && compoundCloud.offsetZ == offsetZ)
        {
            for (int x = 0; x < width/3; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    compoundCloud.density[x+height*2/3][y] =
                        compoundCloud.density[x+height/3][y];
                    compoundCloud.density[x+height/3][y] = compoundCloud.density[x][y];
                    compoundCloud.density[x][y] = 0.0;
                }
            }
            Ogre::Vector4 offset = compoundCloudsPlane->getSubItem(0)->getCustomParameter(1);
            compoundCloudsPlane->getSubItem(0)->setCustomParameter(1,
                Ogre::Vector4(offset.x+1.0f/3, offset.y, 0.0f, 0.0f));
        }
        // If we moved downwards.
        else if (compoundCloud.offsetX == offsetX && compoundCloud.offsetZ > offsetZ)
        {
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height/3; y++)
                {
                    compoundCloud.density[x][y+height*2/3] =
                        compoundCloud.density[x][y+height/3];
                    compoundCloud.density[x][y+height/3] = compoundCloud.density[x][y];
                    compoundCloud.density[x][y] = 0.0;
                }
            }
            Ogre::Vector4 offset = compoundCloudsPlane->getSubItem(0)->getCustomParameter(1);
            compoundCloudsPlane->getSubItem(0)->setCustomParameter(1,
                Ogre::Vector4(offset.x, offset.y+1.0f/3, 0.0f, 0.0f));
        }

        compoundCloud.offsetX = offsetX;
        compoundCloud.offsetZ = offsetZ;
    }

    // Compound clouds move from area of high concentration to area of low.
    diffuse(.01, compoundCloud.oldDens, compoundCloud.density, renderTime);
    // Move the compound clouds about the velocity field.
    advect(compoundCloud.oldDens, compoundCloud.density, renderTime);

    // Store the pixel data in a hardware buffer for quick access.
    Ogre::v1::HardwarePixelBufferSharedPtr cloud;
    cloud = Ogre::TextureManager::getSingleton().getByName(
        compoundCloud.m_textureName, "General")->getBuffer();

    cloud->lock(Ogre::v1::HardwareBuffer::HBL_DISCARD);
    const Ogre::PixelBox& pixelBox = cloud->getCurrentLock();
    uint8_t* pDest = static_cast<uint8_t*>(pixelBox.data);

    size_t rowPitch = pixelBox.rowPitch;

    // Copy the density vector into the buffer.
    for (int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            int intensity = static_cast<int>(compoundCloud.density[i][height-j-1]);

            if (intensity < 0)
            {
                intensity = 0;
            }
            else if (intensity > 255)
            {
                intensity = 255;
            }
            // This can be used to debug the clouds
            // intensity = 190;
            // Alpha value of texture.
            pDest[rowPitch * j + i] = intensity;
        }
    }

    // Unlock the pixel buffer.
    cloud->unlock();
}

void
CompoundCloudSystem::CreateVelocityField() {
    float nxScale = noiseScale;
	float nyScale = nxScale * float(width) / float(height);
	float x0, y0, x1, y1, n0, n1, nx, ny;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			x0 = (float(x - 1) / float(width))  * nxScale;
			y0 = (float(y - 1) / float(height)) * nyScale;
			x1 = (float(x + 1) / float(width))  * nxScale;
			y1 = (float(y + 1) / float(height)) * nyScale;

			n0 = fieldPotential.noise(x0, y0, 0);
			n1 = fieldPotential.noise(x1, y0, 0);
			ny = n0 - n1;
			n0 = fieldPotential.noise(x0, y0, 0);
			n1 = fieldPotential.noise(x0, y1, 0);
			nx = n1 - n0;

			xVelocity[x][y] = nx/2;
			yVelocity[x][y] = ny/2;
		}
	}
}

void
CompoundCloudSystem::diffuse(float diffRate, std::vector<  std::vector<float>  >& oldDens,
    const std::vector<  std::vector<float>  >& density, int dt)
{
    dt = 1;
    float a = dt*diffRate;

    for (int x = 1; x < width-1; x++)
    {
        for (int y = 1; y < height-1; y++)
        {
            oldDens[x][y] = (density[x][y] + a*(oldDens[x - 1][y] + oldDens[x + 1][y] +
                oldDens[x][y-1] + oldDens[x][y+1])) / (1 + 4 * a);
        }
    }
}

void
CompoundCloudSystem::advect(std::vector<  std::vector<float>  >& oldDens,
    std::vector<  std::vector<float>  >& density, int dt)
{
    dt = 1;

    for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			density[x][y] = 0;
		}
	}

    float dx, dy;
    int x0, x1, y0, y1;
    float s1, s0, t1, t0;
	for (int x = 1; x < width-1; x++)
	{
		for (int y = 1; y < height-1; y++)
		{
		    if (oldDens[x][y] > 1) {
                dx = x + dt*xVelocity[x][y];
                dy = y + dt*yVelocity[x][y];

                if (dx < 0.5) dx = 0.5;
                if (dx > width - 1.5) dx = width - 1.5f;

                if (dy < 0.5) dy = 0.5;
                if (dy > height - 1.5) dy = height - 1.5f;

                x0 = static_cast<int>(dx);
                x1 = x0 + 1;
                y0 = static_cast<int>(dy);
                y1 = y0 + 1;

                s1 = dx - x0;
                s0 = 1 - s1;
                t1 = dy - y0;
                t0 = 1 - t1;

                density[x0][y0] += oldDens[x][y] * s0 * t0;
                density[x0][y1] += oldDens[x][y] * s0 * t1;
                density[x1][y0] += oldDens[x][y] * s1 * t0;
                density[x1][y1] += oldDens[x][y] * s1 * t1;
		    }
		}
	}
}
