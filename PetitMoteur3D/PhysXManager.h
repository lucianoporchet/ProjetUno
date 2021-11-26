#pragma once
#include "PxPhysicsAPI.h"
#include "MyContactModification.h"

using namespace physx;

//singleton de la classe du physX manager
class PhysXManager
{
private:
	PhysXManager() noexcept;
public:
	PhysXManager(const PhysXManager&) = delete;
	PhysXManager& operator=(const PhysXManager&) = delete;

	//filter group pour gerer les collission dans physX
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

	//ajouter une gestion de collision entre deux objets a l'aide de leur FilterGroup
	void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);


	
	void initPhysics();		//lancer la physique
	void stepPhysics();		//update la physique
	void cleanupPhysics();	//clean la physique

	//ajouter ou enlever des acteurs de la scene de physX
	void addToScene(PxActor* actor);
	void removeActor(PxActor& actor);

	//creer un rigid body
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity, PxU32 group);
	

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

//Gestionnaire de collisions
PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);