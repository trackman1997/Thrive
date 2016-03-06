#include <map>
#include <typeinfo>
#include <OgreVector3.h>
#include <vector>
#include "microbe_stage/compound.h"
#include "microbe_stage/compound_registry.h"

#pragma once

namespace thrive {

class MicrobeComponent{
    public:
        MicrobeComponent(bool isPlayerMicrobe, std::string speciesName);
        virtual
        ~MicrobeComponent();
        void
        resetCompoundPriorities();

    protected:

    private:
        std::string m_speciesName;
        int m_hitpoints;
        int m_maxHitpoints;
        bool m_dead;
        int m_deathTimer = 0;
        //std::vector<Organelle> m_organelles;
        /**
        * Organelles responsible for producing compounds from other compounds
        **/
        //std::vector<ProcessOrganelle> m_processOrganelles;
        /**
        * Organelles with complete resonsiblity for a specific compound (such as agentvacuoles)
        **/
        //std::vector<StorageOrganelle> m_specialStorageOrganelles;
        Ogre::Vector3* m_movementDirection;
        Ogre::Vector3* m_facingTargetPoint;
        /**
        * Multiplied on the movement speed of the microbe.
        **/
        double m_movementFactor;
        /**
        * The amount that can be stored in the microbe. NOTE: This does not include special storage organelles
        **/
        int m_capacity;
        /**
        * The amount stored in the microbe. NOTE: This does not include special storage organelles
        **/
        int m_stored;
        std::vector<CompoundComponent>* m_compounds;
        std::map <CompoundId, int>* m_compoundPriorities;
        std::map <CompoundId, int>* m_defaultCompoundPriorities;
        bool m_initialized;
        bool m_isPlayerMicrobe;
        double m_maxBandwidth;
        int m_remainingBandwidth;
        int m_compoundCollectionTimer;
        bool m_isCurrentlyEngulfing;
        bool m_isBeingEngulfed;
        bool m_wasBeingEngulfed;
        //Microbe m_hostileEngulfer;
};

}

