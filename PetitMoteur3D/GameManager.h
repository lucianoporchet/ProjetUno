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
	bool hasBeenEnoughTimeSinceLastInput();
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
	void updateSpeed();
	void restartGame();
	void gameOver(bool _win);

	void setChronoStart();
	
private:

	int currentPosteffectShader = 0;
	int chosenPosteffectShader = 0;
	int chosenBlurPosteffectShader = 1;
	int blurShaderFadeTimer = 3;

	int indexShader = 0;
	const std::vector<std::pair<int, int>> shadersVector = {
		{0, 1},
		{2, 6},
		{3, 7},
		{4, 8},
		{5, 9}
	};

	PM3D::Horloge horloge;
	int64_t lastPaused = 0;
	int64_t lastInput = 0;
	int64_t totalPauseTime = 0;
	int64_t startPause = 0;
	int64_t lastTeleport = horloge.GetTimeCount();;
	//attention a modifier si on implémente un menu de démarrage car le chrono ne commencerait pas avant sinon
	int64_t chronoStart = horloge.GetTimeCount();
	bool hardmode = true;
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



