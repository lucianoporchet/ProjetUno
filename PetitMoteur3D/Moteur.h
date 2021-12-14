#pragma once
#include "Singleton.h"
#include "dispositif.h"


#include "GameManager.h"

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "DIManipulateur.h"

#include "PanneauPE.h"
#include "PhysXManager.h"
#include "SceneManager.h"

#include <chrono>
#include <vector>
#include <string>
#include <fstream>

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
		manager.getSceneManager().Draw(manager.getActiveZone());

		EndRenderSceneSpecific();

		return true;
	}

	virtual void Cleanup()
	{
		
		// détruire les objets
		manager.cleanManager();
		TexturesManager.ListeTextures.clear();
		
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
		const float planEloigne = 18000.0;

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

		//association du gestionnaire de saisie au game manager
		manager.setGestionnaireDeSaisie(GetGestionnaireDeSaisie());

		// Initialisation des objets 3D - création et/ou chargement
		if (!InitObjets())
		{
			return 1;
		}

		return 0;
	}

	bool InitObjets()
	{
		std::chrono::nanoseconds result1;
		auto t = std::chrono::high_resolution_clock::now();
		manager.getSceneManager().InitObjects(pDispositif, TexturesManager, freeCam);
		result1 = std::chrono::high_resolution_clock::now() - t;
		
		std::ofstream log{ "Log.txt" };
		log << std::chrono::duration_cast<std::chrono::milliseconds>(result1).count();

		return true;
	}

	bool AnimeScene(float tempsEcoule)
	{
		manager.AnimeScene(tempsEcoule);

		return true;
	}

public:
	// Le dispositif de rendu
	TClasseDispositif* pDispositif;


protected:

	
	// La seule scène
	//std::vector<std::unique_ptr<CObjet3D>> ListeScene;

	// Variables pour le temps de l'animation
	int64_t TempsSuivant;
	int64_t TempsCompteurPrecedent;


	CChargeurOBJ chargeur;
	CParametresChargement params;

	// Les matrices
	XMMATRIX m_MatView;
	XMMATRIX m_MatProj;
	XMMATRIX m_MatViewProj;

	// Le gestionnaire de texture
	CGestionnaireDeTextures TexturesManager;

	//gestionnaire de saisie
	CDIManipulateur GestionnaireDeSaisie;

	//game manager
	GameManager& manager = GameManager::get();
	
	//PhysX manager
	PhysXManager& physXManager = PhysXManager::get();

	//camera joueur
	CCamera freeCam;
};

} // namespace PM3D

