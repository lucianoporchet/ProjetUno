#include "StdAfx.h"
#include "Bloc.h"
#include "sommetbloc.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"

namespace PM3D
{
//  FONCTION : CBloc, constructeur paramètré 
//  BUT :	Constructeur d'une classe de bloc
//  PARAMÈTRES:
//		dx, dy, dz:	dimension en x, y, et z
//		pDispositif: pointeur sur notre objet dispositif
CBloc::CBloc(const float dx, const float dy, const float dz,
	CDispositifD3D11* pDispositif_)
	: pDispositif(pDispositif_) // Prendre en note le dispositif
	, matWorld(XMMatrixIdentity())
	, rotation(0.0f)
	, pVertexBuffer(nullptr)
	, pIndexBuffer(nullptr)
	, pVertexShader(nullptr)
	, pPixelShader(nullptr)
	, pVertexLayout(nullptr)
	, pConstantBuffer(nullptr)
{
	// Les points
	XMFLOAT3 point[8] =
	{
		XMFLOAT3(-dx / 2, dy / 2, -dz / 2),
		XMFLOAT3(dx / 2, dy / 2, -dz / 2),
		XMFLOAT3(dx / 2, -dy / 2, -dz / 2),
		XMFLOAT3(-dx / 2, -dy / 2, -dz / 2),
		XMFLOAT3(-dx / 2, dy / 2, dz / 2),
		XMFLOAT3(-dx / 2, -dy / 2, dz / 2),
		XMFLOAT3(dx / 2, -dy / 2, dz / 2),
		XMFLOAT3(dx / 2, dy / 2, dz / 2)
	};

	// Calculer les normales
	const XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
	const XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arrière
	const XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
	const XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
	const XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
	const XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite

	CSommetBloc sommets[24] =
	{
		// Le devant du bloc
		CSommetBloc(point[0], n0),
		CSommetBloc(point[1], n0),
		CSommetBloc(point[2], n0),
		CSommetBloc(point[3], n0),

		// L'arrière du bloc
		CSommetBloc(point[4], n1),
		CSommetBloc(point[5], n1),
		CSommetBloc(point[6], n1),
		CSommetBloc(point[7], n1),

		// Le dessous du bloc
		CSommetBloc(point[3], n2),
		CSommetBloc(point[2], n2),
		CSommetBloc(point[6], n2),
		CSommetBloc(point[5], n2),

		// Le dessus du bloc
		CSommetBloc(point[0], n3),
		CSommetBloc(point[4], n3),
		CSommetBloc(point[7], n3),
		CSommetBloc(point[1], n3),

		// La face gauche
		CSommetBloc(point[0], n4),
		CSommetBloc(point[3], n4),
		CSommetBloc(point[5], n4),
		CSommetBloc(point[4], n4),

		// La face droite
		CSommetBloc(point[1], n5),
		CSommetBloc(point[7], n5),
		CSommetBloc(point[6], n5),
		CSommetBloc(point[2], n5)
	};

	// Création du vertex buffer et copie des sommets
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(sommets);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = sommets;

	DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

	// Création de l'index buffer et copie des indices
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(index_bloc);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = index_bloc;

	DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
		DXE_CREATIONINDEXBUFFER);

	// Inititalisation des shaders
	InitShaders();
}

void CBloc::Anime(float tempsEcoule)
{
	rotation = rotation + ((XM_PI * 2.0f) / 3.0f * tempsEcoule);

	// modifier la matrice de l'objet bloc
	matWorld = XMMatrixRotationX(rotation);
}

void CBloc::Draw()
{
	// Obtenir le contexte
	ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

	// Choisir la topologie des primitives
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Source des sommets
	const UINT stride = sizeof(CSommetBloc);
	const UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// Source des index
	pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// input layout des sommets
	pImmediateContext->IASetInputLayout(pVertexLayout);

	// Activer le VS
	pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);

	// Initialiser et sélectionner les «constantes» du VS
	const XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
	const XMMATRIX matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
	pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &matWorldViewProj, 0, 0);

	pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	// Activer le PS
	pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);

	// **** Rendu de l'objet
	pImmediateContext->DrawIndexed(ARRAYSIZE(index_bloc), 0, 0);
}

CBloc::~CBloc()
{
	DXRelacher(pPixelShader);
	DXRelacher(pConstantBuffer);
	DXRelacher(pVertexLayout);
	DXRelacher(pVertexShader);
	DXRelacher(pIndexBuffer);
	DXRelacher(pVertexBuffer);
}

void CBloc::InitShaders()
{
	// Compilation et chargement du vertex shader
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	ID3DBlob* pVSBlob = nullptr;
	DXEssayer(D3DCompileFromFile(L"vs1.vhl",
		nullptr, nullptr,
		"VS1",
		"vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS,
		0,
		&pVSBlob, nullptr), DXE_FICHIER_VS);

	DXEssayer(pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		nullptr,
		&pVertexShader),
		DXE_CREATION_VS);

	// Créer l'organisation des sommets
	pVertexLayout = nullptr;
	DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
		CSommetBloc::numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&pVertexLayout),
		DXE_CREATIONLAYOUT);

	pVSBlob->Release(); //  On n'a plus besoin du blob

						// Création d'un tampon pour les constantes du VS
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(matWorld);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

	// Compilation et chargement du pixel shader
	ID3DBlob* pPSBlob = nullptr;
	DXEssayer(D3DCompileFromFile(L"ps1.phl",
		nullptr, nullptr,
		"PS1",
		"ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS,
		0,
		&pPSBlob,
		nullptr), DXE_FICHIER_PS);

	DXEssayer(pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		nullptr,
		&pPixelShader),
		DXE_CREATION_PS);

	pPSBlob->Release(); //  On n'a plus besoin du blob
}

} // namespace PM3D
