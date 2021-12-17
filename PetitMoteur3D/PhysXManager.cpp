#include "stdafx.h"
#include "PhysXManager.h"
#include "SceneManager.h"



#define PX_RELEASE(x) if(x) { x->release(); x = NULL; }
static std::mutex physXMutex, mutexPlanet, mutexAsteroid;

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
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(PxTolerancesScale()));
	if (!mCooking)
		throw "quelquechose s'est mal passé lors de la création du meshCooking";
	registry = PxSerialization::createSerializationRegistry(*gPhysics);

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

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 1.0f);

	const float boxsize = SceneManager::get().getBoxSize();
	constexpr float w = 10.0f;

	for (int i = 0; i < 4; ++i) {
		createWall(PxTransform(PxVec3(-boxsize / 2, boxsize / 2, 0.0f)), PxBoxGeometry(w, boxsize * 2, boxsize), i);
		createWall(PxTransform(PxVec3(boxsize / 2, boxsize / 2, 0.0f)), PxBoxGeometry(w, boxsize * 2, boxsize), i);
		createWall(PxTransform(PxVec3(3 * boxsize / 2, boxsize / 2, 0.0f)), PxBoxGeometry(w, boxsize * 2, boxsize), i);
		createWall(PxTransform(PxVec3(boxsize / 2, boxsize / 2, -boxsize / 2)), PxBoxGeometry(2 * boxsize, 2 * boxsize, w), i);
		createWall(PxTransform(PxVec3(boxsize / 2, boxsize / 2, boxsize / 2)), PxBoxGeometry(2 * boxsize, 2 * boxsize, w), i);
		createWall(PxTransform(PxVec3(0.0f, 3 * boxsize / 2, 0.0f)), PxBoxGeometry(boxsize - w, w, boxsize - w), i);
		createWall(PxTransform(PxVec3(boxsize, 3 * boxsize / 2, 0.0f)), PxBoxGeometry(boxsize - w, w, boxsize - w), i);
		createWall(PxTransform(PxVec3(0.0f, boxsize / 2, 0.0f)), PxBoxGeometry(boxsize - w, w, boxsize - w), i);
		createWall(PxTransform(PxVec3(boxsize, boxsize / 2, 0.0f)), PxBoxGeometry(boxsize - w, w, boxsize - w), i);
		createWall(PxTransform(PxVec3(0.0f, -boxsize / 2, 0.0f)), PxBoxGeometry(boxsize - w, w, boxsize - w), i);
		createWall(PxTransform(PxVec3(boxsize, -boxsize / 2, 0.0f)), PxBoxGeometry(boxsize - w, w, boxsize - w), i);
	}

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
	gScenes.clear();
	//PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	PX_RELEASE(mCooking);
	PX_RELEASE(registry);

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

//creation du terrain s'il n'as pas été cook
void PhysXManager::createTerrain(const PxTransform& t, PxTriangleMeshGeometry& geom, int scene) {
	
	PxShape* shape = PhysXManager::get().getgPhysx()->createShape(geom, *gMaterial, true);
	PxRigidStatic* body = PxCreateStatic(*gPhysics, t, geom, *gMaterial);

	collection = PxCreateCollection();
	collection->add(*body);
	PxSerialization::complete(*collection, *registry);

	PxDefaultFileOutputStream outStream("serialized.dat");
	PxSerialization::serializeCollectionToBinary(outStream, *collection, *registry);
}

//deserialize le cooking pour le mettre dans la scene
PxRigidStatic* PhysXManager::createTerrainSerialized(PxCollection* collection,int scene) {
	this->collection = collection;
	//gScenes[scene]->addCollection(*this->collection);
	PxU32 size = collection->getNbObjects();

	PxRigidActor* actor;

	for (PxU32 o = 0; o < this->collection->getNbObjects(); o++)
	{
		PxRigidActor* rigidActor = this->collection->getObject(o).is<PxRigidActor>();
		if (rigidActor)
		{
			actor = rigidActor;
		}
	}
	addToScene(static_cast<PxRigidStatic*>(actor), scene);
	return static_cast<PxRigidStatic*>(actor);
}


PxRigidDynamic* PhysXManager::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity, int scene)
{
	std::lock_guard<std::mutex> lock(physXMutex);
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 1.0f);
	dynamic->setLinearVelocity(velocity);
	addToScene(dynamic, scene);
	return dynamic;
}

PxPhysics* PhysXManager::getgPhysx()
{
	return gPhysics;
}

PxCooking* PhysXManager::getPxCooking()
{
	return mCooking;
}

