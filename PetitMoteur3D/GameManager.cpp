#include "stdafx.h"
#include "GameManager.h"

//instance du game manager
GameManager GameManager::instance;


void GameManager::cleanManager()
{
	sceneManager.getScenes().clear();
}

//set la pause et donc afficher ou effacer le curseur
void GameManager::setPauseMenu(bool toShow) noexcept
{
	ShowCursor(toShow);
	isPause = toShow;
	if (toShow)
	{
		startPause = horloge.GetTimeCount();
	}
	else {
		totalPauseTime += startPause - horloge.GetTimeCount();
	}
	
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

	//si on est pas sur le menu pause
	if (!getIsPauseStatus()) {

		if (activeZone != nextZone) {
			Zone pastZone = activeZone;
			physXManager.removeActor(*sceneManager.player->body, static_cast<int>(activeZone));
			activeZone = nextZone;
			physXManager.addToScene(sceneManager.player->body, static_cast<int>(activeZone));
			PxQuat qua = sceneManager.player->body->getGlobalPose().q;
			sceneManager.player->body->setGlobalPose(PxTransform(sceneManager.getPortalPos(activeZone, pastZone), qua));
		}

		physXManager.stepPhysics(static_cast<int>(activeZone));


		

		updateChrono();
		sceneManager.Anime(activeZone, tempsEcoule);
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

const Zone& GameManager::getActiveZone() {
	return activeZone;
}

void GameManager::setActiveZone(Zone zone) {
	activeZone = zone;
}

void GameManager::setNextZone(Zone zone) {
	nextZone = zone;
}

void GameManager::updateChrono()
{
	const int64_t currentTime = horloge.GetTimeCount();
	const int64_t diff = currentTime + totalPauseTime - chronoStart;
	const double secPerCount = horloge.GetSecPerCount();
	const int mintmp = static_cast<int>((diff * secPerCount) / 60);
	const int hour = mintmp / 60;
	const int min = mintmp % 60;
	const int sec = static_cast<int>(diff * secPerCount) % 60;
	const int  millisec = static_cast<int>(((diff * secPerCount) - sec) * 1000);

	std::wstring hourStr = std::to_wstring(hour);
	std::wstring minStr = std::to_wstring(min);
	std::wstring secStr = std::to_wstring(sec); 
	std::wstring millisecStr = std::to_wstring(millisec);

	sceneManager.GetpChronoTexte()->Ecrire(hourStr + L"h"s + minStr + L"m"s + secStr + L"s "s + millisecStr, sceneManager.GetpBrush());
}
