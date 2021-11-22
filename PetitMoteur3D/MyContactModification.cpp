#include "stdafx.h"
#include "MyContactModification.h"
#include "BlocEffet1.h"
#include "MoteurWindows.h"
#include <PxRigidDynamic.h>
#include <iostream>



struct FilterGroup
{
    enum Enum
    {
        ePlayer = (1 << 0),
        eObstacle = (1 << 1),
        eMonster = (1 << 2),
        ePortal = (1 << 3),
        eDebris = (1 << 4)
    };
};

void MyContactModification::onContactModify(PxContactModifyPair* const pairs, PxU32 count)
{
    for (PxU32 i = 0; i < count; i++)
    {
        for (PxU32 j = 0; j < pairs[i].contacts.size(); j++) {
            pairs[i].contacts.setNormal(j, PxVec3(0.0f, 0.8f, 1.0f));
        }
    }
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
    PhysXManager& pXManager = PhysXManager::get();
    for (PxU32 i = 0; i < nbPairs; i++)
    {
        
        PxShape* shape = pairs[i].shapes[0];
        PxShape* shape2 = pairs[i].shapes[1];

        if (shape->getSimulationFilterData().word0 == FilterGroup::eObstacle && shape2->getSimulationFilterData().word0 == FilterGroup::eObstacle) {
            PxRigidDynamic* a = (PxRigidDynamic*)shape->getActor();
            PxRigidDynamic* a2 = (PxRigidDynamic*)shape2->getActor();
            const PxVec3 pos = a->getGlobalPose().p;
            const PxVec3 pos2 = a2->getGlobalPose().p;
            //for (int i = 0; i < 10; i++) {
            //    std::unique_ptr<PM3D::CBlocEffet1> pBloc = std::make_unique<PM3D::CBlocEffet1>(0.5f, 0.5f, 0.5f, PM3D::CMoteurWindows::GetInstance().pDispositif, true);
            //   // shape->setSimulationFilterData(FilterGroup::eDebris);

            //    //// Lui assigner une texture
            //    //pBloc->SetTexture(PM3D::CMoteurWindows::GetInstance().TexturesManager.GetNewTexture(L"roche2.dds", PM3D::CMoteurWindows::GetInstance().pDispositif));


            //    PM3D::CMoteurWindows::GetInstance().ListeScene.push_back(std::move(pBloc));
            //}
           /* for (int i = 0; i < 10; ++i) {
                pXManager.createDynamic(PxTransform(pos), PxSphereGeometry(0.2f), PxVec3(0), FilterGroup::eDebris);
                pXManager.createDynamic(PxTransform(pos2), PxSphereGeometry(0.2f), PxVec3(0), FilterGroup::eDebris);
            }*/
           /* a->release();
            a2->release();*/
       /*     pXManager.removeActor(*a);
            pXManager.removeActor(*a2);*/
        }
   
        
    }
}


void MyContactModification::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
}

void MyContactModification::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
}
