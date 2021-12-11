#include "stdafx.h"
#include "PhysXManager.h"
#include <mutex>


#define PX_RELEASE(x) if(x) { x->release(); x = NULL; }


PhysXManager::PhysXManager() noexcept {
	for (int i = 0; i < NBSCENES; ++i) {
		PxScene* s = NULL;
		gScenes.push_back(s);
	}
	initPhysics();
}

void PhysXManager::initPhysics()
{
	MyContactModification* contact = new MyContactModification;
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = FilterShader;

	for (auto& s : gScenes) {
		s = gPhysics->createScene(sceneDesc);
		s->setContactModifyCallback(contact);
		s->setSimulationEventCallback(contact);

		PxPvdSceneClient* pvdClient = s->getScenePvdClient(); 
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}
	/*gScene = gPhysics->createScene(sceneDesc);
	gScene->setContactModifyCallback(contact);
	gScene->setSimulationEventCallback(contact);*/

	/*PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}*/
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
	if (!mCooking)
		throw "quelquechose s'est mal passé lors de la création du meshCooking";
}

void PhysXManager::stepPhysics(int scene)
{
	gScenes[scene]->simulate(1.0f / 60.0f);
	gScenes[scene]->fetchResults(true);

	/*gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);*/
}

void PhysXManager::cleanupPhysics()
{
	for (auto& s : gScenes) {
		PX_RELEASE(s);
	}
	//PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
	
}

void PhysXManager::addToScene(PxActor* actor, int scene)
{
	gScenes[scene]->addActor(*actor);
}

static std::mutex physXMutex;
PxRigidDynamic* PhysXManager::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity, int scene)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 1.0f);
	dynamic->setLinearVelocity(velocity);
	std::lock_guard<std::mutex> lock(physXMutex);
	addToScene(dynamic, scene);
	return dynamic;
}

PhysXManager& PhysXManager::get() noexcept {
	static PhysXManager instance;
	return instance;
}

void PhysXManager::setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback

	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*) * numShapes);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	free(shapes);
}


void PhysXManager::removeActor(PxActor& actor, int scene) {
	gScenes[scene]->removeActor(actor);
}


PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//TO DO: code pour gerer les cas particuliers lors des collisions et le triggers
	if ((filterData0.word0 == FilterGroup::ePlayer && filterData1.word0 == FilterGroup::ePortal1) || 
		(filterData0.word0 == FilterGroup::ePortal1 && filterData1.word0 == FilterGroup::ePlayer))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	if ((filterData0.word0 == FilterGroup::ePlayer && filterData1.word0 == FilterGroup::ePortal2) ||
		(filterData0.word0 == FilterGroup::ePortal2 && filterData1.word0 == FilterGroup::ePlayer))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	if ((filterData0.word0 == FilterGroup::ePlayer && filterData1.word0 == FilterGroup::ePortal3) ||
		(filterData0.word0 == FilterGroup::ePortal3 && filterData1.word0 == FilterGroup::ePlayer))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	if ((filterData0.word0 == FilterGroup::ePlayer && filterData1.word0 == FilterGroup::ePortal4) ||
		(filterData0.word0 == FilterGroup::ePortal4 && filterData1.word0 == FilterGroup::ePlayer))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	if ((filterData0.word0 == FilterGroup::ePlayer && filterData1.word0 == FilterGroup::ePortalEnd) ||
		(filterData0.word0 == FilterGroup::ePortalEnd && filterData1.word0 == FilterGroup::ePlayer))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}


	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	return PxFilterFlag::eDEFAULT;
}

