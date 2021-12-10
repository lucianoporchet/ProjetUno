#include "stdafx.h"
#include "SceneManager.h"
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
		//Creation de la fausse skyBox (cube avec le culling inversé)
		std::unique_ptr<PM3D::CBlocEffet1> skybox = std::make_unique<PM3D::CBlocEffet1>(BOXSIZE, BOXSIZE, BOXSIZE, pDispositif, i);
		//ajoute une texture a la skybox
		skybox->SetTexture(TexturesManager.GetNewTexture(L".\\modeles\\SkyBoxes\\box.dds", pDispositif));
		Scenes[i].push_back(std::move(skybox));
	}


	player = std::make_unique<Player>(".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f));
	player->setCam(&camera);

	futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\4\\Planete.obm"s, pDispositif, 150.0f, PxVec3(0,BOXSIZE,0), 1, [](Planet*) noexcept {}));
	futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\5\\Planete.obm"s, pDispositif, 150.0f, PxVec3(BOXSIZE, BOXSIZE, 0), 2, [](Planet*) noexcept {}));
	futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\6\\Planete.obm"s, pDispositif, 150.0f, PxVec3(BOXSIZE, 0, 0), 3, [](Planet*) noexcept {}));
	//ajoute la skybox a la scene

	//Creation de 15 Planetes avec des tailles aleatoires entre 75 et 150

	for (const auto& pos : planetePos1) {
		float scale = static_cast<float>(RandomGenerator::get().next(75, 150));
		futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\3\\Planete.obm"s, pDispositif, scale, pos, 0, [](Planet*) noexcept {}));
	}


	//Creation de 4 Asteroides avec des tailles aleatoires entre 5 et 20
	//La position des asteroides est une position aleatoire entre -1000 et -500 dans les 3 axes (posibilité de collision entre les asteroides a la creation)
	for (int i = 0; i < NBASTEROIDES; ++i) {
		float scale = static_cast<float>(RandomGenerator::get().next(5, 20));
		PxVec3 pos = RandomGenerator::get().randomVec3(-1000, -500);
		futures.push_back(std::async(load<Asteroid>, &Scenes, ".\\modeles\\Asteroide\\1\\asteroide.obm"s, pDispositif, scale, pos, 0, [](Asteroid*) noexcept {}));
	}

	for (int i = 0; i < NBPORTAILS - 1 ; i+=2) {
		futures.push_back(std::async(load<Portal>, &Scenes, ".\\modeles\\Planete\\2\\Planete.obm"s, pDispositif, 20.0f, portalPos[i], i/2, [&](Portal*) noexcept {}));
		futures.push_back(std::async(load<Portal>, &Scenes, ".\\modeles\\Planete\\2\\Planete.obm"s, pDispositif, 20.0f, portalPos[i+1], i/2, [&](Portal*) noexcept {}));
	}

	////Creation du player, constructeur avec format binaire
	//futures.push_back(std::async(load<Player>, &Scenes, ".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f), 0, f));

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
	for (auto& obj : Scenes[static_cast<int>(scene)])
	{
		obj->Draw();
	}
}

void SceneManager::Anime(Zone scene, float tmps) {
	player->Anime(tmps);
	for (auto& obj : Scenes[static_cast<int>(scene)])
	{
		obj->Anime(tmps);
	}
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

