#pragma once
#include "Objet3D.h"

namespace PM3D
{

class CDispositifD3D11;

const uint16_t index_bloc[36] = {
	0,1,2,			// devant
	0,2,3,			// devant
	5,6,7,			// arrière
	5,7,4,			// arrière
	8,9,10,			// dessous
	8,10,11,		// dessous
	13,14,15,		// dessus
	13,15,12,		// dessus
	19,16,17,		// gauche
	19,17,18,		// gauche
	20,21,22,		// droite
	20,22,23		// droite
};

//  Classe : CBloc
//
//  BUT : 	Classe de bloc
//
class CBloc : public CObjet3D
{
public:
	CBloc(const float dx, const float dy, const float dz,
		CDispositifD3D11* pDispositif);

	// Destructeur
	virtual ~CBloc();

	virtual void Anime(float tempsEcoule) override;
	virtual void Draw() override;

private:
	CDispositifD3D11 * pDispositif;
	void InitShaders();

	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;

	ID3D11VertexShader*  pVertexShader;
	ID3D11PixelShader*  pPixelShader;
	ID3D11InputLayout* pVertexLayout;

	// Définitions des valeurs d'animation
	ID3D11Buffer* pConstantBuffer;
	XMMATRIX matWorld;
	float rotation;
};

} // namespace PM3D
