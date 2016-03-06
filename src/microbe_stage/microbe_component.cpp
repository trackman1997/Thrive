#include <typeinfo>
#include <OgreVector3.h>
#include <vector>
#include <map>
#include "microbe_stage/compound.h"
#include "microbe_stage/compound_registry.h"
#include "microbe_stage/microbe_component.h"

using namespace thrive;

//quantity of physics time between each loop distributing compounds to organelles. TODO: Modify to reflect microbe size.
static const int COMPOUND_PROCESS_DISTRIBUTION_INTERVAL = 100;
//amount the microbes maxmimum bandwidth increases with per organelle added. This is a temporary replacement for microbe surface area
static const double BANDWIDTH_PER_ORGANELLE = 1.0;
//The amount of time it takes for the microbe to regenerate an amount of bandwidth equal to maxBandwidth
static const int BANDWIDTH_REFILL_DURATION = 800;
static const double STORAGE_EJECTION_THRESHHOLD = 0.8;
//The amount of time between each loop to maintaining a fill level below STORAGE_EJECTION_THRESHHOLD and eject useless compounds
static const int EXCESS_COMPOUND_COLLECTION_INTERVAL = 1000;
static const int MICROBE_HITPOINTS_PER_ORGANELLE = 10;
static const int MINIMUM_AGENT_EMISSION_AMOUNT = 1;
static const int REPRODUCTASE_TO_SPLIT = 5;
static const int RELATIVE_VELOCITY_TO_BUMP_SOUND = 6;
static const double INITIAL_EMISSION_RADIUS = 0.5;
static const int ENGULFING_MOVEMENT_DIVISION = 3;
static const int ENGULFED_MOVEMENT_DIVISION = 4;
static const double ENGULFING_ATP_COST_SECOND = 1.5;
static const double ENGULF_HP_RATIO_REQ = 1.5;

MicrobeComponent::MicrobeComponent(bool isPlayerMicrobe, std::string speciesName){
    //ctor
    m_speciesName = speciesName;
    m_hitpoints = 10;
    m_maxHitpoints = 10;
    m_dead = false;
    m_deathTimer = 0;
    //m_organelles = new new std::vector<Organelle>;
    //m_processOrganelles = new std::vector<ProcessOrganelle>;
    //m_specialStorageOrganelles = std::vector<StorageOrganelle>;
    m_movementDirection = new Ogre::Vector3({0, 0, 0}); //{0, 0, 0};
    m_facingTargetPoint = new Ogre::Vector3({0, 0, 0}); //{0, 0, 0};
    m_movementFactor = 1.0;
    m_capacity = 0;
    m_stored = 0;
    m_compounds = new std::vector<CompoundComponent>;
    m_compoundPriorities = new std::map <CompoundId, int>;
    m_defaultCompoundPriorities = new std::map<CompoundId, int>;
    //m_defaultCompoundPriorities->insert( std::pair<CompoundId, int>(CompoundRegistry::getCompoundId("atp"), 10) );
    //m_defaultCompoundPriorities->insert( std::pair<CompoundId, int>(CompoundRegistry::getCompoundId("reproductase"), 8) );
    this->resetCompoundPriorities();
    m_initialized = false;
    m_isPlayerMicrobe = isPlayerMicrobe;
    m_maxBandwidth = 10.0*BANDWIDTH_PER_ORGANELLE;
    m_remainingBandwidth = 0;
    m_compoundCollectionTimer = EXCESS_COMPOUND_COLLECTION_INTERVAL;
    m_isCurrentlyEngulfing = false;
    m_isBeingEngulfed = false;
    m_wasBeingEngulfed = false;
    //self.hostileEngulfer = nullptr;
}

void
MicrobeComponent::resetCompoundPriorities(){
    for(auto &compound : CompoundRegistry::getCompoundList()){
            std::cout << typeid(compound).name() << "Dfsfdsfdfdsfs" << '\n';

    }
}

MicrobeComponent::~MicrobeComponent(){
    //dtor
}
