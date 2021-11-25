#include "StdAfx.h"

#include "ObjetMesh.h"
#include "chargeur.h"
#include "dispositifD3D11.h"
#include "moteurWindows.h"
#include "util.h"
#include "resource.h"
#include "DispositifD3D11.h"

namespace PM3D
{

// Definir l'organisation de notre sommet
D3D11_INPUT_ELEMENT_DESC CObjetMesh::CSommetMesh::layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

UINT CObjetMesh::CSommetMesh::numElements;

struct ShadersParams // toujours un multiple de 16 pour les constantes
{
	XMMATRIX matWorldViewProj;	// la matrice totale 
	//XMMATRIX matWorldViewProjLight;	// WVP pour lumiere 
	XMMATRIX matWorld;			// matrice de transformation dans le monde 
	XMVECTOR vLumiere; 			// la position de la source d'éclairage (Point)
	XMVECTOR vCamera; 			// la position de la caméra
	XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
	XMVECTOR vAMat; 			// la valeur ambiante du matériau
	XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
	XMVECTOR vDMat; 			// la valeur diffuse du matériau 
	XMVECTOR vSEcl; 			// la valeur spéculaire de l'éclairage 
	XMVECTOR vSMat; 			// la valeur spéculaire du matériau 
	float puissance;
	int32_t bTex;					// Texture ou materiau 
	XMFLOAT2 remplissage;
};

// Ancien constructeur
CObjetMesh::CObjetMesh(const IChargeur& chargeur, CDispositifD3D11* _pDispositif)
	: pDispositif(_pDispositif) // prendre en note le dispositif
	, matWorld(XMMatrixIdentity())
	, rotation(0.0f)
{
	// Placer l'objet sur la carte graphique
	TransfertObjet(chargeur);

	// Initialisation de l'effet
	InitEffet();
}

// Constructeur de conversion
// Constructeur pour test ou pour création d'un objet de format OMB
CObjetMesh::CObjetMesh(const IChargeur& chargeur, const std::string& nomfichier, CDispositifD3D11* _pDispositif)
	: pDispositif(_pDispositif) // prendre en note le dispositif
	, matWorld(XMMatrixIdentity())
	, rotation(0.0f)
{
	//// Placer l'objet sur la carte graphique
	// TransfertObjet(chargeur); // On n'utilisera plus cette fonction

	EcrireFichierBinaire(chargeur, nomfichier);

	LireFichierBinaire(nomfichier);

	// Initialisation de l'effet
	InitEffet();
}

// Constructeur pour lecture d'un objet de format OMB
CObjetMesh::CObjetMesh(const std::string& nomfichier, CDispositifD3D11* _pDispositif, float scale)
	: pDispositif(_pDispositif) // prendre en note le dispositif
	, matWorld(XMMatrixIdentity())
	, rotation(0.0f)
{
	// Placer l'objet sur la carte graphique
	LireFichierBinaire(nomfichier);

	// Initialisation de l'effet
	InitEffet();

	
	matWorld = XMMatrixScaling(scale, scale, scale);
}

CObjetMesh::~CObjetMesh()
{
	SubsetMaterialIndex.clear();
	SubsetIndex.clear();
	Material.clear();

	DXRelacher(pConstantBuffer);
	DXRelacher(pSampleState);

	DXRelacher(pEffet);
	DXRelacher(pVertexLayout);
	DXRelacher(pIndexBuffer);
	DXRelacher(pVertexBuffer);

	//DXRelacher(pShadowMapView);
	//DXRelacher(pRenderTargetView);
	//DXRelacher(pTextureShadowMap);
	//DXRelacher(pDepthStencilView);
	//DXRelacher(pDepthTexture);
	//DXRelacher(pVertexLayoutShadow);
}

void CObjetMesh::InitEffet()
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
	DXEssayer(pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer));

	// Pour l'effet
	ID3DBlob* pFXBlob = nullptr;

	DXEssayer(D3DCompileFromFile(L"MiniPhong.fx", 0, 0, 0,
		"fx_5_0", 0, 0, &pFXBlob, 0),
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
	const uint32_t vsCodeLen = effectVSDesc2.BytecodeLength;

	pVertexLayout = nullptr;

	CSommetMesh::numElements = ARRAYSIZE(CSommetMesh::layout);
	DXEssayer(pD3DDevice->CreateInputLayout(CSommetMesh::layout,
		CSommetMesh::numElements,
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

void CObjetMesh::Anime(float tempsEcoule)
{

	
}

void CObjetMesh::Draw()
{
	// Obtenir le contexte
	ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

	// Choisir la topologie des primitives
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// input layout des sommets
	pImmediateContext->IASetInputLayout(pVertexLayout);

	// Index buffer
	pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Vertex buffer
	UINT stride = sizeof(CSommetMesh);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// Initialiser et sélectionner les «constantes» de l'effet
	ShadersParams sp;
	const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

	sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
	sp.matWorld = XMMatrixTranspose(matWorld);

	sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -15.0f, 1.0f);
	sp.vCamera = XMVectorSet(0.0f, 3.0f, -5.0f, 1.0f);
	sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
	sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.vSEcl = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);

	// Le sampler state
	ID3DX11EffectSamplerVariable* variableSampler;
	variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
	variableSampler->SetSampler(0, pSampleState);

	// Dessiner les subsets non-transparents
	for (int32_t i = 0; i < NombreSubset; ++i)
	{
		int32_t indexStart = SubsetIndex[i];
		int32_t indexDrawAmount = SubsetIndex[i + 1] - SubsetIndex[i];
		if (indexDrawAmount)
		{
			sp.vAMat = XMLoadFloat4(&Material[SubsetMaterialIndex[i]].Ambient);
			sp.vDMat = XMLoadFloat4(&Material[SubsetMaterialIndex[i]].Diffuse);
			sp.vSMat = XMLoadFloat4(&Material[SubsetMaterialIndex[i]].Specular);
			sp.puissance = Material[SubsetMaterialIndex[i]].Puissance;

			// Activation de la texture ou non
			if (Material[SubsetMaterialIndex[i]].pTextureD3D != nullptr)
			{
				ID3DX11EffectShaderResourceVariable* variableTexture;
				variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
				variableTexture->SetResource(Material[SubsetMaterialIndex[i]].pTextureD3D);
				sp.bTex = 1;
			}
			else
			{
				sp.bTex = 1;
			}

			// IMPORTANT pour ajuster les param.
			pPasse->Apply(0, pImmediateContext);

			ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
			pCB->SetConstantBuffer(pConstantBuffer);
			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

			pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
		}
	}
}

void CObjetMesh::setMatWorld(XMMATRIX& matworld)
{
	matWorld = matworld;
}

void CObjetMesh::TransfertObjet(const IChargeur& chargeur)
{
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
	{
		const size_t nombreSommets = chargeur.GetNombreSommets();
		std::unique_ptr<CSommetMesh[]> ts(new CSommetMesh[nombreSommets]);

		for (uint32_t i = 0; i < nombreSommets; ++i)
		{
			ts[i].position = chargeur.GetPosition(i);
			ts[i].normal = chargeur.GetNormale(i);
			ts[i].coordTex = chargeur.GetCoordTex(i);
		}

		// 1. SOMMETS b) Création du vertex buffer et copie des sommets
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = static_cast<uint32_t>(sizeof(CSommetMesh) * nombreSommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = ts.get();
		pVertexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
	}

	// 2. INDEX - Création de l'index buffer et copie des indices
	//            Les indices étant habituellement des entiers, j'ai
	//            pris directement ceux du chargeur mais attention au 
	//            format si vous avez autre chose que DXGI_FORMAT_R32_UINT
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t) * chargeur.GetNombreIndex());
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = chargeur.GetIndexData();
		pIndexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);
	}

	// 3. Les sous-objets
	NombreSubset = chargeur.GetNombreSubset();

	//    Début de chaque sous-objet et un pour la fin
	SubsetIndex.reserve(NombreSubset);
	chargeur.CopieSubsetIndex(SubsetIndex);

	// 4. MATERIAUX
	// 4a) Créer un matériau de défaut en index 0
	//     Vous pourriez changer les valeurs, j'ai conservé 
	//     celles du constructeur
	Material.reserve(chargeur.GetNombreMaterial() + 1);
	Material.emplace_back(CMaterial());

	// 4b) Copie des matériaux dans la version locale
	for (int32_t i = 0; i < chargeur.GetNombreMaterial(); ++i)
	{
		CMaterial mat;

		chargeur.GetMaterial(i, mat.NomFichierTexture,
			mat.NomMateriau,
			mat.Ambient,
			mat.Diffuse,
			mat.Specular,
			mat.Puissance);

		Material.push_back(mat);
	}

	// 4c) Trouver l'index du materiau pour chaque sous-ensemble
	SubsetMaterialIndex.reserve(chargeur.GetNombreSubset());
	for (int32_t i = 0; i < chargeur.GetNombreSubset(); ++i)
	{
		int32_t index;
		for (index = 0; index < Material.size(); ++index)
		{
			if (Material[index].NomMateriau == chargeur.GetMaterialName(i)) break;
		}
		if (index >= Material.size()) index = 0;  // valeur de défaut
		SubsetMaterialIndex.push_back(index);
	}

	// 4d) Chargement des textures
	CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();

	for (uint32_t i = 0; i < Material.size(); ++i)
	{
		if (Material[i].NomFichierTexture.length() > 0)
		{
			const std::wstring ws(Material[i].NomFichierTexture.begin(), Material[i].NomFichierTexture.end());
			Material[i].pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
		}
	}
}

