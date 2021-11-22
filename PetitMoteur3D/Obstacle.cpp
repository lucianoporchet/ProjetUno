#include "stdafx.h"
#include "Obstacle.h"
#include "util.h"
#include "DispositifD3D11.h"
#include "resource.h"
#include "MoteurWindows.h"

using namespace DirectX;

namespace PM3D {

	struct ShadersParams {
		XMMATRIX matWorldViewProj; // la matrice totale
		XMMATRIX matWorld;    // matrice de transformation dans le monde
		XMVECTOR vLumiere;    // la position de la source d’éclairage (Point)
		XMVECTOR vCamera;    // la position de la caméra
		XMVECTOR vAEcl;   // la valeur ambiante de l’éclairage
		XMVECTOR vAMat;     // la valeur ambiante du matériau
		XMVECTOR vDEcl;     // la valeur diffuse de l’éclairage
		XMVECTOR vDMat;     // la valeur diffuse du matériau
	};


	Obstacle::Obstacle(std::string filename, XMFLOAT3 scale, CDispositifD3D11* pDispositif)
		: pDispositif(pDispositif) // Prendre en note le dispositif
		, matWorld(XMMatrixIdentity())
		, rotation(0.0f)
		, pVertexBuffer(nullptr)
		, pIndexBuffer(nullptr)
		, pVertexShader(nullptr)
		, pPixelShader(nullptr)
		, pVertexLayout(nullptr)
		, pConstantBuffer(nullptr)
		, filename(filename)
	{
		params.NomChemin = "";
		params.NomFichier = filename;
		

		
		body = PhysXManager::get().createDynamic(PxTransform(RandomGenerator::get().randomVec3(-100, 100)), PxSphereGeometry(100.0f), PxVec3(0, 0, 0), PhysXManager::FilterGroup::eObstacle);
		//body->addForce(RandomGenerator::get().randomVec3(-10, 10), PxForceMode::eIMPULSE);
		//body->addTorque(RandomGenerator::get().randomVec3(-10, 10), PxForceMode::eIMPULSE);
		PhysXManager::get().addToScene(body);

		chargeur.Chargement(params);

		const size_t nombreSommets = chargeur.GetNombreSommets();
		std::unique_ptr<CSommetBloc[]> ts(new CSommetBloc[nombreSommets]);

		for (uint32_t i = 0; i < nombreSommets; ++i)
		{
			ts[i].setPosition(chargeur.GetPosition(i));
			ts[i].setNormal(chargeur.GetNormale(i));
			ts[i].setTexture(chargeur.GetCoordTex(i));
		}

		

		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = static_cast<uint32_t>(sizeof(CSommetBloc) * nombreSommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = ts.get();

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t) * chargeur.GetNombreIndex());
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = chargeur.GetIndexData();

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);

		// Initialisation de l'effet
		InitEffect();


	}

	void Obstacle::Anime(float tempsEcoule)
	{
		PxVec3 pos = body->getGlobalPose().p;
		XMFLOAT3 posF3(pos.x, pos.y, pos.z);
		XMVECTOR posVec = XMLoadFloat3(&posF3);

		PxQuat quat = body->getGlobalPose().q;
		XMFLOAT4 quatF3(quat.x, quat.y, quat.z, quat.w);
		XMVECTOR quatVec = XMLoadFloat4(&quatF3);

		matWorld = XMMatrixRotationQuaternion(quatVec) * XMMatrixTranslationFromVector(posVec);
	}

	void Obstacle::Draw()
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
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);
		// Initialiser et sélectionner les « constantes » de l’effet
		ShadersParams sp;
		CMoteurWindows& moteur = CMoteurWindows::GetInstance();
		XMMATRIX viewProj;

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);

		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f);
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		//sp.remplissage = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);


		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
		// Nous n’avons qu’un seul CBuffer
		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
		pCB->SetConstantBuffer(pConstantBuffer);


		// Activation de la texture
		ID3DX11EffectShaderResourceVariable* variableTexture1;
		variableTexture1 = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
		variableTexture1->SetResource(pTextureD3D1);

		//ID3DX11EffectShaderResourceVariable* variableTexture2;
		//variableTexture2 = pEffet->GetVariableByName("textureEntree2")->AsShaderResource();
		//variableTexture2->SetResource(pTextureD3D2);

		//ID3DX11EffectShaderResourceVariable* variableTexturea;
		//variableTexturea = pEffet->GetVariableByName("textureEntreea")->AsShaderResource();
		//variableTexturea->SetResource(pTextureD3Da);

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		// **** Rendu de l’objet
		pPasse->Apply(0, pImmediateContext);
		pImmediateContext->DrawIndexed((UINT)chargeur.GetNombreIndex(), 0, 0);
	}

	void Obstacle::InitEffect()
	{
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Création d’un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, NULL, &pConstantBuffer);

		// Pour l’effet
		ID3DBlob* pFXBlob = NULL;
		DXEssayer(D3DCompileFromFile(L"MiniPhongBloc.fx", 0, 0, 0,
			"fx_5_0", 0, 0,
			&pFXBlob, 0),
			DXE_ERREURCREATION_FX);
		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(),
			pFXBlob->GetBufferSize(),
			0,
			pD3DDevice,
			&pEffet);


		pFXBlob->Release();
		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);
		// Créer l’organisation des sommets pour le VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		pPasse->GetVertexShaderDesc(&effectVSDesc);
		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
			&effectVSDesc2);
		const void* vsCodePtr = effectVSDesc2.pBytecode;
		unsigned vsCodeLen = effectVSDesc2.BytecodeLength;
		pVertexLayout = NULL;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
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

	Obstacle::~Obstacle()
	{
		DXRelacher(pPixelShader);
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexShader);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}

	void Obstacle::SetTexture(CTexture* pTexture1)
	{
		pTextureD3D1 = pTexture1->GetD3DTexture();
	}


}