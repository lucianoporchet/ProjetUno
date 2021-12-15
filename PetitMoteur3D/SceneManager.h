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
#include "Terrain.h"
#include "Asteroid.h"
#include "Planet.h"
#include "Portal.h"
#include "Monster.h"
#include "GestionnaireDeTextures.h"
#include "TunnelComponent.h"
#include "RandomGenerator.h"
#include <future>
#include <mutex>
#include "PickUpObject.h"
#include "AfficheurSprite.h"
#include "AfficheurTexte.h"



enum class Zone {
	ZONE1,
	ZONE2,
	ZONE3,
	ZONE4
};


class SceneManager
{
protected :
	// Pour le texte
	std::unique_ptr<PM3D::CAfficheurTexte> pChronoTexte;
	std::unique_ptr<PM3D::CAfficheurTexte> pVitesseTexte;
	std::wstring str;
	std::unique_ptr<Gdiplus::Font> pPolice;
	std::unique_ptr<Gdiplus::SolidBrush> pBrush;

	
private:
	std::unique_ptr<PM3D::CAfficheurSprite> spriteManager;
	bool pauseStatus = false;
	SceneManager();
public:

	PM3D::CAfficheurSprite* getSpriteManager() { return spriteManager.get(); };
	std::vector<std::unique_ptr<PM3D::CObjet3D>>& getListScene(int scene);
	std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>>& getScenes() noexcept;

	
	std::vector<std::unique_ptr<PickUpObject>>& getListPickUpObjectScene(int scene);
	std::vector<std::vector<std::unique_ptr<PickUpObject>>>& getPickUpObjectScenes() noexcept;

	
	void InitObjects(PM3D::CDispositifD3D11* pDispositif, PM3D::CGestionnaireDeTextures& TexturesManager, PM3D::CCamera& camera);
	physx::PxVec3 getPortalPos(Zone current, Zone past);
	void Draw(Zone scene);
	void Anime(Zone scene, float tmps);
	PM3D::CAfficheurTexte* GetpChronoTexte();
	PM3D::CAfficheurTexte* GetpVitesseTexte();
	Gdiplus::SolidBrush* GetpBrush();

	void displayPause();
	void hidePause();
	static SceneManager& get() noexcept;
	const float getBoxSize();

private:
	
	std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>> Scenes{};
	std::vector<std::vector<std::unique_ptr<PickUpObject>>> PickUpObjectsScenes{};
	/*std::unordered_map<PxVec3, std::shared_ptr<PM3D::CObjet3D>> pickUpObjectsPosition;*/
	const float BOXSIZE{ 6000.0f };
	enum {

		NBASTEROIDES = 100,
		NBZONES = 4,
		NBPLANETES = 15,
		NBMONSTRES = 4,
		NBPORTAILS = 8,
		NBPICKUPOBJECTS = 10,
		NBETOILES = 256,
		NBTUNNELCOMPONENTS = 4
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

	const physx::PxVec3 portalPos[NBPORTAILS] = {
	physx::PxVec3(1153.0f, -617.0f, 493.0f), physx::PxVec3(-1153.0f, 617.0f, -493.0f),		//Zone1
	physx::PxVec3(-1365.0f, 4916.0f, 283.0f), physx::PxVec3(1411.0f, 7352.0f, -343.0f),		//Zone2
	physx::PxVec3(4845.0f, 6825.0f, 602.0f), physx::PxVec3(6331.0f, 4896.0f, 602.0f),		//Zone3
	physx::PxVec3(7038.0f, 871.0f, -1732.0f), physx::PxVec3(4807.0f, -1605.0f, -1732.0f)	//zone4
	};


	const physx::PxVec3 monsterPos[NBMONSTRES] = {
		physx::PxVec3(-1000.0f, -1000.0f, -1000.0f),
		physx::PxVec3(0, BOXSIZE, 0),
		physx::PxVec3(BOXSIZE, BOXSIZE, 0.0f),
		physx::PxVec3(BOXSIZE, 0.0f, 0.0f),
	};


	const PickUpObjectPlacementInfo pickupObjectsInfo[NBPICKUPOBJECTS]{
		{ physx::PxVec3(500, 500, 500) , PickUpObjectType::RedKey , 0 },
		{ physx::PxVec3(0, BOXSIZE + 500, 0) , PickUpObjectType::GreenKey , 1 },
		{ physx::PxVec3(BOXSIZE + 500, BOXSIZE + 500, 0) , PickUpObjectType::BlueKey , 2 },
		{ physx::PxVec3(-300, -300, -300) , PickUpObjectType::SpeedBuff , 0 },
		{ physx::PxVec3(-150, 300 + BOXSIZE, 100) , PickUpObjectType::SpeedBuff , 1 },
		{ physx::PxVec3(-1000 + BOXSIZE, -300, 1000) , PickUpObjectType::SpeedBuff , 3 },
		{ physx::PxVec3(BOXSIZE + 500, BOXSIZE - 500, -600) , PickUpObjectType::SpeedBuff , 2 },
		{ physx::PxVec3(400, BOXSIZE - 600, 500) , PickUpObjectType::SpeedBuff , 1 },
		{ physx::PxVec3(-1000, -300, 600) , PickUpObjectType::SpeedBuff , 0 },
		{ physx::PxVec3(BOXSIZE - 1000, BOXSIZE + 600, 300) , PickUpObjectType::SpeedBuff , 2 },

	};
	
	const physx::PxVec3 tunnelPos[NBTUNNELCOMPONENTS] = {
		physx::PxVec3(242.035f, 5764.03f, 154.102f),
		physx::PxVec3(17.926f, 5574.52f, -748.041f),
		physx::PxVec3(-166.146f, 6164.27f, -28.0985f),
		physx::PxVec3(57.9616f, 6353.78f, 874.045f)
	};

	const physx::PxVec3 tunnelScale[NBTUNNELCOMPONENTS] = {
		physx::PxVec3(1000.0f, 1000.0f, 100.0f),
		physx::PxVec3(100.0f, 1000.0f, 200.0f),
		physx::PxVec3(1000.0f, 1000.0f, 100.0f),
		physx::PxVec3(100.0f, 1000.0f, 200.0f)
	};

	physx::PxQuat tunnelRot = physx::PxQuat(0.130f, 0.576f, -0.424f, 0.687f);
	

public:
	std::unique_ptr<Player> player;
	std::unique_ptr<PM3D::CTerrain> terrain;
};