void CObjetMesh::EcrireFichierBinaire(const IChargeur& chargeur, const std::string& nomFichier)
{
	std::ofstream fichier;
	fichier.open(nomFichier, std::ios::out | std::ios_base::binary);
	// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
	{
		int32_t nombreSommets = static_cast<int32_t>(chargeur.GetNombreSommets());
		std::unique_ptr<CSommetMesh[]> ts(new CSommetMesh[nombreSommets]);

		for (int32_t i = 0; i < nombreSommets; ++i)
		{
			ts[i].position = chargeur.GetPosition(i);
			ts[i].normal = chargeur.GetNormale(i);
			ts[i].coordTex = chargeur.GetCoordTex(i);
		}

		// 1. SOMMETS b) Écriture des sommets dans un fichier binaire
		fichier.write((char*)&nombreSommets, sizeof(nombreSommets));
		fichier.write((char*)ts.get(), nombreSommets * sizeof(CSommetMesh));
	}

	// 2. INDEX 
	const int32_t nombreIndex = static_cast<int32_t>(chargeur.GetNombreIndex());

	fichier.write((char*)&nombreIndex, sizeof(nombreIndex));
	fichier.write((char*)chargeur.GetIndexData(), nombreIndex * sizeof(uint32_t));

	// 3. Les sous-objets
	const int32_t NombreSubset = chargeur.GetNombreSubset();

	//    Début de chaque sous-objet et un pour la fin
	std::vector<int32_t> SI;
	SI.reserve(NombreSubset);
	chargeur.CopieSubsetIndex(SI);

	fichier.write((char*)&NombreSubset, sizeof(NombreSubset));
	fichier.write((char*)SI.data(), (NombreSubset + 1) * sizeof(int32_t));

	// 4. MATERIAUX
	// 4a) Créer un matériau de défaut en index 0
	//     Vous pourriez changer les valeurs, j'ai conservé 
	//     celles du constructeur
	int32_t NbMaterial = static_cast<int32_t>(chargeur.GetNombreMaterial());
	std::vector<CMaterial> MatLoad;
	MatLoad.reserve(NbMaterial + 1);
	MatLoad.emplace_back(CMaterial());

	// 4b) Copie des matériaux dans la version locale
	CMaterial mat;
	for (int32_t i = 0; i < NbMaterial; ++i)
	{
		chargeur.GetMaterial(i, mat.NomFichierTexture,
			mat.NomMateriau,
			mat.Ambient,
			mat.Diffuse,
			mat.Specular,
			mat.Puissance);

		MatLoad.push_back(mat);
	}

	NbMaterial++;
	fichier.write((char*)&NbMaterial, sizeof(int32_t));

	MaterialBlock mb;
	for (int32_t i = 0; i < NbMaterial; ++i)
	{
		MatLoad[i].MatToBlock(mb);
		fichier.write((char*)&mb, sizeof(MaterialBlock));
	}

	// 4c) Trouver l'index du materiau pour chaque sous-ensemble
	std::vector<int32_t> SubsetMI;
	SubsetMI.reserve(NombreSubset);
	for (int32_t i = 0; i < NombreSubset; ++i)
	{
		int32_t index;
		for (index = 0; index < MatLoad.size(); ++index)
		{
			if (MatLoad[index].NomMateriau == chargeur.GetMaterialName(i)) break;
		}

		if (index >= MatLoad.size()) index = 0;  // valeur de défaut

		SubsetMI.push_back(index);
	}

	fichier.write((char*)SubsetMI.data(), (NombreSubset) * sizeof(int32_t));
}

