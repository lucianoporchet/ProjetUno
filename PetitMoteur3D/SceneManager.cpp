#include "stdafx.h"
#include "SceneManager.h"
#include "Terrain.h"
#include "LectureFichier.h"
#include "AfficheurSprite.h"

#include <functional>
#include "MoteurWindows.h"

using namespace std::literals;
static std::mutex objMutex;

template<class T>
void load(std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>>* Scenes,
		  std::string&& nomfichier, 
		  PM3D::CDispositifD3D11* _pDispositif,
		  float scale, 
		  physx::PxVec3 pos,
		  int scene,
		  std::function<void(T*)> func)
{
	std::unique_ptr<T> obj = std::make_unique<T>(nomfichier, _pDispositif, scale, pos, scene);
	func(obj.get());
	std::lock_guard<std::mutex> lock(objMutex);
	if(Scenes) (*Scenes)[scene].push_back(std::move(obj));
}

template<class T>
void loadMonster(std::map<int, std::unique_ptr<Monster>>* Scenes,
	std::string&& nomfichier,
	PM3D::CDispositifD3D11* _pDispositif,
	float scale,
	physx::PxVec3 pos,
	int scene,
	std::function<void(T*)> func)
{
	std::unique_ptr<T> obj = std::make_unique<T>(nomfichier, _pDispositif, scale, pos, scene);
	func(obj.get());
	std::lock_guard<std::mutex> lock(objMutex);
	if (Scenes) (*Scenes)[scene] = std::move(obj);
}

template<class T>
void loadPickUp(std::vector<std::vector<std::unique_ptr<PickUpObject>>>* Scenes,
	std::string&& nomfichier,
	PM3D::CDispositifD3D11* _pDispositif,PickUpObjectType objType,
	float scale,
	physx::PxVec3 pos,
	int scene,
	std::function<void(T*)> func)
{
	std::unique_ptr<T> obj = std::make_unique<T>(nomfichier, _pDispositif,objType, scale, pos, scene);
	func(obj.get());
	std::lock_guard<std::mutex> lock(objMutex);
	if (Scenes) (*Scenes)[scene].push_back(std::move(obj));
}


std::vector<std::unique_ptr<PM3D::CObjet3D>>& SceneManager::getListScene(int scene)
{
	return Scenes[scene];
}

std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>>& SceneManager::getScenes() noexcept
{
	return Scenes;
}

std::vector<std::unique_ptr<PickUpObject>>& SceneManager::getListPickUpObjectScene(int scene)
{
	return PickUpObjectsScenes[scene];
}

std::vector<std::vector<std::unique_ptr<PickUpObject>>>& SceneManager::getPickUpObjectScenes() noexcept
{
	return PickUpObjectsScenes;
}
//std::vector<std::unique_ptr<PickUpObject>>& SceneManager::getListPickUpObjectScene(int scene)
//{
//	return PickUpObjectsScenes[scene];
//}
//
//std::vector<std::vector<std::unique_ptr<PickUpObject>>>& SceneManager::getPickUpObjectScenes() noexcept
//{
//	return PickUpObjectsScenes;
//}


