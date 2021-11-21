#pragma once
#include "objet3d.h"
#include <string>
#include "d3dx11effect.h"
#include "sommetTerrain.h"

namespace PM3D
{
	class CDispositifD3D11;

	class Terrain : public CObjet3D {
	public:

		Terrain(char* filename, XMFLOAT3 scale, CDispositifD3D11* pDispositif);

		// Destructeur
		virtual ~Terrain();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;


		float getHeight(float x, float z);
		int height;
		int width;

		XMFLOAT3 scale;

	private:
		CSommetTerrain* sommets;

		char* filename;
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

		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
	};
}