#pragma once

#include "scripting/luabind.h"
#include "engine/component.h"
#include <OgreVector3.h>
#include <string>

// Amount the microbes maximum bandwidth increases with per organelle added.
// This is a temporary replacement for microbe surface area.
#define BANDWIDTH_PER_ORGANELLE 1.0

// The amount of time between each loop to maintaining a fill
// level below STORAGE_EJECTION_THRESHHOLD and eject useless compounds.
#define EXCESS_COMPOUND_COLLECTION_INTERVAL 1000

// The of time it takes for the microbe to regenerate an amount of bandwidth equal to maxBandwidth
#define BANDWIDTH_REFILL_DURATION 800

namespace thrive {

//--------------------------------------------------------------------------------
//-- MicrobeComponent
//--
//-- Holds data common to all microbes. You probably shouldn't use this directly,
//-- use the Microbe class (in scripts/microbe_stage/microbe.lua) instead.
//--------------------------------------------------------------------------------

class MicrobeComponent : public Component {
	COMPONENT(MicrobeComponent)

public:
	static luabind::scope
	luaBindings();

	MicrobeComponent(
        bool _isPlayerMicrobe,
        const std::string& _name
    );

    /*
        Attempts to obtain an amount of bandwidth for immediate use
        This should be in conjunction with most operations ejecting  or absorbing compounds and agents for microbe

        param maxAmount
         The max amount of units that is requested

        param compoundId
         The compound being requested for volume considerations

        return
         amount in units avaliable for use
    */
    Ogre::Real
    getBandwidth(
        Ogre::Real maxAmount,
        int compoundId
    );

    void
    regenerateBandwidth(int logicTime);

    void
    load(
        const StorageContainer& storage
    ) override;

    StorageContainer
    storage() const override;


    std::string speciesName;
    int hitpoints;
    int maxHitpoints;
    bool dead;
    int deathTimer;
    luabind::object organelles;

    // Organelles with complete responsibility for a specific compound (such as agent vacuoles).
    luabind::object specialStorageOrganelles;

    Ogre::Vector3 movementDirection;
    Ogre::Vector3 facingTargetPoint;

    // Multiplied on the movement speed of the microbe.
    Ogre::Real movementFactor;

    // The amount that can be stored in the microbe.
    // NOTE: This does not include special storage organelles.
    int capacity;

    // The amount stored in the microbe.
    // NOTE: This does not include special storage organelles.
    int stored;

    luabind::object compounds;
    bool initialized;
    bool isPlayerMicrobe;
    Ogre::Real maxBandwidth;
    Ogre::Real remainingBandwidth;
    int compoundCollectionTimer;

    // Engulfing-related attributes.
    bool isCurrentlyEngulfing;
    bool isBeingEngulfed;
    bool wasBeingEngulfed;
    luabind::object hostileEngulfer;
};

}
