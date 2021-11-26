#pragma once
#include "Singleton.h"
#include "dispositif.h"

#include "Objet3D.h"
#include "Bloc.h"
#include "BlocEffet1.h"
#include "ObjetMesh.h"
#include "ChargeurOBJ.h"
#include "Terrain.h"
#include "Camera.h"
#include "Player.h"
#include "Asteroid.h"
#include "Planet.h"
#include "GameManager.h"

#include "GestionnaireDeTextures.h"
#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "DIManipulateur.h"

#include "PanneauPE.h"
#include "PhysXManager.h"
#include "SceneManager.h"
#include "RandomGenerator.h"

#include <vector>
#include <string>


namespace PM3D
{

const int IMAGESPARSECONDE = 60;
const double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

//
//   TEMPLATE : CMoteur
//
//   BUT : Template servant à construire un objet Moteur qui implantera les
//         aspects "génériques" du moteur de jeu
//
//   COMMENTAIRES :
//
//        Comme plusieurs de nos objets représenteront des éléments uniques 
//		  du système (ex: le moteur lui-même, le lien vers 
//        le dispositif Direct3D), l'utilisation d'un singleton 
//        nous simplifiera plusieurs aspects.
//
template <class T, class TClasseDispositif> class CMoteur :public CSingleton<T>
{
public:

	virtual void Run()
	{
		bool bBoucle = true;

		while (bBoucle)
		{
			// Propre à la plateforme - (Conditions d'arrêt, interface, messages)
			bBoucle = RunSpecific();

			// appeler la fonction d'animation
			if (bBoucle)
			{
				bBoucle = Animation();
			}
		}
	}

	virtual int Initialisations()
	{
		// Propre à la plateforme
		InitialisationsSpecific();

		// * Initialisation du dispositif de rendu
		pDispositif = CreationDispositifSpecific(CDS_FENETRE);

		// * Initialisation de la scène
		InitScene();

		// * Initialisation des paramètres de l'animation et 
		//   préparation de la première image
		InitAnimation();

		return 0;
	}

	virtual bool Animation()
	{
		// méthode pour lire l'heure et calculer le 
		// temps écoulé
		const int64_t TempsCompteurCourant = GetTimeSpecific();
		const double TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

		// Est-il temps de rendre l'image?
		if (TempsEcoule > EcartTemps)
		{
			// Affichage optimisé
			pDispositif->Present(); // On enlevera «//» plus tard

			// On prépare la prochaine image
			AnimeScene(static_cast<float>(TempsEcoule));

			// On rend l'image sur la surface de travail
			// (tampon d'arrière plan)
			RenderScene();

			// Calcul du temps du prochain affichage
			TempsCompteurPrecedent = TempsCompteurCourant;
		}

		return true;
	}

	const XMMATRIX& GetMatView() const { return m_MatView; }
	const XMMATRIX& GetMatProj() const { return m_MatProj; }
	const XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }

	CCamera& GetFreeCamera() { return freeCam; }

	CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }
	CDIManipulateur& GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }

