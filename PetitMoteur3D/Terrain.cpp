#include "stdafx.h"
#include "Terrain.h"
#include "util.h"
#include "SommetTerrain.h"
#include "resource.h"
#include <algorithm>
#include "MoteurWindows.h"

vector<uint32_t> indexes_terrain;

struct ShadersParams
{
	XMMATRIX matWorldViewProj;	// la matrice totale 
	XMMATRIX matWorld;			// matrice de transformation dans le monde 
	XMVECTOR vLumiere; 			// la position de la source d'éclairage (Point)
	XMVECTOR vCamera; 			// la position de la caméra
	XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
	XMVECTOR vAMat; 			// la valeur ambiante du matériau
	XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
	XMVECTOR vDMat; 			// la valeur diffuse du matériau 
};

PM3D::CTerrain::CTerrain(CDispositifD3D11* pDispositif, LectureFichier lecteur) 
	: pDispositif(pDispositif) // Prendre en note le dispositif
	, matWorld(XMMatrixIdentity())
	, rotation(0.0f)
	, pVertexBuffer(nullptr)
	, pIndexBuffer(nullptr)
	, pConstantBuffer(nullptr)
	, pEffet(nullptr)
	, pTechnique(nullptr)
	, pVertexLayout(nullptr)
	, pSampleState(nullptr)
{
	
	pVectorTexturesD3D = {};

	vector<CSommetTerrain> sommets;
	float scale = 1 / 10.0f;

	// Going through vertexes and normals to create each and every vertice on the buffer
	vector<XMFLOAT3> vertexes = lecteur.getVertexes();

	float sideSize = (float)sqrt(vertexes.size());
	vector<XMFLOAT3> normals = lecteur.getNormals();

	int iterate_index = 0;
	for_each(vertexes.begin(), vertexes.end(), [&](XMFLOAT3 vertex) {
		XMFLOAT3 vertexScaled = XMFLOAT3(vertex.x, vertex.y * scale, vertex.z);
		// Adding a new vertex with the following properties : x, y, z from our file, scaled down with the scale value
		// With the corresponding normal from our file
		// And a texCoord calculated from our side size. We do not tile here. We are going to do so in our effect.
		sommets.push_back(CSommetTerrain(vertexScaled, normals[iterate_index], XMFLOAT2(vertexScaled.x/sideSize, vertexScaled.z/sideSize)));
		mapSommets.emplace(std::make_pair((unsigned int)vertexScaled.x, (unsigned int)vertexScaled.z), vertexScaled.y);
		++iterate_index;
	});

	// Going through all the faces and pushing back only the indexes, converted
	vector<long> faces = lecteur.getFaces();
	for_each(faces.begin(), faces.end(), [&](long index) {
		indexes_terrain.push_back((uint32_t)index);
	});

	// Création du vertex buffer et copie des sommets
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = (UINT)sommets.size()*sizeof(CSommetTerrain);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &sommets[0];
	pVertexBuffer = nullptr;

	DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

	// Création de l'index buffer et copie des indices
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = (UINT)indexes_terrain.size()*sizeof(uint32_t);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &indexes_terrain[0];
	pIndexBuffer = nullptr;

	DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
		DXE_CREATIONINDEXBUFFER);

	// Initialisation de l'effet
	InitEffet();
}

PM3D::CTerrain::~CTerrain()
{
	DXRelacher(pVertexBuffer);
	DXRelacher(pIndexBuffer);
	DXRelacher(pConstantBuffer);
	DXRelacher(pPasse);
	DXRelacher(pTechnique);
	DXRelacher(pEffet);
	DXRelacher(pVertexLayout);
	DXRelacher(pSampleState);
}

void PM3D::CTerrain::Anime(float tempsEcoule)
{
	tempsEcoule;
}

void PM3D::CTerrain::Draw()
{
	// Obtenir le contexte
	ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

	// Choisir la topologie des primitives
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Source des sommets
	const UINT stride = sizeof(CSommetTerrain);
	const UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// Source des index
	pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// input layout des sommets
	pImmediateContext->IASetInputLayout(pVertexLayout);

	// Initialiser et sélectionner les «constantes» de l'effet
	ShadersParams sp;
	XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

	sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
	sp.matWorld = XMMatrixTranspose(matWorld);

	sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f);
	sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
	sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
	sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

	ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
	pCB->SetConstantBuffer(pConstantBuffer);

	// Le sampler state
	ID3DX11EffectSamplerVariable* variableSampler;
	variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
	variableSampler->SetSampler(0, pSampleState);

	// Activation de la texture
	ID3DX11EffectShaderResourceVariable* variableTexture;
	variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
	variableTexture->SetResourceArray(pVectorTexturesD3D.data(), 0, (uint32_t)pVectorTexturesD3D.size());

	// **** Rendu de l'objet
	pPasse->Apply(0, pImmediateContext);

	pImmediateContext->DrawIndexed((UINT)indexes_terrain.size(), 0, 0);
}

void PM3D::CTerrain::InitEffet()
{	
	// Compilation et chargement du vertex shader
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	// Création d'un tampon pour les constantes du VS
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ShadersParams);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

	// Pour l'effet
	ID3DBlob* pFXBlob = nullptr;

	DXEssayer(D3DCompileFromFile(L"MiniPhongTerrain.fx", 0, 0, 0,
		"fx_5_0", 0, 0,
		&pFXBlob, nullptr),
		DXE_ERREURCREATION_FX);

	D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

	pFXBlob->Release();

	pTechnique = pEffet->GetTechniqueByIndex(0);
	pPasse = pTechnique->GetPassByIndex(0);

	// Créer l'organisation des sommets pour le VS de notre effet
	D3DX11_PASS_SHADER_DESC effectVSDesc;
	pPasse->GetVertexShaderDesc(&effectVSDesc);

	D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
	effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

	const void* vsCodePtr = effectVSDesc2.pBytecode;
	const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

	pVertexLayout = nullptr;
	DXEssayer(pD3DDevice->CreateInputLayout(CSommetTerrain::layout,
		CSommetTerrain::numElements,
		vsCodePtr,
		vsCodeLen,
		&pVertexLayout),
		DXE_CREATIONLAYOUT);

	// Initialisation des paramètres de sampling de la texture
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
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

void PM3D::CTerrain::AddTexture(CTexture* pTexture)
{
	pVectorTexturesD3D.push_back(pTexture->GetD3DTexture());
}
