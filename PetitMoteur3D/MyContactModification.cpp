#include "stdafx.h"
#include "MyContactModification.h"
#include "BlocEffet1.h"
#include "MoteurWindows.h"
#include <PxRigidDynamic.h>
#include <iostream>


class GameManager;
class SceneManager;


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
    for (PxU32 i = 0; i < nbPairs; ++i)
    {
        if (once) {
            PxShape* shape = pairs[i].shapes[0];
            PxShape* shape2 = pairs[i].shapes[1];

            if (shape->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape2->getSimulationFilterData().word0 == FilterGroup::ePortal1) {
               /* PxRigidDynamic* a = (PxRigidDynamic*)shape->getActor();
                PxQuat qua = a->getGlobalPose().q;
                a->setGlobalPose(PxTransform(PxVec3(-405.0f, 1403, 5560.0f), qua));*/
                //PhysXManager::get().removeActor(*SceneManager::get().player->body, static_cast<int>(GameManager::get().getActiveZone()));
                GameManager::get().setNextZone(Zone::ZONE2);
                //PhysXManager::get().addToScene(SceneManager::get().player->body, static_cast<int>(Zone::ZONE2));
            }
            else if (shape2->getSimulationFilterData().word0 == FilterGroup::ePlayer && shape->getSimulationFilterData().word0 == FilterGroup::ePortal1) {
                /*PxRigidDynamic* a = (PxRigidDynamic*)shape2->getActor();
                PxQuat qua = a->getGlobalPose().q;
                a->setGlobalPose(PxTransform(PxVec3(-405.0f, 1403, 5560.0f), qua));*/
               // PhysXManager::get().removeActor(*SceneManager::get().player->body, static_cast<int>(GameManager::get().getActiveZone()));
                GameManager::get().setNextZone(Zone::ZONE2);
                //PhysXManager::get().addToScene(SceneManager::get().player->body, static_cast<int>(Zone::ZONE2));
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
