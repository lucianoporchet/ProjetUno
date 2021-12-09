#pragma once
#include "PxPhysicsAPI.h"
#include "MyContactModification.h"

using namespace physx;
//filter group pour gerer les collission dans physX
struct FilterGroup
{
	enum Enum
	{
		ePlayer = (1 << 0),
		eObstacle = (1 << 1),
		eMonster = (1 << 2),
		ePortal1to2 = (1 << 3),
		ePortal2to1 = (1 << 4),
		ePortal2to3 = (1 << 5),
		ePortal3to2 = (1 << 6),
		ePortal3to4 = (1 << 7),
		ePortal4to3 = (1 << 8),
		ePortal4to1 = (1 << 9),
		ePortalEnd = (1 << 10)
	};
};
//singleton de la classe du physX manager
class PhysXManager
{
private:
	PhysXManager() noexcept;
public:
	PhysXManager(const PhysXManager&) = delete;
	PhysXManager& operator=(const PhysXManager&) = delete;

	//ajouter une gestion de collision entre deux objets a l'aide de leur FilterGroup
	void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);


	
	void initPhysics();					//lancer la physique
	void stepPhysics(int scene);		//update la physique
	void cleanupPhysics();				//clean la physique
	//ajouter ou enlever des acteurs de la scene de physX
	void addToScene(PxActor* actor, int scene);
	void removeActor(PxActor& actor, int scene);

	//creer un rigid body
	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity, int scene);
	

public:
	static PhysXManager& get() noexcept;

private:
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;
	PxDefaultCpuDispatcher* gDispatcher = NULL;
	std::vector<PxScene*> gScenes;
	PxMaterial* gMaterial = NULL;
	PxPvd* gPvd = NULL;
	
	enum {
		NBSCENES = 4
	};

};

//Gestionnaire de collisions
PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);