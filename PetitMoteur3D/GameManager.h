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
	static GameManager& get() noexcept { return instance; }
	void setPauseMenu(bool) noexcept;
	bool getIsPauseStatus() noexcept;
	bool hasBeenEnoughTimeSinceLastPause();
	bool AnimeScene(float tempsEcoule);
	void setGestionnaireDeSaisie(PM3D::CDIManipulateur&);
	SceneManager& getSceneManager();
	PM3D::CDIManipulateur* GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }

	
private:
	PM3D::Horloge horloge;
	int64_t lastPaused = 0;
	bool isPause = false;

	//PhysX manager
	PhysXManager& physXManager = PhysXManager::get();

	//gestionnaire de saisie
	PM3D::CDIManipulateur* GestionnaireDeSaisie;

	//scene manager
	SceneManager& sceneManager = SceneManager::get();

protected :
	

};