void SceneManager::InitObjects(PM3D::CDispositifD3D11* pDispositif, PM3D::CGestionnaireDeTextures& TexturesManager, PM3D::CCamera& camera) {

	std::vector<std::future<void>> futures;

	//auto f = [&](Player* p) { p->setCam(&camera); };


	for (int i = 0; i < NBZONES; ++i) {
		//Creation de la fausse skyBox (cube avec le culling inversÃ©)
		std::unique_ptr<PM3D::CBlocEffet1> skybox = std::make_unique<PM3D::CBlocEffet1>(BOXSIZE, BOXSIZE, BOXSIZE, pDispositif, i);
		//ajoute une texture a la skybox
		std::wstring texture = L".\\modeles\\SkyBoxes\\" + std::to_wstring(i);
		skybox->SetTextures(TexturesManager.GetNewTexture(texture + L"\\up.dds"s, pDispositif),
							TexturesManager.GetNewTexture(texture + L"\\down.dds", pDispositif),
							TexturesManager.GetNewTexture(texture + L"\\left.dds", pDispositif),
							TexturesManager.GetNewTexture(texture + L"\\right.dds", pDispositif),
							TexturesManager.GetNewTexture(texture + L"\\back.dds", pDispositif),
							TexturesManager.GetNewTexture(texture + L"\\front.dds", pDispositif));

		Scenes[i].push_back(std::move(skybox));
	}

	for (int i = 0; i < NBTUNNELCOMPONENTS; ++i) {
		std::unique_ptr<TunnelComponent> tunnel = std::make_unique<TunnelComponent>(".\\modeles\\VaisseauTunnel\\cube.obm", pDispositif, tunnelScale[i], tunnelPos[i], tunnelRot);
		Scenes[1].push_back(std::move(tunnel));
	}


	

	player = std::make_unique<Player>(".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f));
	player->setCam(&camera);

	//ajoute la skybox a la scene

	//Creation de 15 Planetes avec des tailles aleatoires entre 75 et 150
	int plan = 1;
	for (const auto& pos : planetePos1) {
		float scale = static_cast<float>(RandomGenerator::get().next(75, 150));
		string NB = to_string(plan);
		if (plan == 8)
		{
			plan = 1;
		}
		
		futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\" + NB + "\\Planete.obm"s, pDispositif, scale, pos, 0, [](Planet*) noexcept {}));
		++plan;
	}


	//Creation de plusieurs Asteroides avec des tailles aleatoires entre 5 et 20
	//La position des asteroides est une position aleatoire entre -1000 et -500 dans les 3 axes (posibilité de collision entre les asteroides a la creation)
	float scale{};
	PxVec3 pos;
	int z = 1, sc = 1;
	for (int j = 0; j < NBZONES; ++j) {
		if (j == 3) {z = 2; sc = 10;}
		else { z = 1; sc = 1; }
		for (int i = 0; i < NBASTEROIDES * z; ++i) {
			
			scale = static_cast<float>(RandomGenerator::get().next(5 * sc, 20 * sc));
			pos = RandomGenerator::get().randomPosInZone(j);
			futures.push_back(std::async(load<Asteroid>, &Scenes, ".\\modeles\\Asteroide\\1\\asteroide.obm"s, pDispositif, scale, pos, j, [](Asteroid*) noexcept {}));
		}
	}

	for (int i = 0; i < NBPORTAILS - 1 ; i+=2) {
		PxVec3 portalposi = portalPos[i];
		PxVec3 portalposiplus = portalPos[i+1];
		futures.push_back(std::async(load<Portal>, &Scenes, ".\\modeles\\Portal\\portal.obm"s, pDispositif, 20.0f, portalposi, i/2, [&](Portal*) noexcept {}));
		futures.push_back(std::async(load<Portal>, &Scenes, ".\\modeles\\Portal\\portal.obm"s, pDispositif, 20.0f, portalposiplus, i/2, [&](Portal*) noexcept {}));
	}

	// Initialisation du portail final
	//pFinalPortal = std::make_unique<Portal>(".\\modeles\\Portal\\portal.obm"s, PM3D::CMoteurWindows::GetInstance().pDispositif, 20.0f, physx::PxVec3{ finalPortalPos.x, finalPortalPos.y, finalPortalPos.z });

	for (int i = 0; i < NBMONSTRES; ++i) {
		scale = static_cast<float>(RandomGenerator::get().next(50, 200));
		futures.push_back(std::async(loadMonster<Monster>, &Monsters, ".\\modeles\\Monstre\\monstre.obm"s, pDispositif, scale, monsterPos[i], i%NBZONES, [](Monster*) noexcept {}));
	}

	for (int i = 0; i < NBPICKUPOBJECTS; ++i) {
		if (pickupObjectsInfo[i].objectType == PickUpObjectType::SpeedBuff) 
		{
			futures.push_back(std::async(loadPickUp<PickUpObject>, &PickUpObjectsScenes, ".\\modeles\\PickUp\\burger.obm"s, pDispositif, pickupObjectsInfo[i].objectType, 10.0f, pickupObjectsInfo[i].pos, pickupObjectsInfo[i].zoneNumber, [](PickUpObject*) noexcept {}));
		}
		else 
		{
			futures.push_back(std::async(loadPickUp<PickUpObject>, &PickUpObjectsScenes, ".\\modeles\\PickUp\\key.obm"s, pDispositif, pickupObjectsInfo[i].objectType, 10.0f, pickupObjectsInfo[i].pos, pickupObjectsInfo[i].zoneNumber, [](PickUpObject*) noexcept {}));
		}
	}

	LectureFichier lecteurHeightmap{ "smolOBJECT" };
	physx::PxQuat terrainRot = physx::PxQuat(-0.394f, 0.707f, 0.107f, 0.578f);
	terrain = std::make_unique<PM3D::CTerrain>(pDispositif, lecteurHeightmap, physx::PxVec3(-546.29f, 5567.4f, 1147.1f), 1, physx::PxVec3(4.05f, 0.9f, 5.02f), terrainRot);
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\metal2.dds", pDispositif));
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\metal1.dds", pDispositif));
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\filtre.dds", pDispositif));

	////Creation du player, constructeur avec format binaire
	//futures.push_back(std::async(load<Player>, &Scenes, ".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f), 0, f));

	// Variables utiles
	int largeur = pDispositif->GetLargeur();
	int hauteur = pDispositif->GetHauteur();

	// Creation du gestionnaire de billboards, sprites et texte
	this->spriteManager = std::make_unique<PM3D::CAfficheurSprite>(pDispositif);

	// Ajout des portails
	spriteManager->AjouterPanneau(0, true, ".\\modeles\\Billboards\\portal_red_light.dds"s, { portalPos[0].x, portalPos[0].y, portalPos[0].z }, true, 100.0f, 100.0f);
	spriteManager->AjouterPanneau(0, true, ".\\modeles\\Billboards\\portal_green_light.dds"s, { portalPos[1].x, portalPos[1].y, portalPos[1].z }, true, 100.0f, 100.0f);
	spriteManager->AjouterPanneau(1, true, ".\\modeles\\Billboards\\portal_blue_light.dds"s, { portalPos[2].x, portalPos[2].y, portalPos[2].z }, true, 100.0f, 100.0f);
	spriteManager->AjouterPanneau(1, true, ".\\modeles\\Billboards\\portal_purple_light.dds"s, { portalPos[3].x, portalPos[3].y, portalPos[3].z }, true, 100.0f, 100.0f);
	spriteManager->AjouterPanneau(2, true, ".\\modeles\\Billboards\\portal_green_light.dds"s, { portalPos[4].x, portalPos[4].y, portalPos[4].z }, true, 100.0f, 100.0f);
	spriteManager->AjouterPanneau(2, true, ".\\modeles\\Billboards\\portal_red_light.dds"s, { portalPos[5].x, portalPos[5].y, portalPos[5].z }, true, 100.0f, 100.0f);
	spriteManager->AjouterPanneau(3, true, ".\\modeles\\Billboards\\portal_purple_light.dds"s, { portalPos[6].x, portalPos[6].y, portalPos[6].z }, true, 100.0f, 100.0f);
	spriteManager->AjouterPanneau(3, true, ".\\modeles\\Billboards\\portal_blue_light.dds"s, { portalPos[7].x, portalPos[7].y, portalPos[7].z }, true, 100.0f, 100.0f);

	// Ajout des sprites pour rendre les pickupObjects plus visibles
	for (int i = 0; i < NBPICKUPOBJECTS; ++i) {
		if (pickupObjectsInfo[i].objectType == PickUpObjectType::SpeedBuff)
		{
			spriteManager->AjouterPanneau(pickupObjectsInfo[i].zoneNumber, false, ".\\modeles\\Billboards\\fastfood_billboard.dds"s, { pickupObjectsInfo[i].pos.x, pickupObjectsInfo[i].pos.y, pickupObjectsInfo[i].pos.z }, true, 50.0f, 50.0f);
		}
		else
		{
			spriteManager->AjouterPanneau(pickupObjectsInfo[i].zoneNumber, false, ".\\modeles\\Billboards\\key_billboard.dds"s, { pickupObjectsInfo[i].pos.x, pickupObjectsInfo[i].pos.y, pickupObjectsInfo[i].pos.z }, true, 50.0f, 50.0f);
		}
	}


	// Ajout du sprite de pause
	spriteManager->AjouterPauseSprite(".\\modeles\\Billboards\\pausemenu.dds"s, largeur / 2, hauteur / 2);
	spriteManager->AjouterPauseSprite(".\\modeles\\Billboards\\transparent.dds"s, largeur / 2, hauteur/2, largeur, hauteur);

	// /UI ingame sprites. SURTOUT NE PAS TOUCHER A L'ORDRE.
	// | keys
	int largeurCle = largeur / 18;
	int hauteurCle = (int)(largeurCle * 1.832f); // Ratio du sprite de la cle pour largeur -> hauteur.
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\key_purple.dds"s, largeurCle, (int)(hauteur / 1.2), largeurCle, hauteurCle, false);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\key_green.dds"s, 2 * largeurCle, (int)(hauteur / 1.2), largeurCle, hauteurCle, false);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\key_blue.dds"s, 3 * largeurCle, (int)(hauteur / 1.2), largeurCle, hauteurCle, false);
	// | Speed-o-meter
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\gauge0.dds"s, largeur - 150, 7 * (hauteur / 8), 0, 0, true);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\gauge1.dds"s, largeur - 150, 7 * (hauteur / 8), 0, 0, false);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\gauge2.dds"s, largeur - 150, 7 * (hauteur / 8), 0, 0, false);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\gauge3.dds"s, largeur - 150, 7 * (hauteur / 8), 0, 0, false);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\gauge4.dds"s, largeur - 150, 7 * (hauteur / 8), 0, 0, false);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\gauge5.dds"s, largeur - 150, 7 * (hauteur / 8), 0, 0, false);
	// | Tomato warning
	int largeurPortal = largeur / 10;
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\tomato_warn.dds"s, largeur - largeurPortal, (int)(hauteur / 1.2) - (largeurPortal + hauteurCle / 2), 200, 200, false);
	// | Avertissement Limite
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\demi_tour.dds"s, largeur / 2, hauteur / 4, (int)((largeur / 4) * 1.16), largeur / 4, false);
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\demi_tour_texte.dds"s, largeur / 2, (hauteur / 4) * 3, largeur - 400, (int)((largeur - 400) / 10.5), false);
	// | Portal status
	spriteManager->AjouterUISprite(".\\modeles\\Billboards\\finalPortalON.dds"s, largeurPortal, (int)(hauteur / 1.2) - (largeurPortal + hauteurCle / 2), (largeurCle * 3), (largeurCle * 3), false);
	// \DONE WITH UI

	// Effet "etoiles"
	for (int i = 0; i < NBETOILES; ++i) {
		physx::PxVec3 off = RandomGenerator::get().randomVec3(-spriteManager->starAreaOffsetFromCenter, spriteManager->starAreaOffsetFromCenter);
		XMFLOAT3 offset = { off.x, off.y, off.z };
		spriteManager->AjouterEtoile(".\\modeles\\Billboards\\star.dds"s, offset, 0.02f, 0.02f);
	}

	// exemple panneau oriente. Params : zone, chemin vers texture, vecteur de position (centre du sprite), scale en x, scale en y (non utilises actuellement).
	// celui-ci reste a sa position attribuee dans le monde et se tourne vers le vaisseau
	//spriteManager->AjouterPanneau(0, ".\\modeles\\Billboards\\testing_tex.dds"s, { 10, 10, 10 }, true, 50.0f, 10.0f);

	// exemple panneau. Params : zone, bool si portail, chemin vers texture, vecteur de position (centre du billboard), bool si billboard oriente, scale en x, scale en y.
	// celui-ci a sa position attribuee dans le monde. Represente le 0,0
	//spriteManager->AjouterPanneau(0, true, ".\\modeles\\Billboards\\testing_tex.dds"s, { 0, 0, 0 }, false, 100.0f, 100.0f);

	// exemple sprite. Params : zone, chemin vers texture, pos en X sur l'ecran, pos en Y sur l'ecran (0,0 en centre du sprite, attention), taille en px de la texture sur l'ecran x, puis y.
	// attention, l'image grandit vers le haut-droite quand on monte les deux derniers params, a partir du point fourni dans les deux precedents.
	//spriteManager->AjouterSprite(0, ".\\modeles\\Billboards\\tomato_warn.dds"s, 350, 450, 200, 200);

	// exemple texte.
	// il faudrait mettre en place des variables dans moteur.h pour cela. Je le ferai une autre fois quand ce sera necessaire. (voir p.282 du poly du prof).
	//const Gdiplus::FontFamily oFamily(L"Comic Sans", nullptr);
	//pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 16.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	//pTexte1 = std::make_unique<PM3D::CAfficheurTexte>(pDispositif, 256, 256, pPolice.get());
	//spriteManager->AjouterSpriteTexte(pTexte1->GetTextureView(), 0, 257);

	/******************HUD************************/
	PM3D::CAfficheurTexte::Init();
	const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
	pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 20.0f, Gdiplus::FontStyleBold,
		Gdiplus::UnitPixel);
	/*************Fin init HUD********************/

	/*******Chargement composants HUD*************/

	//INIT DU CHRONO
	pChronoTexte = std::make_unique<PM3D::CAfficheurTexte>(pDispositif, 140, 100, pPolice.get());
	//couleur du texte, param 1 = alpha et ensuite RGB
	pBrush = std::make_unique<Gdiplus::SolidBrush>(Gdiplus::Color(255, 255, 255, 255));
	// a l'init le chrono démarre a 0
	pChronoTexte->Ecrire(L"00:00"s, pBrush.get());
	spriteManager->AjouterSpriteTexte(pChronoTexte->GetTextureView(), 80, 60);

	// INIT DE LA VITESSE
	pVitesseTexte = std::make_unique<PM3D::CAfficheurTexte>(pDispositif, 140, 100, pPolice.get());
	pVitesseTexte->Ecrire(L"0"s, pBrush.get());
	spriteManager->AjouterSpriteTexte(pVitesseTexte->GetTextureView(), largeur - 170 , hauteur - 15);

	//INIT D'AUTRES ELEMENTS
	pGameOverTexte = std::make_unique<PM3D::CAfficheurTexte>(pDispositif, 140, 100, pPolice.get());
}

