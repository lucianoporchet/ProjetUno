#pragma once
#include <string>
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "DispositifD3D11.h"
#include "Texture.h"

using namespace std;

namespace PM3D
{

	class CSommetSprite
	{
	public:
		CSommetSprite (){};
		CSommetSprite ( XMFLOAT3 _position, XMFLOAT2 _coordTex)
		{   position = _position; coordTex = _coordTex; }

	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];

		XMFLOAT3 position;
		XMFLOAT2 coordTex;
	};

class CSpriteTemp : 	public CObjet3D
{
public:
	CSpriteTemp(string NomTexture, CDispositifD3D11* _pDispositif);
	virtual ~CSpriteTemp(void);
	virtual void Anime(float tempsEcoule){};
	virtual	void Draw();
	
	void SetPosDim( int _x, int _y, int _dx=0, int _dy=0 );



protected:
	static CSommetSprite sommets[6];
	ID3D11Buffer* pVertexBuffer;
	CDispositifD3D11* pDispositif;

	ID3D11Buffer* pConstantBuffer;		
	ID3DX11Effect* pEffet;
	ID3DX11EffectTechnique* pTechnique; 
	ID3DX11EffectPass* pPasse; 
	ID3D11InputLayout* pVertexLayout;
	ID3D11ShaderResourceView* pTextureD3D;
	ID3D11SamplerState* pSampleState;

	float dimX;    // Dimensions de la texture
	float dimY;

	XMMATRIX matPosDim;



protected:
	virtual	void InitEffet();



};

}

