#pragma once

#include "chargeur.h"

using namespace DirectX;

namespace PM3D
{

	class CChargeurOBJ : public IChargeur
	{
	private:
		class OBJVertexInfo
		{
		public:
			OBJVertexInfo()
				: PositionIndex()
				, TextCoordIndex()
				, NormalIndex()
			{ }

			int PositionIndex;
			int TextCoordIndex;
			int NormalIndex;
		};

		class OBJMaterial
		{
		public:
			std::string NomFichierTexture;
			std::string NomMateriau;

			XMFLOAT4 Ambient;
			XMFLOAT4 Diffuse;
			XMFLOAT4 Specular;
			float Puissance;
			bool Transparent;

			OBJMaterial()
				: Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
				, Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
				, Specular(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
				, Puissance(1.0f)
				, Transparent(false)
			{
			}
		};

	public:

		void Chargement(const CParametresChargement& param) override;

		size_t GetNombreSommets() const override { return tabVertexInfo.size(); }
		size_t GetNombreIndex() const override { return tabIndex.size(); }
		const void* GetIndexData() const override { return tabIndex.data(); }
		int GetNombreSubset() const override { return NombreSubset; }
		size_t GetNombreMaterial() const override { return material.size(); }
		void GetMaterial(int _i,
			std::string& _NomFichierTexture,
			std::string& _NomMateriau,
			XMFLOAT4& _Ambient,
			XMFLOAT4& _Diffuse,
			XMFLOAT4& _Specular,
			float& _Puissance) const override;

		const std::string& GetMaterialName(int i) const override { return materialName[i]; };

		void CopieSubsetIndex(std::vector<int>& dest) const override { dest = SubsetIndex; }

		const XMFLOAT3& GetPosition(int NoSommet) const override { return Position[tabVertexInfo[NoSommet].PositionIndex]; }
		const XMFLOAT2& GetCoordTex(int NoSommet) const override { return CoordTex[tabVertexInfo[NoSommet].TextCoordIndex]; }
		const XMFLOAT3& GetNormale(int NoSommet) const override { return Normale[tabVertexInfo[NoSommet].NormalIndex]; }

	private:
		// FONCTIONS
		bool LireFichier(const std::string& nomFichier);
		bool LireFichierMateriel();
		void TraiterLigneOBJ(const std::string& str);
		void TraiterLigneMAT(const std::string& str);
		int TrouverVertex(const OBJVertexInfo& vertex);

		std::string cheminModele;

		// *** Les sommets 
		std::vector<XMFLOAT3> Position;				// Tableau des positions ( v )
		std::vector<XMFLOAT2> CoordTex;				// Tableau des coordonnées de texture ( vt )
		std::vector<XMFLOAT3> Normale;				// Tableau des normales ( vn )

		std::vector<OBJVertexInfo> tabVertexInfo;	// Sommets par index  no/no/no
		std::vector<int> tabIndex;					// Index pour les polygones (triangles)

		// *** Les sous-objets
		std::vector<int> SubsetIndex;			// index
		int NombreSubset;					// nombre de sous objets

		// *** Indicateurs
		bool bCoordTex;   // Il y a des coordonnées de texture
		bool bNormales;	  // Il y a des normales
		bool bInverserCulling;
		bool bMainGauche;

		// *** Materiaux - Dans le fichier OBJ
		std::string OBJMaterialLib;			// Nom de la bibliotheque materiaux
		int NombreMateriaux;			// Nombre de materiaux
		std::vector<std::string> materialName;	// Nom des materiaux

		// *** Materiaux - Dans le fichier MTL
		std::vector<OBJMaterial> material;
	};

} // namespace PM3D