PxSerializationRegistry* PhysXManager::getRegistry()
{
	return registry;
}

PxRigidStatic* PhysXManager::createStatic(const PxTransform& t, const PxGeometry& geometry, int scene)
{
	PxRigidStatic* statique = PxCreateStatic(*gPhysics, t, geometry, *gMaterial);
	std::lock_guard<std::mutex> lock(physXMutex);
	addToScene(statique, scene);
	return statique;
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

void PhysXManager::addForcesPlanet(float scale, PxRigidDynamic* body)
{
	std::lock_guard<std::mutex> lock(mutexPlanet);
	body->setAngularDamping(0.f); //enleve l'angular damping pour que les planetes tournent sans s'arreter
	body->setLinearDamping(0.5f); //ajoute du linear damping pour qu'en cas de grosse collision la planete de bouge pas a l'infini
	body->setMass(scale * 10000); //ajoute une masse proportionnelle a la taille
	//choisi un sens de rotation pour la Planete, aleatoirement entre 4 sens de rotation definis
	int rotaAxis = RandomGenerator::get().next(1, 4);
	switch (rotaAxis)
	{
	case 1:
		body->addTorque(PxVec3(static_cast<float>(pow(scale * 2, 4)), 0, 0), PxForceMode::eIMPULSE);
		break;
	case 2:
		body->addTorque(PxVec3(0, static_cast<float>(pow(scale * 2, 4)), 0), PxForceMode::eIMPULSE);
		break;
	case 3:
		body->addTorque(PxVec3(0, 0, static_cast<float>(pow(scale * 2, 4))), PxForceMode::eIMPULSE);
		break;
	case 4:
		body->addTorque(PxVec3(static_cast<float>(pow(scale * 2, 4)), static_cast<float>(pow(scale * 2, 4)), 0), PxForceMode::eIMPULSE);
		break;
	default:
		break;
	}

}

void PhysXManager::addForcesAsteroid(float scale, PxRigidDynamic* body, PxVec3 dir) {

	std::lock_guard<std::mutex> lock(mutexAsteroid);
	body->addTorque(RandomGenerator::get().randomVec3(-20, 20) * static_cast<float>(pow(scale * 3, 4)), PxForceMode::eIMPULSE);
	body->addForce(dir, PxForceMode::eIMPULSE);
	body->setMass(scale * 10);
	body->setLinearDamping(0);
	body->setAngularDamping(0);

}


void PhysXManager::removeActor(PxActor& actor, int scene) {
	gScenes[scene]->removeActor(actor);
}

void PhysXManager::initVectorScenes()
{
	for (int i = 0; i < NBSCENES; ++i) {
		PxScene* s = NULL;
		gScenes.push_back(s);
	}
}


PxFilterFlags FilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{

	if ((filterData0.word0 == FilterGroup::ePortal || filterData1.word0 == FilterGroup::ePortal))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	else if ((filterData1.word0 == FilterGroup::ePortal) || (filterData0.word0 == FilterGroup::ePortal))
	{
		pairFlags = PxPairFlag::eDETECT_DISCRETE_CONTACT;
		return PxFilterFlag::eDEFAULT;
	}
	if ((filterData0.word0 == FilterGroup::ePlayer && filterData1.word0 == FilterGroup::ePickupObject) ||
		(filterData0.word0 == FilterGroup::ePickupObject && filterData1.word0 == FilterGroup::ePlayer))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	else if ((filterData1.word0 == FilterGroup::ePickupObject) || (filterData0.word0 == FilterGroup::ePickupObject))
	{
		pairFlags = PxPairFlag::eDETECT_DISCRETE_CONTACT;
		return PxFilterFlag::eDEFAULT;
	}

	if (((filterData0.word0 == FilterGroup::eFinalPortal) && (filterData1.word0 == FilterGroup::ePlayer)) ||
		((filterData1.word0 == FilterGroup::eFinalPortal) && (filterData0.word0 == FilterGroup::ePlayer)))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	else if ((filterData0.word0 == FilterGroup::eFinalPortal) || (filterData1.word0 == FilterGroup::eFinalPortal))
	{
		pairFlags = PxPairFlag::eDETECT_DISCRETE_CONTACT;
		return PxFilterFlag::eDEFAULT;
	}
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	return PxFilterFlag::eDEFAULT;
}

void PhysXManager::createWall(const PxTransform& t, const PxGeometry& geometry, int scene)
{
	PxRigidStatic* wall = PxCreateStatic(*gPhysics, t, geometry, *gMaterial);
	addToScene(wall, scene);
}

