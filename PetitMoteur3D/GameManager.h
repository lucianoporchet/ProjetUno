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
	void cleanManager();
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

	bool isGreenKeyCollected();
	bool isBlueKeyCollected();
	bool isRedKeyCollected();
	bool allKeysCollected();
	void activateFinalPortal();
	void activatePickUpObjectFromPos(PxVec3 pos);

	void updateChrono();
	void updateSpeed();

	void gameOver(bool _win);

	void setChronoStart();
	
private:

	PM3D::Horloge horloge;
	int64_t lastPaused = 0;
	int64_t totalPauseTime = 0;
	int64_t startPause = 0;
	int64_t chronoStart;
	bool isPause = true;
	bool onTitleScreen = true;
	bool gameOverStatus = false;
	bool gameWon = false;
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



