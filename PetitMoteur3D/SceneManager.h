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
	std::unique_ptr<PM3D::CAfficheurTexte> pGameOverTexte;
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

	void activateFinalPortal();

	void changePauseToGameOver(bool _gameWon, std::wstring _finalTime);

private:
	
	//std::unique_ptr<PM3D::CObjet3D> pFinalPortal;
	std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>> Scenes{};
	std::map<int, std::unique_ptr<Monster>> Monsters;
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

	const physx::PxVec3 planetePos2[NBPLANETES] = {
		physx::PxVec3(6627.0f, 7576.0f, -1136.0f),
		physx::PxVec3(4594.0f, 7326.0f, 349.0f),
		physx::PxVec3(6135.0f, 7286.0f, 735.0f),
		physx::PxVec3(7084.0f, 7040.0f, -332.0f),
		physx::PxVec3(5459.0f, 6994.0f, -985.0f),
		physx::PxVec3(6488.0f, 6473.0f, -24.0f),
		physx::PxVec3(4767.0f, 6359.0f, 783.0f),
		physx::PxVec3(5173.0f, 6168.0f, -321.0f),
		physx::PxVec3(7077.0f, 6078.0f, 754.0f),
		physx::PxVec3(4663.0f, 5850.0f, -1208.0f),
		physx::PxVec3(6049.0f, 5779.0f, -762.0f),
		physx::PxVec3(7034.0f, 5482.0f, -50.0f),
		physx::PxVec3(5897.0f, 5317.0f, 840.0f),
		physx::PxVec3(6835.0f, 5027.0f, -1213.0f),
		physx::PxVec3(5123.0f, 5011.0f, 246.0f)
	};

	const physx::PxVec3 portalPos[NBPORTAILS] = {
	physx::PxVec3(1153.0f, -617.0f, 493.0f), physx::PxVec3(-1153.0f, 617.0f, -493.0f),		//Zone1
	physx::PxVec3(-1365.0f, 4916.0f, 283.0f), physx::PxVec3(1411.0f, 7352.0f, -343.0f),		//Zone2
	physx::PxVec3(4845.0f, 6825.0f, 602.0f), physx::PxVec3(6331.0f, 4896.0f, 602.0f),		//Zone3
	physx::PxVec3(7038.0f, 871.0f, 249.0f), physx::PxVec3(5823.0f, -923.0f, -1492.0f)		//zone4
	};


	const XMFLOAT3 finalPortalPos = { 6000.0f, 0.0f, 0.0f }; // Zone 3

	const physx::PxVec3 monsterPos[NBMONSTRES] = {
		physx::PxVec3(-1000.0f, -1000.0f, -1000.0f),
		physx::PxVec3(0, BOXSIZE, 0),
		physx::PxVec3(BOXSIZE, BOXSIZE, 0.0f),
		physx::PxVec3(BOXSIZE, 0.0f, 0.0f),
	};


	const PickUpObjectPlacementInfo pickupObjectsInfo[NBPICKUPOBJECTS]{
		{ physx::PxVec3(500, 500, 500) , PickUpObjectType::RedKey , 0 },
		{ physx::PxVec3(96.0f, 5572.0f, -359.0f) , PickUpObjectType::GreenKey , 1 },
		{ physx::PxVec3(5204.0f, 5439.0f, -505.0f) , PickUpObjectType::BlueKey , 2 },
		{ physx::PxVec3(-300, -300, -300) , PickUpObjectType::SpeedBuff , 0 },
		{ physx::PxVec3(-150, 300 + BOXSIZE, 100) , PickUpObjectType::SpeedBuff , 1 },
		{ physx::PxVec3(-1000 + BOXSIZE, -300, 1000) , PickUpObjectType::SpeedBuff , 3 },
		{ physx::PxVec3(BOXSIZE + 500, BOXSIZE - 500, -600) , PickUpObjectType::SpeedBuff , 2 },
		{ physx::PxVec3(400, BOXSIZE - 600, 500) , PickUpObjectType::SpeedBuff , 1 },
		{ physx::PxVec3(-1000, -300, 600) , PickUpObjectType::SpeedBuff , 0 },
		{ physx::PxVec3(BOXSIZE - 1000, BOXSIZE + 600, 300) , PickUpObjectType::SpeedBuff , 2 },

	};
	
	const physx::PxVec3 tunnelPos[NBTUNNELCOMPONENTS] = {
		physx::PxVec3(242.035f, 5764.03f, 154.102f),//dessous
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

	const physx::PxVec3 zonesCenters[NBZONES] = {
	physx::PxVec3(0.0f, 0.0f, 0.0f),
	physx::PxVec3(0, BOXSIZE, 0),
	physx::PxVec3(BOXSIZE, BOXSIZE, 0.0f),
	physx::PxVec3(BOXSIZE, 0.0f, 0.0f)
	};

	std::unique_ptr<Player> player;
	std::unique_ptr<PM3D::CTerrain> terrain;
};







