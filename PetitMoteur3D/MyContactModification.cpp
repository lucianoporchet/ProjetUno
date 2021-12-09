#include "stdafx.h"
#include "MyContactModification.h"
#include "BlocEffet1.h"
#include "MoteurWindows.h"
#include <PxRigidDynamic.h>
#include <iostream>


class GameManager;
class SceneManager;

bool MyContactModification::hasBeenEnoughTimeSinceLastTrigger()
{
    if (horlogeM.GetTimeBetweenCounts(lastTriggered, horlogeM.GetTimeCount()) >= 1)
    {
        lastTriggered = horlogeM.GetTimeCount();
        return true;
    }
    return false;
}

void MyContactModification::onContactModify(PxContactModifyPair* const pairs, PxU32 count)
{
 
}

void MyContactModification::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void MyContactModification::onWake(PxActor** actors, PxU32 count)
{
}

void MyContactModification::onSleep(PxActor** actors, PxU32 count)
{
}

void MyContactModification::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    if (hasBeenEnoughTimeSinceLastTrigger()) {
        for (PxU32 i = 0; i < nbPairs; ++i)
        {

            PxShape* shape = pairs[i].shapes[0];
            PxShape* shape2 = pairs[i].shapes[1];

            if (shape->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape2->getSimulationFilterData().word0 == FilterGroup::ePortal1to2)
                GameManager::get().setNextZone(Zone::ZONE2);
            else if (shape2->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape->getSimulationFilterData().word0 == FilterGroup::ePortal1to2)
                GameManager::get().setNextZone(Zone::ZONE2);
            else if (shape->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape2->getSimulationFilterData().word0 == FilterGroup::ePortal2to1)
                GameManager::get().setNextZone(Zone::ZONE1);
            else if (shape2->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape->getSimulationFilterData().word0 == FilterGroup::ePortal2to1)
                GameManager::get().setNextZone(Zone::ZONE1);

        }
    }
}

void MyContactModification::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
}

void MyContactModification::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}
