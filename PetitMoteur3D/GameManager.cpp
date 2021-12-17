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
		sceneManager.displayPause();
		startPause = horloge.GetTimeCount();
	}
	else {
		sceneManager.hidePause();
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

	if (onTitleScreen)
	{
		// E-Z mode
		if (GestionnaireDeSaisie->ToucheAppuyee(DIK_1))
		{
			onTitleScreen = false;
			isPause = false;
			sceneManager.hidePause();
			sceneManager.getSpriteManager()->changePauseToPauseUI();
			chronoStart = horloge.GetTimeCount();

			hardmode = false;
		}
		// HARD mode
		if (GestionnaireDeSaisie->ToucheAppuyee(DIK_2))
		{
			onTitleScreen = false;
			isPause = false;
			sceneManager.hidePause();
			sceneManager.getSpriteManager()->changePauseToPauseUI();
			chronoStart = horloge.GetTimeCount();

			hardmode = true;
		}
	}
	else
	{
		// game running
		if (gameOverStatus)
		{
			// TODO : Special pause for game over. Softlock.
			// Needs to watch for reset.
		}
		else
		{
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



		updateShader();

		
		updateSpeed();
		updateChrono();
		sceneManager.Anime(activeZone, tempsEcoule);

				float distance = (sceneManager.player->body->getGlobalPose().p - sceneManager.zonesCenters[static_cast<int>(activeZone)]).magnitude();
				if (distance > 2500.0f)
				{
					gameOver(false);
				}
			}
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

const Zone& GameManager::getActiveZone() {
	return activeZone;
}

void GameManager::setActiveZone(Zone zone) {
	activeZone = zone;
}

void GameManager::setNextZone(Zone zone) {
	nextZone = zone;
}

void GameManager::setLastTeleportTime()
{
	lastTeleport = horloge.GetTimeCount();
}

int GameManager::getShaderTechnique()
{
	return currentPosteffectShader;
}

void GameManager::setShaderTechniqueToBlur()
{
	currentPosteffectShader = chosenBlurPosteffectShader;
}

void GameManager::setShaderTechniqueToClear()
{
	currentPosteffectShader = chosenPosteffectShader;
}

bool GameManager::IsHardModeOn()
{
	return hardmode;
}


bool GameManager::isGreenKeyCollected()
{
	return greenKeyCollected;
}

bool GameManager::isBlueKeyCollected()
{
	return false;
}

bool GameManager::isRedKeyCollected()
{
	return hardmode;
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
	sceneManager.activateFinalPortal();
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
				sceneManager.getSpriteManager()->afficherCle(1);
			}
			else if (obj->getType() == PickUpObjectType::BlueKey)
			{
				blueKeyCollected = true;
				sceneManager.getSpriteManager()->afficherCle(0);
			}
			else if (obj->getType() == PickUpObjectType::RedKey)
			{
				redKeyCollected = true;
				sceneManager.getSpriteManager()->afficherCle(2);
			}
			else if (obj->getType() == PickUpObjectType::SpeedBuff)
			{
				speedBuffCollected = true;
				sm.player->setSpeed(sm.player->getSpeed() + 10);
			}
			sceneManager.getSpriteManager()->removeBillboardAtPos(static_cast<int>(activeZone), { pos.x, pos.y, pos.z });
			PickUpObjectList.erase(It);
			break;
		}
	}
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
	const int  millisec = static_cast<int>(((diff * secPerCount) - sec - (min*60)) * 1000);

	std::wstring hourStr = std::to_wstring(hour);
	std::wstring minStr = std::to_wstring(min);
	std::wstring secStr = std::to_wstring(sec); 
	std::wstring millisecStr = std::to_wstring(millisec);

	if (sec < 10)
		secStr = L"0"s + secStr; // affichage en style X:00	
	if (min < 10)
		minStr = L"0"s + minStr; // affichage en style X:00
	if (millisec < 100)
		millisecStr = L"0"s + millisecStr;
	if (millisec < 10)
		millisecStr = L"0"s + millisecStr;

	sceneManager.GetpChronoTexte()->Ecrire(hourStr + L"h"s + minStr + L"m"s + secStr + L"s" + millisecStr, sceneManager.GetpBrush());
}


void GameManager::updateSpeed() 
{
	float plrSpeed = sceneManager.player->body->getLinearVelocity().magnitude();

	std::wstring speedStr = std::to_wstring((int)plrSpeed);

	sceneManager.GetpVitesseTexte()->Ecrire(speedStr, sceneManager.GetpBrush());
	sceneManager.getSpriteManager()->updateGauge((int)plrSpeed);
}

void GameManager::gameOver(bool _win)
{
	// Do stuff that makes the game is over
	gameOverStatus = true;

	// stuff for the timer
	const int64_t currentTime = horloge.GetTimeCount();
	const int64_t diff = currentTime + totalPauseTime - chronoStart;
	const double secPerCount = horloge.GetSecPerCount();
	const int mintmp = static_cast<int>((diff * secPerCount) / 60);
	const int hour = mintmp / 60;
	const int min = mintmp % 60;
	const int sec = static_cast<int>(diff * secPerCount) % 60;
	const int  millisec = static_cast<int>(((diff * secPerCount) - sec - (min * 60)) * 1000);

	std::wstring hourStr = std::to_wstring(hour);
	std::wstring minStr = std::to_wstring(min);
	std::wstring secStr = std::to_wstring(sec);
	std::wstring millisecStr = std::to_wstring(millisec);

	if (sec < 10)
		secStr = L"0"s + secStr; // affichage en style X:00	
	if (min < 10)
		minStr = L"0"s + minStr; // affichage en style X:00
	if (millisec < 100)
		millisecStr = L"0"s + millisecStr;
	if (millisec < 10)
		millisecStr = L"0"s + millisecStr;

	sceneManager.changePauseToGameOver(_win, hourStr + L"h"s + minStr + L"m"s + secStr + L"s" + millisecStr);
	setPauseMenu(true);
}

void GameManager::setChronoStart()
{
	chronoStart = horloge.GetTimeCount();
}

void GameManager::updateShader()
{
	if (currentPosteffectShader == chosenBlurPosteffectShader) 
	{
		if (horloge.GetTimeBetweenCounts(lastTeleport, horloge.GetTimeCount()) > blurShaderFadeTimer)
		{
			setShaderTechniqueToClear();
		}
	}
}
