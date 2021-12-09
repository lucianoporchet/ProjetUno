#pragma once
#include <vector>
#include "PxPhysicsAPI.h"
#include "ObjetMesh.h"
#include "Objet3D.h"
#include "Bloc.h"
#include "BlocEffet1.h"
#include "ChargeurOBJ.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Asteroid.h"
#include "Planet.h"
#include "Portal.h"
#include "GestionnaireDeTextures.h"
#include "RandomGenerator.h"
#include <future>
#include <mutex>


enum class Zone {
	ZONE1,
	ZONE2,
	ZONE3,
	PORTAIL
};


class SceneManager
{
	
private:
	SceneManager();
public:
	
	std::vector<PM3D::CObjetMesh> objectList;

	std::vector<std::unique_ptr<PM3D::CObjet3D>>& getListScene(int scene);
	std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>>& getScenes() noexcept;
	
	void InitObjects(PM3D::CDispositifD3D11* pDispositif, PM3D::CGestionnaireDeTextures& TexturesManager, PM3D::CCamera& camera);
	physx::PxVec3 getPortalPos(Zone scene);
	void Draw(Zone scene);
	void Anime(Zone scene, float tmps);
	static SceneManager& get() noexcept;

private:
	
	std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>> Scenes;
	const float BOXSIZE{ 6000.0f };
	enum {

		NBASTEROIDES = 4,
		NBZONES = 4,
		NBPLANETES = 15,
		NBPORTAILS = 7
	};
	const physx::PxVec3 planetePos1[NBPLANETES] = {
	physx::PxVec3(1032.0f, -782.0f, 0.0f),
	physx::PxVec3(-877.0f, -1252.0f, 296.0f),
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

	const physx::PxVec3 planetePos2[NBPLANETES] = {
	physx::PxVec3(1032.0f, 2218.0f, 0.0f),
	physx::PxVec3(-877.0f, 1748.0f, 296.0f),
	physx::PxVec3(-103.0f, 2053, -890.0f),
	physx::PxVec3(-1234.0f, 3094.0f, 833.0f),
	physx::PxVec3(1075.0f, 2814.0f, 805.0f),
	physx::PxVec3(133.0f, 4021.0f, 785.0f),
	physx::PxVec3(486.0f, 3208.0f, 25.0f),
	physx::PxVec3(-1407.0f, 4061.0f, 399.0f),
	physx::PxVec3(1082.0f, 3775.0f, -282.0f),
	physx::PxVec3(-828.0f, 2904.0f, -271.0f),
	physx::PxVec3(47.0f, 2515.0f, -712.0f),
	physx::PxVec3(-542.0f, 3729.0f, -935.0f),
	physx::PxVec3(-1337.0f, 2586.0f, -1158.0f),
	physx::PxVec3(625.0f, 4311.0f, -1086.0f),
	physx::PxVec3(834.0f, 1763.0f, -1163.0f)
	};

	const physx::PxVec3 planetePos3[NBPLANETES] = {
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

	const physx::PxVec3 portalPos[NBPORTAILS] = {
	physx::PxVec3(-1153.0f, 617.0f, -493.0f),
	physx::PxVec3(-375.0f, 5296.0f, -343.0f),
	physx::PxVec3(693.0f, 7017.0f, -343.0f),
	physx::PxVec3(4845.0f, 6825.0f, 602.0f),
	physx::PxVec3(6331.0f, 4896.0f, 602.0f),
	physx::PxVec3(5506.0f, 871.0f, -1732.0f),
	physx::PxVec3(6393.0f, -1605.0f, -1732.0f)
	};

public:
	std::unique_ptr<Player> player;
};







