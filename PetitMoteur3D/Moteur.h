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
#include "Obstacle.h"

#include "GestionnaireDeTextures.h"
#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "DIManipulateur.h"

#include "PanneauPE.h"
#include "PhysXManager.h"


namespace PM3D
{

const int IMAGESPARSECONDE = 60;
const double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

//
//   TEMPLATE�: CMoteur
//
//   BUT�: Template servant � construire un objet Moteur qui implantera les
//         aspects "g�n�riques" du moteur de jeu
//
//   COMMENTAIRES�:
//
//        Comme plusieurs de nos objets repr�senteront des �l�ments uniques 
//		  du syst�me (ex: le moteur lui-m�me, le lien vers 
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
			// Propre � la plateforme - (Conditions d'arr�t, interface, messages)
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
		// Propre � la plateforme
		InitialisationsSpecific();

		// * Initialisation du dispositif de rendu
		pDispositif = CreationDispositifSpecific(CDS_FENETRE);

		// * Initialisation de la sc�ne
		InitScene();

		// * Initialisation des param�tres de l'animation et 
		//   pr�paration de la premi�re image
		InitAnimation();

		return 0;
	}

	virtual bool Animation()
	{
		// m�thode pour lire l'heure et calculer le 
		// temps �coul�
		const int64_t TempsCompteurCourant = GetTimeSpecific();
		const double TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

		// Est-il temps de rendre l'image?
		if (TempsEcoule > EcartTemps)
		{
			// Affichage optimis�
			pDispositif->Present(); // On enlevera �//� plus tard

			// On pr�pare la prochaine image
			AnimeScene(static_cast<float>(TempsEcoule));

			// On rend l'image sur la surface de travail
			// (tampon d'arri�re plan)
			RenderScene();

			// Calcul du temps du prochain affichage
			TempsCompteurPrecedent = TempsCompteurCourant;
		}

		return true;
	}

	const XMMATRIX& GetMatView() const { return m_MatView; }
	const XMMATRIX& GetMatProj() const { return m_MatProj; }
	const XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }

	CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }
	CDIManipulateur& GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }

