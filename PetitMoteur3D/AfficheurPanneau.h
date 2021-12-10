#pragma once
#include "Objet3D.h"

namespace PM3D
{

class CDispositifD3D11;

class CSommetPanneau
{
public:
	CSommetPanneau() = default;
	CSommetPanneau(const XMFLOAT3& position, const XMFLOAT2& coordTex)
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

class CAfficheurPanneau : public CObjet3D
{
public:
	CAfficheurPanneau(CDispositifD3D11* _pDispositif);
	virtual ~CAfficheurPanneau();
	virtual void Draw() override;
	virtual void Anime(float) override;

	void AjouterPanneau(const std::string& NomTexture, const XMFLOAT3& _position,
		float _dx = 0.0f, float _dy = 0.0f);
	void AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, int _x, int _y);

private:
	class CSprite
	{
	public:
		ID3D11ShaderResourceView * pTextureD3D;

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

	static CSommetPanneau sommets[6];
	ID3D11Buffer* pVertexBuffer;
	CDispositifD3D11* pDispositif;

	ID3D11Buffer* pConstantBuffer;
	ID3DX11Effect* pEffet;
	ID3DX11EffectTechnique* pTechnique;
	ID3DX11EffectPass* pPasse;
	ID3D11InputLayout* pVertexLayout;

	ID3D11SamplerState* pSampleState;

	// Tous nos sprites
	std::vector<std::unique_ptr<CSprite>> tabSprites;

	void InitEffet();
};

} // namespace PM3D