SceneManager& SceneManager::get() noexcept {
	static SceneManager instance;
	return instance;
}

SceneManager::SceneManager() {
	for (int i = 0; i < NBZONES; ++i) {
		Scenes.push_back(std::vector<std::unique_ptr<PM3D::CObjet3D>>{});
	}
	for (int i = 0; i < NBZONES; ++i) 
	{
		PickUpObjectsScenes.push_back(std::vector<std::unique_ptr<PickUpObject>>{});
	}
}

void SceneManager::Draw(Zone scene) {
	if (!player->getCam()->isCamera1st())
	{
		player->Draw();
	}
	
	terrain->Draw();
	for (auto& obj : Scenes[static_cast<int>(scene)])
	{
		obj->Draw();
	}
	Monsters[static_cast<int>(scene)]->Draw();

	for (auto& obj : PickUpObjectsScenes[static_cast<int>(scene)]) 
	{
		obj->Draw();
	}

	// Billboards, sprites et panneaux
	spriteManager->DrawZone(static_cast<int>(scene));
	spriteManager->Draw();
}

void SceneManager::Anime(Zone scene, float tmps) {
	player->Anime(tmps);
	terrain->Anime(tmps);
	for (auto& obj : Scenes[static_cast<int>(scene)])
	{
		obj->Anime(tmps);
	}
	Monsters[static_cast<int>(scene)]->Anime(tmps);
  
	for (auto& obj : PickUpObjectsScenes[static_cast<int>(scene)])
	{
		obj->Anime(tmps);
	}

	// Billboards, sprites et panneaux
	{ // Distance monstre-joueur
		spriteManager->Anime(tmps);
		float distance = (player->body->getGlobalPose().p - Monsters[static_cast<int>(scene)]->getPosition().p).magnitude();
		if (distance < 600.0f)
			spriteManager->displayWarning();
		else
			spriteManager->hideWarning();
	}

	{ // Distance joueur-centre zone
		float distance = (player->body->getGlobalPose().p - zonesCenters[static_cast<int>(scene)]).magnitude();
		if (distance > 2000.0f)
		{
			spriteManager->displayOutOfBoundsWarns();
		}
		else
			spriteManager->hideOutOfBoundsWarns();
	}

	spriteManager->AnimeZone(static_cast<int>(scene), tmps);
}

