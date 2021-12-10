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

            if (shape->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape2->getSimulationFilterData().word0 == FilterGroup::ePortal) {
                PxRigidDynamic* a = static_cast<PxRigidDynamic*>(shape2->getActor());
                GameManager::get().setNextZone(getNextZoneFromPos(a->getGlobalPose().p));
            }
            else if (shape2->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape->getSimulationFilterData().word0 == FilterGroup::ePortal) {
                PxRigidDynamic* a = static_cast<PxRigidDynamic*>(shape->getActor());
                GameManager::get().setNextZone(getNextZoneFromPos(a->getGlobalPose().p));
            }
            

        }
    }
}

void MyContactModification::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
}

void MyContactModification::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}

Zone MyContactModification::getNextZoneFromPos(PxVec3 pos) {
    const float size = SceneManager::get().getBoxSize();
    if (pos.y > size) {
        if (pos.x < 0)
            return Zone::ZONE1;
        else if (pos.x < size / 2)
            return Zone::ZONE3;
        else if (pos.x < size)
            return Zone::ZONE2;
        else
            return Zone::PORTAIL;
    }
    else {
        if (pos.x < size / 2)
            return Zone::ZONE2;
        else if (pos.x < size)
            return Zone::ZONE3;
        else
            return Zone::ZONE1;
    }
  
}
