#include "stdafx.h"
#include "SceneManager.h"
#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
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
	std::unique_ptr<T> obj = std::make_unique<T>(nomfichier, _pDispositif, scale, pos);
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

	//Creation de la fausse skyBox (cube avec le culling inversé)
	std::unique_ptr<PM3D::CBlocEffet1> skybox = std::make_unique<PM3D::CBlocEffet1>(boxSize, boxSize, boxSize, pDispositif);
	//ajoute une texture a la skybox
	skybox->SetTexture(TexturesManager.GetNewTexture(L".\\modeles\\SkyBoxes\\box.dds", pDispositif));
	Scenes[0].push_back(std::move(skybox));		//ajoute la skybox a la scene

	auto f = [&](Player* p) { p->setCam(&camera); };
	//Creation du player, constructeur avec format binaire
	futures.push_back(std::async(load<Player>, &Scenes, ".\\modeles\\Player\\Soucoupe1\\UFO1.obm"s, pDispositif, 2.0f, physx::PxVec3(0.0f), 0, f));

	//Creation de 15 Planetes avec des tailles aleatoires entre 75 et 150

	for (const auto& pos : planetePos) {
		float scale = static_cast<float>(RandomGenerator::get().next(75, 150));
		futures.push_back(std::async(load<Planet>, &Scenes, ".\\modeles\\Planete\\3\\Planete.obm"s, pDispositif, scale, pos, 0, [](Planet*) noexcept {}));
	}

	//Creation de 4 Asteroides avec des tailles aleatoires entre 5 et 20
	//La position des asteroides est une position aleatoire entre -1000 et -500 dans les 3 axes (posibilité de collision entre les asteroides a la creation)
	for (int i = 0; i < 4; ++i) {
		float scale = static_cast<float>(RandomGenerator::get().next(5, 20));
		PxVec3 pos = RandomGenerator::get().randomVec3(-1000, -500);
		futures.push_back(std::async(load<Asteroid>, &Scenes, ".\\modeles\\Asteroide\\1\\asteroide.obm"s, pDispositif, scale, pos, 0, [](Asteroid*) noexcept {}));
	}

	// Creation du gestionnaire de billboards, sprites et texte
	
	this->spriteManager = std::make_unique<PM3D::CAfficheurSprite>(pDispositif);
	// exemple panneau. Params : chemin vers texture, vecteur de position, scale en x, scale en y.
	spriteManager->AjouterPanneau(".\\modeles\\Billboards\\testing_tex.dds"s, { 0, 0, 0 }, 10.0f, 10.0f);

	// exemple sprite. Params : chemin vers texture, pos en X sur l'ecran, pos en Y sur l'ecran (0,0 en haut a gauche, attention), taille en px de la texture sur l'ecran x, puis y.
	// attention, l'image grandit vers le haut-droite quand on monte les deux derniers params, a partir du point fourni dans les deux precedents.
	spriteManager->AjouterSprite(".\\modeles\\Billboards\\testing_tex.dds"s, 10, 110, 100, 100);

	// exemple texte.
	// il faudrait mettre en place des variables dans moteur.h pour cela. Je le ferai une autre fois quand ce sera necessaire. (voir p.282 du poly du prof).
	//const Gdiplus::FontFamily oFamily(L"Comic Sans", nullptr);
	//pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 16.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	//pTexte1 = std::make_unique<PM3D::CAfficheurTexte>(pDispositif, 256, 256, pPolice.get());
	//spriteManager->AjouterSpriteTexte(pTexte1->GetTextureView(), 0, 257);
}

SceneManager& SceneManager::get() noexcept {
	static SceneManager instance;
	return instance;
}

SceneManager::SceneManager() {
	for (int i = 0; i < 4; ++i) {
		Scenes.push_back(std::vector<std::unique_ptr<PM3D::CObjet3D>>{});
	}
}

void SceneManager::Draw(int scene) {
	for (auto& obj : Scenes[scene])
	{
		obj->Draw();
	}
}