PM3D::CAfficheurTexte* SceneManager::GetpChronoTexte()
{
	return pChronoTexte.get();
}

PM3D::CAfficheurTexte* SceneManager::GetpVitesseTexte()
{
	return pVitesseTexte.get();
}

Gdiplus::SolidBrush* SceneManager::GetpBrush()
{
	return pBrush.get();

}

void SceneManager::displayPause()
{
	pauseStatus = true;
	spriteManager->displayPauseSprite();
}

void SceneManager::hidePause()
{
	pauseStatus = false;
	spriteManager->hidePauseSprite();
}

physx::PxVec3 SceneManager::getPortalPos(Zone current, Zone past) {
	
	const int c = static_cast<int>(current);
	const int p = static_cast<int>(past);
	constexpr float offset(25.0f);

	if(p == NBZONES - 1 && c == 0) return portalPos[c] + offset * player->getDir();
	else if(p == 0 && c == NBZONES - 1) return portalPos[NBPORTAILS - 1] + offset * player->getDir();
	else if ((p % 3) < c && p != NBZONES - 1) return portalPos[2 * c] + offset * player->getDir();
	else return portalPos[p + c] + offset * player->getDir();

}

const float SceneManager::getBoxSize() {
	return BOXSIZE;
}

void SceneManager::activateFinalPortal()
{
	// TODO : activer un collider-portail
	// Ajout du sprite du portail final
	if (!spriteManager->isFinalPortalOn())
	{
		// Initialisation du portail final
		std::unique_ptr<PM3D::CObjet3D> pFinalPortal = std::make_unique<Portal>(".\\modeles\\Portal\\portal.obm"s, PM3D::CMoteurWindows::GetInstance().pDispositif, 20.0f, physx::PxVec3{ finalPortalPos.x, finalPortalPos.y, finalPortalPos.z });
		Scenes[3].push_back(std::move(pFinalPortal));
		spriteManager->displayFinalPortal();
		spriteManager->AjouterPanneau(3, true, ".\\modeles\\Billboards\\finalPortal.dds"s, finalPortalPos, true, 100.0f, 100.0f);
	}
}

void SceneManager::changePauseToGameOver(bool _gameWon, std::wstring _finalTime)
{
	pGameOverTexte->Ecrire(_finalTime, pBrush.get());
	spriteManager->changePauseToGameOver(_gameWon, pGameOverTexte);
}