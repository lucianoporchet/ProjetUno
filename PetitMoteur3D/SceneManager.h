#pragma once
#include <vector>
#include "PxPhysicsAPI.h"
#include "ObjetMesh.h"
#include "Objet3D.h"

class SceneManager
{
public:

	const physx::PxVec3 planetePos[15] = {
		physx::PxVec3(1032.0f, -782.0f, 0.0f),
		physx::PxVec3(-877.0f, -12520.0f, 296.0f),
		physx::PxVec3(-103.0f, -947.0f, -890.0f),
		physx::PxVec3(-1234.0f, 94.0f, 833.0f),
		physx::PxVec3(1075.0f, -186.0f, 805.0f),
		physx::PxVec3(133.0f, 1021.0f, 785.0f),
		physx::PxVec3(486.0f, 208.0f, 25.0f),
		physx::PxVec3(-1407.0f, 1061.0f, 399.0f),
		physx::PxVec3(1082.0f, 775.0f, -282.0f),
		physx::PxVec3(-828.0f, -96.0f, -271.0f),
		physx::PxVec3(47.0f, -485.0f, -712.0f),
		physx::PxVec3(-542.0f, 729.0f, -935.0f),
		physx::PxVec3(-1337.0f, -414.0f, -1158.0f),
		physx::PxVec3(625.0f, 1311.0f, -1086.0f),
		physx::PxVec3(834.0f, -1237.0f, -1163.0f)
	};
	std::vector<PM3D::CObjetMesh> objectList;

	std::vector<std::unique_ptr<PM3D::CObjet3D>>& getListScene();

private :

	// La seule scène
	std::vector<std::unique_ptr<PM3D::CObjet3D>> ListeScene;
};

