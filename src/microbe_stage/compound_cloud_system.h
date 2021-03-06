#pragma once

#include "general/perlin_noise.h"
// #include "ogre/scene_node_system.h"
#include "microbe_stage/compounds.h"

// #include "engine/component.h"
// #include "engine/system.h"
// #include "engine/touchable.h"
#include "engine/typedefs.h"
#include "engine/component_types.h"

#include <Entities/Component.h>
#include <Entities/System.h>


//#include <OgreEntity.h>
//#include <OgreSceneManager.h>
//#include "OgreHardwarePixelBuffer.h"

#include <OgreMesh.h>

#include <vector>


namespace thrive {

class CompoundCloudSystem;
class CellStageWorld;

/**
* @brief Compound clouds that flow in the environment
* @todo Make the cloud support 4 compound types to improve efficiency
*/
class CompoundCloudComponent : public Leviathan::Component {
public:

    // True once initialized by CompoundCloudSystem
    bool initialized = false;
    
    /// The size of the compound cloud grid.
	int width, height;
	int offsetX, offsetZ;
	float gridSize;

    /// The 2D array that contains the current compound clouds and those from last frame.
    std::vector<  std::vector<float>  > density;
    std::vector<  std::vector<float>  > oldDens;

    /// The 3x3 grid of density tiles around the player for seamless movement.
    //std::vector<  std::vector<float>  > density_11;
    //std::vector<  std::vector<float>  > density_12;
    //std::vector<  std::vector<float>  > density_13;
    //std::vector<  std::vector<float>  > density_21;
    //std::vector<  std::vector<float>  > density_23;
    //std::vector<  std::vector<float>  > density_31;
    //std::vector<  std::vector<float>  > density_32;
    //std::vector<  std::vector<float>  > density_33;

    /// The color of the compound cloud.
    Ogre::Vector3 color;

    /**
    * @brief The compound id.
    */
    CompoundId m_compoundId = NULL_COMPOUND;

    //! The name of the texture that is made for this cloud
    std::string m_textureName;

    static constexpr auto TYPE = componentTypeConvert(THRIVE_COMPONENT::COMPOUND_CLOUD);

public:
    //! \brief Creates a cloud with the specified compound(s) types and colour
    CompoundCloudComponent(CompoundId Id, float red, float green, float blue);

    //! \brief Places specified amount of compound at position (in this cloud's coordinates)
    void
    addCloud(
        float density,
        int x,
        int y
    );

    //! \param rate should be less than one.
    int
    takeCompound(
        int x,
        int y,
        float rate
    );

    //! \param rate should be less than one.
    int
    amountAvailable(
        int x,
        int y,
        float rate
    );

    REFERENCE_HANDLE_UNCOUNTED_TYPE(CompoundCloudComponent);
};



/**
* @brief Moves the compound clouds.
*/
class CompoundCloudSystem {
public:
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
    * @param world
    */
    void Init(CellStageWorld &world);

    /**
    * @brief Shuts the system down
    */
    void Release(CellStageWorld &world);

    /**
    * @brief Updates the system
    * @todo Is it too rough if the compound clouds only update every 50 milliseconds.
    * Does there need to be a third category of systems that can adapt to different length
    * ticks and run like tick but at render time?
    */
    void Run(CellStageWorld &world,
        std::unordered_map<ObjectID, CompoundCloudComponent*> &index);

private:

    void ProcessCloud(CompoundCloudComponent &cloud, int renderTime);

private:
    //! \todo Remove this. This is in the base class already
    //GameStateData* gameState;
    Ogre::Item* compoundCloudsPlane = nullptr;
    Ogre::SceneNode* m_sceneNode = nullptr;
    Ogre::MeshPtr m_planeMesh;
    
    //! \todo Check should this be a pointer to the component or is ObjectID fast enough
    ObjectID playerEntity = 0;

    PerlinNoise fieldPotential;
	float noiseScale;

	/// The size of the compound cloud grid.
	int width, height;
	int offsetX, offsetZ;
	float gridSize;

    /// The velocity of the fluid.
	std::vector<  std::vector<float>  > xVelocity;
	std::vector<  std::vector<float>  > yVelocity;

	void CreateVelocityField();
	void diffuse(float diffRate, std::vector<  std::vector<float>  >& oldDens, const std::vector<  std::vector<float>  >& density, int dt);
	void advect(std::vector<  std::vector<float>  >& oldDens, std::vector<  std::vector<float>  >& density, int dt);

    // Clears the density field file to blank (black).
    void initializeFile(std::string compoundName);
	// Draws the density field of the compound to a .bmp, which can be used for debugging
	void writeToFile(std::vector<  std::vector<float>  >& density, std::string compoundName, Ogre::ColourValue color);
};

}
