#pragma once
#include "PxPhysicsAPI.h"
#include "MyContactModification.h"

using namespace physx;
class PhysXManager
{
private:
	PhysXManager();

public:

	struct FilterGroup
	{
		enum Enum
		{
			ePlayer = (1 << 0),
			eObstacle = (1 << 1),
			eMonster = (1 << 2),
			ePortal = (1 << 3),
			eDebris = (1 << 4),
		};
	};

	void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
	
	PhysXManager(const PhysXManager&) = delete;
	PhysXManager& operator=(const PhysXManager&) = delete;
	//PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));
	void initPhysics();
	void stepPhysics();
	void cleanupPhysics();

	void addToScene(PxActor* actor);

	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity, PxU32 group);

	void removeActor(PxActor& actor);

public:
	static PhysXManager& get();

private:
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;
	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	PxPvd* gPvd = NULL;
	

};

PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);