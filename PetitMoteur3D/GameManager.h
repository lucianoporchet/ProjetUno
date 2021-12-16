#pragma once
#include "GSingleton.h"
#include "Horloge.h"
#include "PhysXManager.h"
#include "DIManipulateur.h"
#include "SceneManager.h"

//singleton de la classe du game manager
class GameManager : public Incopiable {
	static GameManager instance;
	GameManager() = default;

public:
	inline static GameManager& get() noexcept { return instance; }
	void setPauseMenu(bool) noexcept;
	bool getIsPauseStatus() noexcept;
	bool hasBeenEnoughTimeSinceLastPause();
	bool AnimeScene(float tempsEcoule);
	void setGestionnaireDeSaisie(PM3D::CDIManipulateur&);
	inline PM3D::CDIManipulateur* GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }
	SceneManager& getSceneManager();
	const Zone& getActiveZone();
	void setActiveZone(Zone zone);
	void setNextZone(Zone zone);
	void setLastTeleportTime();
	int getShaderTechnique();
	void setShaderTechniqueToBlur();
	void setShaderTechniqueToClear();

	bool IsHardModeOn();

	bool isGreenKeyCollected();
	bool isBlueKeyCollected();
	bool isRedKeyCollected();
	bool allKeysCollected();
	void activateFinalPortal();
	void activatePickUpObjectFromPos(PxVec3 pos);

	void updateChrono();
	void updateShader();

	
private:

	int currentPosteffectShader = 0;
	int chosenPosteffectShader = 0;
	int chosenBlurPosteffectShader = 1;
	int blurShaderFadeTimer = 3;
	PM3D::Horloge horloge;
	int64_t lastPaused = 0;
	int64_t totalPauseTime = 0;
	int64_t startPause = 0;
	int64_t lastTeleport = horloge.GetTimeCount();;
	//attention a modifier si on implémente un menu de démarrage car le chrono ne commencerait pas avant sinon
	int64_t chronoStart = horloge.GetTimeCount();
	bool hardmode = true;
	bool isPause = false;
	Zone activeZone = Zone::ZONE1;
	Zone nextZone = Zone::ZONE1;
	bool greenKeyCollected = false;
	bool blueKeyCollected = false;
	bool redKeyCollected = false;
	bool speedBuffCollected = false;

	//PhysX manager
	PhysXManager& physXManager = PhysXManager::get();

	//gestionnaire de saisie
	PM3D::CDIManipulateur* GestionnaireDeSaisie;

	//scene manager
	SceneManager& sceneManager = SceneManager::get();

protected :
	

};



