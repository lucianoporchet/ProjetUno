#include "StdAfx.h"
#include "SpriteTemp.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "util.h"

using namespace UtilitairesDX;

namespace PM3D
{
struct ShadersParams
{ 
	XMMATRIX matWVP;	// la matrice totale 
};

// Definir l'organisation de notre sommet
D3D11_INPUT_ELEMENT_DESC CSommetSprite::layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
	
UINT CSommetSprite::numElements = ARRAYSIZE(layout);  

CSommetSprite CSpriteTemp::sommets[6] = {
				CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
				CSommetSprite(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
				CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
				CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
				CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
				CSommetSprite(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f))
									};


CSpriteTemp::CSpriteTemp(string NomTexture, CDispositifD3D11* _pDispositif)
{
	pVertexBuffer = 0;
	pConstantBuffer = 0;		
	pEffet = 0;
	pTechnique = 0; 
	pPasse = 0; 
	pVertexLayout = 0;
	pSampleState = 0;

	pDispositif  = _pDispositif;  // Prendre en note le dispositif

	// Création du vertex buffer et copie des sommets
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( CSommetSprite ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
	InitData.pSysMem = sommets;
	pVertexBuffer = NULL;
		
	DXEssayer( pD3DDevice->CreateBuffer( &bd, &InitData, &pVertexBuffer ), 
					DXE_CREATIONVERTEXBUFFER);

	// Initialisation de l'effet
	InitEffet();

	// Initialisation de la texture
	CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();

	if (NomTexture != "")
	{
		wstring ws;
		ws.assign(NomTexture.begin(), NomTexture.end());
			
		pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
	}

	// Obtenir la dimension de la texture
		ID3D11Resource* pResource;
		ID3D11Texture2D *pTextureInterface = 0;
		pTextureD3D->GetResource(&pResource);
		pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		D3D11_TEXTURE2D_DESC desc;
		pTextureInterface->GetDesc(&desc);

		dimX = float(desc.Width);
		dimY = float(desc.Height);

		matPosDim = XMMatrixIdentity();


}


CSpriteTemp::~CSpriteTemp(void)
{
	DXRelacher(pConstantBuffer);
	DXRelacher(pSampleState);

	DXRelacher(pEffet);
	DXRelacher(pVertexLayout);
	DXRelacher(pVertexBuffer);

}


void CSpriteTemp::InitEffet()
{
	// Compilation et chargement du vertex shader
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	// Création d'un tampon pour les constantes de l'effet
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ShadersParams);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	pD3DDevice->CreateBuffer( &bd, NULL, &pConstantBuffer ); 
		
	// Pour l'effet
	ID3DBlob* pFXBlob = NULL;
			
	DXEssayer( D3DCompileFromFile( L"Sprite1.fx", 0, 0, 0, 
									"fx_5_0", 0, 0,  
									&pFXBlob, 0),
				DXE_ERREURCREATION_FX);
		
	D3DX11CreateEffectFromMemory(  pFXBlob->GetBufferPointer(), 
                  pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

	pFXBlob->Release();
	
	pTechnique = pEffet->GetTechniqueByIndex(0);  
	pPasse = pTechnique->GetPassByIndex(0);

	// Créer l'organisation des sommets pour le VS de notre effet
	D3DX11_PASS_SHADER_DESC effectVSDesc; 
	pPasse->GetVertexShaderDesc(&effectVSDesc);

	D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
	effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
                                               &effectVSDesc2);

	const void *vsCodePtr = effectVSDesc2.pBytecode;
	unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

	pVertexLayout = NULL;
	DXEssayer( pD3DDevice->CreateInputLayout(  
                                 CSommetSprite::layout, 
												CSommetSprite::numElements, 
												vsCodePtr, 
												vsCodeLen, 
												&pVertexLayout ),
				DXE_CREATIONLAYOUT);

	// Initialisation des paramètres de sampling de la texture
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter =  D3D11_FILTER_ANISOTROPIC; 
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Création de l'état de sampling
	pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);

}

void CSpriteTemp::Draw()
{
	// Obtenir le contexte
	ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

	// Choisir la topologie des primitives
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Source des sommets
	UINT stride = sizeof( CSommetSprite );
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );

	// input layout des sommets
	pImmediateContext->IASetInputLayout( pVertexLayout );

	// Initialiser et sélectionner les «constantes» de l'effet
	ShadersParams sp; 
	// sp.matWVP = XMMatrixIdentity();  // retirez cette ligne
	sp.matWVP = XMMatrixTranspose(matPosDim);

	pImmediateContext->UpdateSubresource( pConstantBuffer, 0, NULL, &sp, 0, 0 );

	// Nous n'avons qu'un seul CBuffer	
	ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");  
	pCB->SetConstantBuffer(pConstantBuffer);

	// Activation de la texture
	ID3DX11EffectShaderResourceVariable* variableTexture;
	variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
	variableTexture->SetResource(pTextureD3D);

	// Le sampler state
	ID3DX11EffectSamplerVariable* variableSampler;
	variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
	variableSampler->SetSampler(0, pSampleState);

	pPasse->Apply(0, pImmediateContext);

	// **** Rendu de l'objet	  
	pDispositif->ActiverMelangeAlpha();
	pImmediateContext->Draw( 6, 0 );	
	pDispositif->DesactiverMelangeAlpha();
	
}

void CSpriteTemp::SetPosDim( int _x, int _y, int _dx, int _dy )
{
float x, y, dx, dy;
float posX, posY;
float facteurX, facteurY;

	// Dimensions en pixel
	if (_dx==0 && _dy==0)
	{
		// Dimensions par défaut
		dx = dimX;
		dy = dimY;
	}
	else
	{
		dx = float(_dx);
		dy = float(_dy);
	}

	// Dimensions en facteur
	facteurX = dx*2.0f/pDispositif->GetLargeur();
	facteurY = dy*2.0f/pDispositif->GetHauteur();

	// Position en coordonnées logiques
	// 0,0 pixel = -1,1   
	x = float(_x);
	y = float(_y);

	posX = x*2.0f/pDispositif->GetLargeur() - 1.0f;
	posY = 1.0f - y*2.0f/pDispositif->GetHauteur();

	matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) * 
               XMMatrixTranslation(posX, posY, 0.0f) ;
}


}
