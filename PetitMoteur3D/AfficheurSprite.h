#pragma once
#include "Objet3D.h"
#include <map>

namespace PM3D
{

	class CDispositifD3D11;

	class CSommetSprite
	{
	public:
		CSommetSprite() = default;
		CSommetSprite(const XMFLOAT3& position, const XMFLOAT2& coordTex)
			: m_Position(position)
			, m_CoordTex(coordTex)
		{
		}

	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];

		XMFLOAT3 m_Position;
		XMFLOAT2 m_CoordTex;
	};

	class CAfficheurSprite : public CObjet3D
	{
	public:
		CAfficheurSprite(CDispositifD3D11* _pDispositif);
		virtual ~CAfficheurSprite();
		void Draw() override;
		void DrawZone(int _zone);
		void Anime(float) override;
		void AnimeZone(int _zone, float);

		void AjouterSprite(int _zone, const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0);
		void AjouterSpriteTexte(int _zone, ID3D11ShaderResourceView* pTexture, int _x, int _y);
		void AjouterPanneau(int _zone, const std::string& NomTexture, const XMFLOAT3& _position, bool _followsCam,
			float _dx = 0.0f, float _dy = 0.0f);

	private:
		class CSprite
		{
		public:
			ID3D11ShaderResourceView* pTextureD3D;

			XMMATRIX matPosDim;
			bool bPanneau;
			CSprite()
				: bPanneau(false)
				, pTextureD3D(nullptr)
			{
			}
		};

		class CPanneau : public CSprite
		{
		public:
			XMFLOAT3 position;
			XMFLOAT2 dimension;

			CPanneau()
			{
				bPanneau = true;
			}
		};

		static CSommetSprite sommets[6];
		ID3D11Buffer* pVertexBuffer;
		CDispositifD3D11* pDispositif;

		ID3D11Buffer* pConstantBuffer;
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		ID3D11SamplerState* pSampleState;

		// Tous nos sprites, panneaux et billboards. Separes pour pouvoir les differencier.
		std::vector<std::unique_ptr<CSprite>> tabSprites;
		std::map<int, std::vector<std::unique_ptr<CSprite>>> tabSigns;
		std::map<int, std::vector<std::unique_ptr<CSprite>>> tabBillboards;

		void InitEffet();
	};

} // namespace PM3D
