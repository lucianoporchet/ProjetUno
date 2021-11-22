#pragma once
#include "objet3d.h"
#include "sommetbloc.h"
#include "d3dx11effect.h"
#include "PhysXManager.h"
#include "RandomGenerator.h"
#include "ChargeurOBJ.h"
#include "Texture.h"
#include "LectureFichier.h"
#include <string>

namespace PM3D 
{
	class CDispositifD3D11;

	class Obstacle : public CObjet3D
	{
	public:
		Obstacle(std::string filename, XMFLOAT3 scale, CDispositifD3D11* pDispositif);

		virtual ~Obstacle();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;

		float getHeight(float x, float z);
		int height;
		int width;

		void SetTexture(CTexture* pTexture1);

		XMFLOAT3 scale;

		PxRigidDynamic* body; 

	private:

		CParametresChargement params;
		CChargeurOBJ chargeur;
		//LectureFichier lecteur;

		std::vector<CSommetBloc> sommets;
		//CSommetBloc* sommets;

		std::string filename;
		CDispositifD3D11* pDispositif;
		void InitShaders();
		void InitEffect();

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3D11InputLayout* pVertexLayout;
		
		unsigned int* pIndices;
		int nbSommets;
		int nbPolygones;

		// Définitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		float rotation;

		ID3D11ShaderResourceView* pTextureD3D1;
		ID3D11SamplerState* pSampleState;

		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;

	};

}

