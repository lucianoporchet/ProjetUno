#include "stdafx.h"
#include "SceneManager.h"
#include "Terrain.h"
#include "LectureFichier.h"
#include "AfficheurSprite.h"

#include <functional>

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


std::vector<std::unique_ptr<PM3D::CObjet3D>>& SceneManager::getListScene(int scene)
{
	return Scenes[scene];
}

std::vector<std::vector<std::unique_ptr<PM3D::CObjet3D>>>& SceneManager::getScenes() noexcept
{
	return Scenes;
}


void SceneManager::InitObjects(PM3D::CDispositifD3D11* pDispositif, PM3D::CGestionnaireDeTextures& TexturesManager, PM3D::CCamera& camera) {

	std::vector<std::future<void>> futures;
	auto f = [&](Player* p) { p->setCam(&camera); };

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

	for (const auto& pos : planetePos1) {
		float scale = static_cast<float>(RandomGenerator::get().next(75, 150));
		futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\3\\Planete.obm"s, pDispositif, scale, pos, 0, [](Planet*) noexcept {}));
	}


	//Creation de 4 Asteroides avec des tailles aleatoires entre 5 et 20
	//La position des asteroides est une position aleatoire entre -1000 et -500 dans les 3 axes (posibilité de collision entre les asteroides a la creation)
	float scale;
	PxVec3 pos;
	for (int j = 0; j < NBZONES; ++j) {
		for (int i = 0; i < NBASTEROIDES; ++i) {
			
			scale = static_cast<float>(RandomGenerator::get().next(5, 20));
			pos = RandomGenerator::get().randomPosInZone(j);
			futures.push_back(std::async(load<Asteroid>, &Scenes, ".\\modeles\\Asteroide\\1\\asteroide.obm"s, pDispositif, scale, pos, j, [](Asteroid*) noexcept {}));
		}
	}

	for (int i = 0; i < NBPORTAILS - 1 ; i+=2) {
		futures.push_back(std::async(load<Portal>, &Scenes, ".\\modeles\\Planete\\2\\Planete.obm"s, pDispositif, 20.0f, portalPos[i], i/2, [&](Portal*) noexcept {}));
		futures.push_back(std::async(load<Portal>, &Scenes, ".\\modeles\\Planete\\2\\Planete.obm"s, pDispositif, 20.0f, portalPos[i+1], i/2, [&](Portal*) noexcept {}));
	}

	for (int i = 0; i < NBMONSTRES; ++i) {
		float scale = static_cast<float>(RandomGenerator::get().next(50, 200));
		futures.push_back(std::async(load<Monster>, &Scenes, ".\\modeles\\Monstre\\monstre.obm"s, pDispositif, scale, monsterPos[i], i%NBZONES, [](Monster*) noexcept {}));
	}

	////Creation du player, constructeur avec format binaire
	//futures.push_back(std::async(load<Player>, &Scenes, ".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f), 0, f));
	
	LectureFichier lecteurHeightmap{ "smolOBJECT" };
	terrain = std::make_unique<PM3D::CTerrain>(pDispositif, lecteurHeightmap, physx::PxVec3(0.f),0, 1.0f);
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\grass.dds", pDispositif));
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\water.dds", pDispositif));
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\filtre.dds", pDispositif));

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

	// Ajout du sprite de pause
	spriteManager->AjouterPauseSprite(".\\modeles\\Billboards\\testing_tex.dds"s, 350, 450, 1000, 1000);

	for (int i = 0; i < NBETOILES; ++i) {
		XMFLOAT3 offset = { (float)RandomGenerator::get().next(-spriteManager->starAreaOffsetFromCenter, spriteManager->starAreaOffsetFromCenter),
			(float)RandomGenerator::get().next(-spriteManager->starAreaOffsetFromCenter, spriteManager->starAreaOffsetFromCenter),
			(float)RandomGenerator::get().next(-spriteManager->starAreaOffsetFromCenter, spriteManager->starAreaOffsetFromCenter) };
		spriteManager->AjouterEtoile(".\\modeles\\Billboards\\star.dds"s, offset, 0.02f, 0.02f);
		//spriteManager->AjouterEtoile(".\\modeles\\Billboards\\star.dds"s, offset, 1.0f, 1.0f);
	}

	// exemple panneau oriente. Params : zone, chemin vers texture, vecteur de position (centre du sprite), scale en x, scale en y (non utilises actuellement).
	// celui-ci reste a sa position attribuee dans le monde et se tourne vers le vaisseau
	//spriteManager->AjouterPanneau(0, ".\\modeles\\Billboards\\testing_tex.dds"s, { 10, 10, 10 }, true, 50.0f, 10.0f);

	// exemple panneau. Params : zone, bool si portail, chemin vers texture, vecteur de position (centre du billboard), bool si billboard oriente, scale en x, scale en y.
	// celui-ci a sa position attribuee dans le monde. Represente le 0,0
	spriteManager->AjouterPanneau(0, true, ".\\modeles\\Billboards\\testing_tex.dds"s, { 0, 0, 0 }, false, 100.0f, 100.0f);

	// exemple sprite. Params : zone, chemin vers texture, pos en X sur l'ecran, pos en Y sur l'ecran (0,0 en centre du sprite, attention), taille en px de la texture sur l'ecran x, puis y.
	// attention, l'image grandit vers le haut-droite quand on monte les deux derniers params, a partir du point fourni dans les deux precedents.
	spriteManager->AjouterSprite(0, ".\\modeles\\Billboards\\tomato_warn.dds"s, 350, 450, 200, 200);

	// exemple texte.
	// il faudrait mettre en place des variables dans moteur.h pour cela. Je le ferai une autre fois quand ce sera necessaire. (voir p.282 du poly du prof).
	//const Gdiplus::FontFamily oFamily(L"Comic Sans", nullptr);
	//pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 16.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	//pTexte1 = std::make_unique<PM3D::CAfficheurTexte>(pDispositif, 256, 256, pPolice.get());
	//spriteManager->AjouterSpriteTexte(pTexte1->GetTextureView(), 0, 257);

	/******************HUD************************/
	PM3D::CAfficheurTexte::Init();
	const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
	pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 16.0f, Gdiplus::FontStyleBold,
		Gdiplus::UnitPixel);
	/*************Fin init HUD********************/

	/*******Chargement composants HUD*************/

	//INIT DU CHRONO
	pChronoTexte = std::make_unique<PM3D::CAfficheurTexte>(pDispositif, 140, 100, pPolice.get());
	//couleur du texte, param 1 = alpha et ensuite RGB
	pBrush = std::make_unique<Gdiplus::SolidBrush>(Gdiplus::Color(255, 255, 255, 255));
	// a l'init le chrono démarre a 0
	pChronoTexte->Ecrire(L"0h0m0s 0"s, pBrush.get());
	spriteManager->AjouterSpriteTexte(0, pChronoTexte->GetTextureView(), 70, 50);

	//INIT D'AUTRES ELEMENTS


	

}

SceneManager& SceneManager::get() noexcept {
	static SceneManager instance;
	return instance;
}

SceneManager::SceneManager() {
	for (int i = 0; i < NBZONES; ++i) {
		Scenes.push_back(std::vector<std::unique_ptr<PM3D::CObjet3D>>{});
	}
}

void SceneManager::Draw(Zone scene) {
	player->Draw();
	terrain->Draw();
	for (auto& obj : Scenes[static_cast<int>(scene)])
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
	// Billboards, sprites et panneaux
	spriteManager->Anime(tmps);
	spriteManager->AnimeZone(static_cast<int>(scene), tmps);
}

PM3D::CAfficheurTexte* SceneManager::GetpChronoTexte()
{
	return pChronoTexte.get();
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

