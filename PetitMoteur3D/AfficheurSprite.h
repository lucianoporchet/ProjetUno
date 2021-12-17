#pragma once
#include "Objet3D.h"
#include <map>
#include "AfficheurTexte.h"

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

		void displayPauseSprite();
		void hidePauseSprite();

		void displayOutOfBoundsWarns() { tabUISprite[10]->displayed = true; tabUISprite[11]->displayed = true; }
		void hideOutOfBoundsWarns() { tabUISprite[10]->displayed = false; tabUISprite[11]->displayed = false; }

		void displayWarning() { tabUISprite[9]->displayed = true; };
		void hideWarning() { tabUISprite[9]->displayed = false; };
		
		void displayFinalPortal() { tabUISprite.back()->displayed = true; };
		bool isFinalPortalOn() { return tabUISprite.back()->displayed; };

		// Ces fonctions doivent etre plus ou moins specialisees, mais restent a titre d'exemple pour les autres.
		void AjouterSprite(int _zone, const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0);
		void AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, int _x, int _y);		
		void AjouterPanneau(int _zone, bool _isPortal, const std::string& NomTexture, const XMFLOAT3& _position, bool _followsCam,
			float _dx = 0.0f, float _dy = 0.0f);		
		
		void AjouterEtoile(const std::string& NomTexture, const XMFLOAT3& _offset, float _dx = 0.0f, float _dy = 0.0f);
		void AjouterPauseSprite(const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0);
		void AjouterUISprite(const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0, bool _displayed = false);

		void removeBillboardAtPos(int _zone, XMFLOAT3 _pos);

		// Note : 0, 1, 2. NE PAS UTILISER D'AUTRES VALEURS.
		void afficherCle(int _id) { tabUISprite[_id]->displayed = true; };
		// Note : les jauges commencent a 3 dans le tableau et il y en a 6 sprites.
		void updateGauge(int _speed);

		void changePauseToGameOver(bool _gameWon, std::unique_ptr<CAfficheurTexte> & _timerTex);
		void changePauseToTitleScreen();
		void changePauseToPauseUI();
		void changePauseToLoading();

		int starAreaOffsetFromCenter = 17;
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
		
		class CUISprite : public CSprite
		{
		public:
			bool displayed;
			CUISprite() {
				displayed = false;
			}
		};

		class CPanneau : public CSprite
		{
		public:
			XMFLOAT3 position;
			XMFLOAT2 dimension;
			bool portal;
			int rotation;

			CPanneau()
			{
				bPanneau = true;
				portal = false;
				rotation = 0;
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
		std::map<int, std::vector<std::unique_ptr<CPanneau>>> tabSigns;
		std::map<int, std::vector<std::unique_ptr<CPanneau>>> tabBillboards;

		std::vector<std::unique_ptr<CSprite>> tabPauseSprite;

		std::vector<std::unique_ptr<CPanneau>> tabEtoiles;

		std::vector<std::unique_ptr<CUISprite>> tabUISprite;
		bool pauseStatus = true;

		void InitEffet();
	};

} // namespace PM3D
