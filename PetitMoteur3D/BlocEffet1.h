#pragma once
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "Bloc.h"   // Pour l'index du bloc
#include "Texture.h"
//#include "PhysXManager.h"
//#include "RandomGenerator.h"

namespace PM3D
{

class CDispositifD3D11;

//  Classe : CBlocEffet1
//
//  BUT : 	Classe de bloc avec effet version 1 section 6.5
//
class CBlocEffet1 : public CObjet3D
{
public:
	CBlocEffet1(const float dx, const float dy, const float dz,
		CDispositifD3D11* pDispositif, bool multiplicateur = 0);

	virtual ~CBlocEffet1();

	virtual void Anime(float tempsEcoule) override;
	virtual void Draw() override;

	void SetTexture(CTexture* pTexture);

	//PxRigidDynamic* body;

private:
	bool multi;
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

	ID3D11ShaderResourceView* pTextureD3D;
	ID3D11SamplerState* pSampleState;
};

} // namespace PM3D
