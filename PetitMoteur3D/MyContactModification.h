#pragma once
#include "PhysXManager.h"

using namespace physx;

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

	bool once = true;

	
};

