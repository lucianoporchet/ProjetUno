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

		physXManager.stepPhysics(static_cast<int>(activeZone));


		if (activeZone != nextZone) {
			Zone pastZone = activeZone;
			physXManager.removeActor(*sceneManager.player->body, static_cast<int>(activeZone));
			activeZone = nextZone;
			physXManager.addToScene(sceneManager.player->body, static_cast<int>(activeZone));
			PxQuat qua = sceneManager.player->body->getGlobalPose().q;
			sceneManager.player->body->setGlobalPose(PxTransform(sceneManager.getPortalPos(activeZone, pastZone), qua));
		}
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

bool GameManager::allKeysCollected()
{
	if (greenKeyCollected && blueKeyCollected && redKeyCollected) 
	{
		return true;
	}
	return false;
}

void GameManager::activateFinalPortal()
{
	// TODO
}

void GameManager::activatePickUpObjectFromPos(PxVec3 pos)
{
	SceneManager& sm = sceneManager;
	/*std::unique_ptr<PickUpObject> objectActivated;*/
	std::vector<std::unique_ptr<PickUpObject>>& PickUpObjectList = sm.getListPickUpObjectScene(static_cast<int>(activeZone));
	/*for (auto& obj : PickUpObjectList)*/
	for (auto It = PickUpObjectList.begin(); It != PickUpObjectList.end(); It++)
	{
		auto& obj = *It;
		if (obj->body->getGlobalPose().p == pos) 
		{
			if (obj->getType() == PickUpObjectType::GreenKey) 
			{
				greenKeyCollected = true;
			} 
			else if (obj->getType() == PickUpObjectType::BlueKey)
			{
				blueKeyCollected = true;
			}
			else if (obj->getType() == PickUpObjectType::RedKey)
			{
				redKeyCollected = true;
			}
			else if (obj->getType() == PickUpObjectType::SpeedBuff) 
			{
				speedBuffCollected = true;
				sm.player->setSpeed(sm.player->getSpeed() + 100);
			}
			PickUpObjectList.erase(It);
			break;
		}
	}
	// TODO : depending of the position of pickup object, activate its effects, then despawn it
}
