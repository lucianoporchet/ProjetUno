#pragma once
#include <vector>
#include "PxPhysicsAPI.h"
#include "ObjetMesh.h"
class SceneManager
{
public:

	const physx::PxVec3 planetePos[10] = {
		physx::PxVec3(-100.0f, -100.0f, -100.0f),
		physx::PxVec3(100.0f, 100.0f, 80.0f),
		physx::PxVec3(-500.0f, -80.0f, -50.0f),
		physx::PxVec3(-80.0f, 700.0f, 100.0f),
		physx::PxVec3(-30.0f, 900.0f, -300.0f),
		physx::PxVec3(400.0f, -100.0f, -100.0f),
		physx::PxVec3(500.0f, 600.0f, -900.0f),
		physx::PxVec3(750.0f, -400.0f, -100.0f),
		physx::PxVec3(600.0f, 450.0f, -70.0f),
		physx::PxVec3(10.0f, 650.0f, 100.0f)
	};
	std::vector<PM3D::CObjetMesh> objectList;

};

