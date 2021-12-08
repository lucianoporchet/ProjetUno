#include "stdafx.h"
#include "GameManager.h"

//instance du game manager
GameManager GameManager::instance;


//set la pause et donc afficher ou effacer le curseur
void GameManager::setPauseMenu(bool toShow) noexcept
{
	ShowCursor(toShow);
	isPause = toShow;
}

//retourne le statut pour savoir si la pause est active
bool GameManager::getIsPauseStatus() noexcept
{
	return isPause;
}

//empeche de spam la pause et empeche la touche de bounce pour afficher le menu pause
bool GameManager::hasBeenEnoughTimeSinceLastPause()
{
	if (horloge.GetTimeBetweenCounts(lastPaused, horloge.GetTimeCount()) >= 1)
	{
		lastPaused = horloge.GetTimeCount();
		return true;
	}
	return false;
}

bool GameManager::AnimeScene(float tempsEcoule) {
	
	// Prendre en note le statut du clavier
	GestionnaireDeSaisie->StatutClavier();
	// Prendre en note l'état de la souris
	GestionnaireDeSaisie->SaisirEtatSouris();

	if ((GestionnaireDeSaisie->ToucheAppuyee(DIK_ESCAPE)) && hasBeenEnoughTimeSinceLastPause())
	{
		if (getIsPauseStatus())
		{
			setPauseMenu(false);
		}
		else {
			setPauseMenu(true);
		}
	}

	//si on est sur le menu pause
	if (!getIsPauseStatus()) {

		physXManager.stepPhysics();
		for (auto& object3D : sceneManager.getListScene(0))
		{
			object3D->Anime(tempsEcoule);
		}
	}

	return true;
}

void GameManager::setGestionnaireDeSaisie(PM3D::CDIManipulateur& g)
{
	GestionnaireDeSaisie = &g;
}

SceneManager& GameManager::getSceneManager()
{
	return sceneManager;
}
