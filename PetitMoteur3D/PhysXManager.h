#pragma once
#include "PxPhysicsAPI.h"

using namespace physx;
class PhysXManager
{
private:
	PhysXManager();

public:
	PhysXManager(const PhysXManager&) = delete;
	PhysXManager& operator=(const PhysXManager&) = delete;
	//PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));
	void initPhysics();
	void stepPhysics();
	void cleanupPhysics();

	void addToScene(PxActor* actor);

	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));

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

