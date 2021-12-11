#include "stdafx.h"
#include "SceneManager.h"
#include "Terrain.h"
#include "LectureFichier.h"
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

//template<class T, class ... Ts>
//void load(std::vector<std::unique_ptr<PM3D::CObjet3D>>* ListeScene,
//		  Ts ... args) 
//{
//	std::unique_ptr<T> obj = std::make_unique<T>(&args...);
//	std::lock_guard<std::mutex> lock(objMutex);
//	if (ListeScene) ListeScene->push_back(std::move(obj));
//}

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
		//Creation de la fausse skyBox (cube avec le culling inversé)
		std::unique_ptr<PM3D::CBlocEffet1> skybox = std::make_unique<PM3D::CBlocEffet1>(BOXSIZE, BOXSIZE, BOXSIZE, pDispositif, i);
		//ajoute une texture a la skybox
		skybox->SetTexture(TexturesManager.GetNewTexture(L".\\modeles\\SkyBoxes\\box.dds", pDispositif));
		Scenes[i].push_back(std::move(skybox));
	}

	player = std::make_unique<Player>(".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f));
	player->setCam(&camera);
	//ajoute la skybox a la scene

	//Creation de 15 Planetes avec des tailles aleatoires entre 75 et 150

	for (const auto& pos : planetePos1) {
		float scale = static_cast<float>(RandomGenerator::get().next(75, 150));
		futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\3\\Planete.obm"s, pDispositif, scale, pos, 0, [](Planet*) noexcept {}));
	}

	for (const auto& pos : planetePos2) {
		float scale = static_cast<float>(RandomGenerator::get().next(75, 150));
		futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\3\\Planete.obm"s, pDispositif, scale, pos, 1, [](Planet*) noexcept {}));
	}

	//Creation de 4 Asteroides avec des tailles aleatoires entre 5 et 20
	//La position des asteroides est une position aleatoire entre -1000 et -500 dans les 3 axes (posibilité de collision entre les asteroides a la creation)
	for (int i = 0; i < NBASTEROIDES; ++i) {
		float scale = static_cast<float>(RandomGenerator::get().next(5, 20));
		PxVec3 pos = RandomGenerator::get().randomVec3(-1000, -500);
		futures.push_back(std::async(load<Asteroid>, &Scenes, ".\\modeles\\Asteroide\\1\\asteroide.obm"s, pDispositif, scale, pos, 0, [](Asteroid*) noexcept {}));
	}

	for (int i = 0; i < NBPORTAILS; ++i) {
		futures.push_back(std::async(load<Portal>, &Scenes, ".\\modeles\\Planete\\5\\Planete.obm"s, pDispositif, 20.0f, portalPos[i], i, [](Portal*) noexcept {}));
	}

	////Creation du player, constructeur avec format binaire
	//futures.push_back(std::async(load<Player>, &Scenes, ".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f), 0, f));
	
	LectureFichier lecteurHeightmap{ "smolOBJECT" };
	terrain = std::make_unique<PM3D::CTerrain>(pDispositif, lecteurHeightmap, physx::PxVec3(0.f),0);
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\grass.dds", pDispositif));
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\water.dds", pDispositif));
	terrain->AddTexture(TexturesManager.GetNewTexture(L".\\modeles\\Terrain\\filtre.dds", pDispositif));
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
}

void SceneManager::Anime(Zone scene, float tmps) {
	player->Anime(tmps);
	terrain->Anime(tmps);
	for (auto& obj : Scenes[static_cast<int>(scene)])
	{
		obj->Anime(tmps);
	}
}

