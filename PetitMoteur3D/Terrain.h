#pragma once
#include <vector>
#include <future>
#include <fstream>
#include <string>

#include "Objet3D.h"
#include "d3dx11effect.h"
#include "sommetbloc.h"


namespace PM3D
{

	class CDispositifD3D11;


	//  Classe : CBloc
	//
	//  BUT : 	Classe de bloc
	//
	class CTerrain : public CObjet3D
	{
	public:
		CTerrain(std::string index,std::string sommets,
			CDispositifD3D11* pDispositif);
		
		// Destructeur
		virtual ~CTerrain();

		virtual void Anime(float tempsEcoule) override;
		virtual void Draw() override;

	private:
		static void readSommets(std::string sf, std::vector<CSommetBloc>* s, float* width);
		static void readIndex(std::string ixf, std::vector<unsigned int>* ix);
		

		std::vector<CSommetBloc> v_sommets;
		std::vector<unsigned int> v_index;
		CDispositifD3D11* pDispositif;

		float widthT;

		void InitEffet();

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;
		ID3D11InputLayout* pVertexLayout;

		// Définitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		XMMATRIX matWorld;
		float rotation;
		bool freeCam;
		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;

		/*static std::vector<CSommetTerrain> v_sommets;
		static std::vector<unsigned int> v_index;*/
		

	};

} // namespace PM3D
