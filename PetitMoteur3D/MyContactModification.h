#pragma once
#include "PhysXManager.h"
#include "Horloge.h"


using namespace physx;

enum class Zone;

//Gestionnaire de conflits
class MyContactModification : public PxContactModifyCallback, public PxSimulationEventCallback
{
	void onContactModify(PxContactModifyPair* const pairs, PxU32 count);

	// Inherited via PxSimulationEventCallback
	virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;

	virtual void onWake(PxActor** actors, PxU32 count) override;

	virtual void onSleep(PxActor** actors, PxU32 count) override;

	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override;

	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;

	virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override;
	
	bool hasBeenEnoughTimeSinceLastTrigger();

	Zone getNextZoneFromPos(PxVec3 pos);

	PM3D::Horloge horlogeM;
	int64_t lastTriggered = 0;
};