void CObjetMesh::LireFichierBinaire(const std::string& nomFichier)
{
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	std::ifstream fichier;
	fichier.open(nomFichier, std::ios::in | std::ios_base::binary);
	assert(fichier.is_open());

	// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
	{
		int32_t nombreSommets;
		fichier.read((char*)&nombreSommets, sizeof(nombreSommets));

		std::unique_ptr<CSommetMesh[]> ts(new CSommetMesh[nombreSommets]);

		// 1. SOMMETS b) Lecture des sommets à partir d'un fichier binaire
		fichier.read((char*)ts.get(), nombreSommets * sizeof(CSommetMesh));

		// 1. SOMMETS b) Création du vertex buffer et copie des sommets
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CSommetMesh) * nombreSommets;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = ts.get();
		pVertexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
	}

	// 2. INDEX 
	{
		int32_t nombreIndex;
		fichier.read((char*)&nombreIndex, sizeof(nombreIndex));

		std::unique_ptr<uint32_t[]> index(new uint32_t[nombreIndex]);
		fichier.read((char*)index.get(), nombreIndex * sizeof(uint32_t));

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * nombreIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index.get();
		pIndexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);
	}

	// 3. Les sous-objets
	fichier.read((char*)&NombreSubset, sizeof(NombreSubset));
	//    Début de chaque sous-objet et un pour la fin
	{
		std::unique_ptr<int32_t[]> si(new int32_t[NombreSubset + 1]);

		fichier.read((char*)si.get(), (NombreSubset + 1) * sizeof(int32_t));
		SubsetIndex.assign(si.get(), si.get() + (NombreSubset + 1));
	}

	// 4. MATERIAUX
	// 4a) Créer un matériau de défaut en index 0
	//     Vous pourriez changer les valeurs, j'ai conservé 
	//     celles du constructeur
	CMaterial mat;

	int32_t NbMaterial;
	fichier.read((char*)&NbMaterial, sizeof(int32_t));

	Material.resize(NbMaterial);

	MaterialBlock mb;
	for (int32_t i = 0; i < NbMaterial; ++i)
	{
		fichier.read((char*)&mb, sizeof(MaterialBlock));
		Material[i].BlockToMat(mb);
	}

	// 4c) Trouver l'index du materiau pour chaque sous-ensemble
	{
		std::unique_ptr<int32_t[]> smi(new int32_t[NombreSubset]);

		fichier.read((char*)smi.get(), (NombreSubset) * sizeof(int32_t));
		SubsetMaterialIndex.assign(smi.get(), smi.get() + NombreSubset);
	}

	// 4d) Chargement des textures
	CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();

	for (uint32_t i = 0; i < Material.size(); ++i)
	{
		if (Material[i].NomFichierTexture.length() > 0)
		{
			std::wstring ws(Material[i].NomFichierTexture.begin(), Material[i].NomFichierTexture.end());
			Material[i].pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
		}
	}
}


} // namespace PM3D
