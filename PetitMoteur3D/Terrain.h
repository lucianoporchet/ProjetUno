#pragma once
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "dispositifD3D11.h"
#include "LectureFichier.h"
#include <map>
#include "Texture.h"

namespace PM3D {
	class CTerrain :
		public CObjet3D
	{
	public:
		CTerrain(CDispositifD3D11* pDispositif, LectureFichier lecteur);

		virtual ~CTerrain();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;

		std::map<std::pair<unsigned int, unsigned int>, float> mapSommets;

		void AddTexture(CTexture* pTexture);

	private:
		void InitEffet();

		CDispositifD3D11* pDispositif;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		// Définitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		float rotation;

		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		std::vector<ID3D11ShaderResourceView*> pVectorTexturesD3D; 
		ID3D11SamplerState* pSampleState;
	};
}