protected:
	~CMoteur()
	{
		Cleanup();
	}

	// Spécifiques - Doivent être implantés
	virtual bool RunSpecific() = 0;
	virtual int InitialisationsSpecific() = 0;

	virtual int64_t GetTimeSpecific() const = 0;
	virtual double GetTimeIntervalsInSec(int64_t start, int64_t stop) const = 0;

	virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
	virtual void BeginRenderSceneSpecific() = 0;
	virtual void EndRenderSceneSpecific() = 0;

	// Autres fonctions
	virtual int InitAnimation()
	{
		TempsSuivant = GetTimeSpecific();
		TempsCompteurPrecedent = TempsSuivant;

		// première Image
		RenderScene();

		return true;
	}

	// Fonctions de rendu et de présentation de la scène
	virtual bool RenderScene()
	{
		
		BeginRenderSceneSpecific();

		// Appeler les fonctions de dessin de chaque objet de la scène
		for (auto& object3D : ListeScene)
		{
			object3D->Draw();
		}

		EndRenderSceneSpecific();

		return true;
	}

	virtual void Cleanup()
	{
		// détruire les objets
		ListeScene.clear();

		// Détruire le dispositif
		if (pDispositif)
		{
			delete pDispositif;
			pDispositif = nullptr;
		}

		physXManager.cleanupPhysics();
	}

	virtual int InitScene()
	{
		// Initialisation des matrices View et Proj
		m_MatView = XMMatrixLookAtRH(
			XMVectorSet(0.0f, 0.0f, 100.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

		const float champDeVision = XM_PI / 4; 	// 45 degrés
		const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
		const float planRapproche = 1.0;
		const float planEloigne = 4000.0;

		m_MatProj = XMMatrixPerspectiveFovRH(
			champDeVision,
			ratioDAspect,
			planRapproche,
			planEloigne);

		// Calcul de VP à l'avance
		m_MatViewProj = m_MatView * m_MatProj;

		// Initialisation de la camera du joeur
		freeCam.Init(XMVectorSet(0.0f, 0.0f, 100.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), &m_MatView, &m_MatProj, &m_MatViewProj);
		freeCam.angleDirectionCamera = XM_PI / 2.0f;


		// Initialisation des objets 3D - création et/ou chargement
		if (!InitObjets())
		{
			return 1;
		}

		return 0;
	}

	bool InitObjets()
	{
		float boxSize = 3000.0f; //taille de la fausse skybox

		//Creation de la fausse skyBox (cube avec le culling inversé)
		std::unique_ptr<CBlocEffet1> skybox = std::make_unique<CBlocEffet1>(boxSize, boxSize, boxSize, pDispositif);
		//ajoute une texture a la skybox
		skybox->SetTexture(TexturesManager.GetNewTexture(L".\\modeles\\SkyBoxes\\box.dds", pDispositif));
		ListeScene.push_back(std::move(skybox));		//ajoute la skybox a la scene



		


		//Creation de 15 Planetes avec des tailles aleatoires entre 75 et 150
		for (int i = 0; i < 15; i++) {
			float scale = static_cast<float>(RandomGenerator::get().next(75, 150));
			std::unique_ptr<Planet> planet = std::make_unique<Planet>(".\\modeles\\Planete\\3\\Planete.obm", pDispositif, sceneManager.planetePos[i], scale);
			ListeScene.push_back(std::move(planet));
		}

		//Creation du player, constructeur avec format binaire
		player = std::make_unique<Player>(".\\modeles\\Player\\Soucoupe1\\UFO1.obm", pDispositif, 2.0f);
		player->setCam(&freeCam);						//lie la camera au player
		ListeScene.push_back(std::move(player));		//ajoute le player a la scene


		//Creation de 4 Asteroides avec des tailles aleatoires entre 5 et 20
		//La position des asteroides est une position aleatoire entre -1000 et -500 dans les 3 axes (posibilité de collision entre les asteroides a la creation)
		for (int i = 0; i < 4; i++) {
			float scale = static_cast<float>(RandomGenerator::get().next(5, 20));
			PxVec3 pos = RandomGenerator::get().randomVec3(-1000, -500);
			std::unique_ptr<Asteroid> asteroid = std::make_unique<Asteroid>(".\\modeles\\Asteroide\\1\\asteroide.obm", pDispositif, pos, boxSize, scale);
			ListeScene.push_back(std::move(asteroid));
		}

		return true;
	}

	bool AnimeScene(float tempsEcoule)
	{
		// Prendre en note le statut du clavier
		GestionnaireDeSaisie.StatutClavier();
		// Prendre en note l'état de la souris
		GestionnaireDeSaisie.SaisirEtatSouris();
		

		if ((GestionnaireDeSaisie.ToucheAppuyee(DIK_ESCAPE)) && manager.hasBeenEnoughTimeSinceLastPause())
		{
			if (manager.getIsPauseStatus())
			{
				manager.setPauseMenu(false);
			}
			else {
				manager.setPauseMenu(true);
			}
		}


		//si on est sur le menu pause
		if (!manager.getIsPauseStatus()) {

			physXManager.stepPhysics();
			for (auto& object3D : ListeScene)
			{
				object3D->Anime(tempsEcoule);
			}
		}
		

		return true;
	}

public:
	// Le dispositif de rendu
	TClasseDispositif* pDispositif;


protected:

	
	// La seule scène
	std::vector<std::unique_ptr<CObjet3D>> ListeScene;

	// Variables pour le temps de l'animation
	int64_t TempsSuivant;
	int64_t TempsCompteurPrecedent;


	CChargeurOBJ chargeur;
	CParametresChargement params;

	SceneManager sceneManager;

	// Les matrices
	XMMATRIX m_MatView;
	XMMATRIX m_MatProj;
	XMMATRIX m_MatViewProj;

	// Le gestionnaire de texture
	CGestionnaireDeTextures TexturesManager;

	// Pour le texte
	std::unique_ptr<CAfficheurTexte> pTexte1;
	std::wstring str;

	std::unique_ptr<Gdiplus::Font> pPolice;
	std::unique_ptr<CPanneauPE> pPanneauPE;


	//gestionnaire de saisie
	CDIManipulateur GestionnaireDeSaisie;

	//game manager
	GameManager& manager = GameManager::get();

	
	//PhysX manager
	PhysXManager& physXManager = PhysXManager::get();

	//camera joueur
	CCamera freeCam;


	//objet joueur
	std::unique_ptr<Player> player;


};

} // namespace PM3D