protected:
	~CMoteur()
	{
		Cleanup();
	}

	// Sp�cifiques - Doivent �tre implant�s
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

		// premi�re Image
		RenderScene();

		return true;
	}

	// Fonctions de rendu et de pr�sentation de la sc�ne
	virtual bool RenderScene()
	{
		BeginRenderSceneSpecific();

		//pPanneauPE->DebutPostEffect();

		BeginRenderSceneSpecific();

		// Appeler les fonctions de dessin de chaque objet de la sc�ne
		for (auto& object3D : ListeScene)
		{
			object3D->Draw();
		}

		EndRenderSceneSpecific();

		//pPanneauPE->FinPostEffect();

		//pPanneauPE->Draw();

		EndRenderSceneSpecific();

		return true;
	}

	virtual void Cleanup()
	{
		// d�truire les objets
		ListeScene.clear();

		// D�truire le dispositif
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
		// Dans notre cas, ces matrices sont fixes
		m_MatView = XMMatrixLookAtRH(
			XMVectorSet(0.0f, 0.0f, 10.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

		const float champDeVision = XM_PI / 4; 	// 45 degr�s
		const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
		const float planRapproche = 1.0;
		const float planEloigne = 3000.0;

		m_MatProj = XMMatrixPerspectiveFovRH(
			champDeVision,
			ratioDAspect,
			planRapproche,
			planEloigne);

		// Calcul de VP � l'avance
		m_MatViewProj = m_MatView * m_MatProj;

		freeCam.Init(XMVectorSet(0.0f, 0.0f, 10.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), &m_MatView, &m_MatProj, &m_MatViewProj);
		freeCam.angleDirectionCamera = XM_PI / 2.0f;


		// Initialisation des objets 3D - cr�ation et/ou chargement
		if (!InitObjets())
		{
			return 1;
		}

		return 0;
	}

	bool InitObjets()
	{

		/*char* filename = new char[50]("Planet.obj");
		std::unique_ptr<Obstacle> obs = std::make_unique<Obstacle>(filename, XMFLOAT3(10.0f, 10.0f, 10.0f), pDispositif);
		obs->SetTexture(TexturesManager.GetNewTexture(L"roche2.dds", pDispositif));
		ListeScene.emplace_back(std::move(obs));*/
		/*params.NomChemin = "";
		params.NomFichier = "Planet.obj";
		chargeur.Chargement(params);*/
		 //Constructeur avec format binaire
		//std::unique_ptr<CObjetMesh> pMesh =
		//	std::make_unique<CObjetMesh>("Planet.OMB", pDispositif);
		//// Puis, il est ajout� � la sc�ne
		//ListeScene.push_back(std::move(pMesh));

		for (int i = 0; i < 10; i++) {
			std::unique_ptr<CBlocEffet1> pBloc = std::make_unique<CBlocEffet1>(2.0f, 2.0f, 2.0f, pDispositif);


			// Lui assigner une texture
			pBloc->SetTexture(TexturesManager.GetNewTexture(L"roche2.dds", pDispositif));


			ListeScene.push_back(std::move(pBloc));
		}

		//return true;
		 //Constructeur avec format binaire

		std::unique_ptr<CObjetMesh> pMesh =
			std::make_unique<CObjetMesh>("Planet.OMB", pDispositif);
		// Puis, il est ajout� � la sc�ne
		ListeScene.push_back(std::move(pMesh));
		std::unique_ptr<CObjetMesh> pMesh2 = std::make_unique<CObjetMesh>(".\\modeles\\jin\\jin.OMB", pDispositif);
		 //Puis, il est ajout� � la sc�ne
		ListeScene.push_back(std::move(pMesh2));

		// //Cr�ation de l'afficheur de sprites et ajout des sprites
		//std::unique_ptr<CAfficheurSprite> pAfficheurSprite = std::make_unique<CAfficheurSprite>(pDispositif);

		//// ajout de panneaux 
		//pAfficheurSprite->AjouterPanneau("grass_v1_basic_tex.dds",
		//	XMFLOAT3(1.0f, 0.0f, 1.0f));
		//pAfficheurSprite->AjouterPanneau("grass_v1_basic_tex.dds",
		//	XMFLOAT3(0.0f, 0.0f, -1.0f));
		//pAfficheurSprite->AjouterPanneau("grass_v1_basic_tex.dds",
		//	XMFLOAT3(-1.0f, 0.0f, 0.5f));
		//pAfficheurSprite->AjouterPanneau("grass_v1_basic_tex.dds",
		//	XMFLOAT3(-0.5f, 0.0f, 1.0f));
		//pAfficheurSprite->AjouterPanneau("grass_v1_basic_tex.dds",
		//	XMFLOAT3(-2.0f, 0.0f, 2.0f));

		//pAfficheurSprite->AjouterSprite("tree02s.dds", 200,400);
		//pAfficheurSprite->AjouterSprite("tree02s.dds", 500,500, 100, 100);
		//pAfficheurSprite->AjouterSprite("tree02s.dds", 800,200, 100, 100);

		//CAfficheurTexte::Init();
		//const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
		//pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 16.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		//pTexte1 = std::make_unique<CAfficheurTexte>(pDispositif, 256, 256, pPolice.get());

		//pAfficheurSprite->AjouterSpriteTexte(pTexte1->GetTextureView(), 0, 257);

		//pTexte1->Ecrire(L"Test du texte");

		// //Puis, il est ajout� � la sc�ne
		//ListeScene.push_back(std::move(pAfficheurSprite));

		//pPanneauPE = std::make_unique<CPanneauPE>(pDispositif);

		return true;
	}

	bool AnimeScene(float tempsEcoule)
	{

		physXManager.stepPhysics();
		// Prendre en note le statut du clavier
		GestionnaireDeSaisie.StatutClavier();

		// Prendre en note l'�tat de la souris
		GestionnaireDeSaisie.SaisirEtatSouris();

		freeCam.UpdateFree(tempsEcoule);

		for (auto& object3D : ListeScene)
		{
			object3D->Anime(tempsEcoule);
		}

		return true;
	}

protected:
	// Variables pour le temps de l'animation
	int64_t TempsSuivant;
	int64_t TempsCompteurPrecedent;

	// Le dispositif de rendu
	TClasseDispositif* pDispositif;

	CChargeurOBJ chargeur;
	CParametresChargement params;

	// La seule sc�ne
	std::vector<std::unique_ptr<CObjet3D>> ListeScene;

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

	CDIManipulateur GestionnaireDeSaisie;

	std::unique_ptr<CPanneauPE> pPanneauPE;

	PhysXManager& physXManager = PhysXManager::get();

	CCamera freeCam;
};

} // namespace PM3D

