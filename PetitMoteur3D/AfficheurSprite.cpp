#include "StdAfx.h"
#include "AfficheurSprite.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "util.h"
#include "DispositifD3D11.h"
#include "SceneManager.h"

namespace PM3D
{
	struct ShadersParams
	{
		XMMATRIX matWVP;	// la matrice totale 
	};

	// Definir l'organisation de notre sommet
	D3D11_INPUT_ELEMENT_DESC CSommetSprite::layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT CSommetSprite::numElements = ARRAYSIZE(layout);

	CSommetSprite CAfficheurSprite::sommets[6] =
	{
		//CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
		//CSommetSprite(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
		//CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
		//CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
		//CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
		//CSommetSprite(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f))
		 
		CSommetSprite(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
		CSommetSprite(XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
		CSommetSprite(XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
		CSommetSprite(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
		CSommetSprite(XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
		CSommetSprite(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f))
	};

	CAfficheurSprite::CAfficheurSprite(CDispositifD3D11* _pDispositif)
		: pDispositif(_pDispositif)
		, pVertexBuffer(nullptr)
		, pConstantBuffer(nullptr)
		, pEffet(nullptr)
		, pTechnique(nullptr)
		, pPasse(nullptr)
		, pVertexLayout(nullptr)
		, pSampleState(nullptr)
	{
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

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer),
			DXE_CREATIONVERTEXBUFFER);

		// Initialisation de l'effet
		InitEffet();
	}

	CAfficheurSprite ::~CAfficheurSprite()
	{
		DXRelacher(pConstantBuffer);
		DXRelacher(pSampleState);

		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexBuffer);

		tabBillboards.clear();
		tabSprites.clear();
		tabSigns.clear();
		tabPauseSprite.clear();
	}

	void CAfficheurSprite::InitEffet()
	{
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Création d'un tampon pour les constantes de l'effet
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Pour l'effet
		ID3DBlob* pFXBlob = nullptr;

		DXEssayer(D3DCompileFromFile(L"Sprite1.fx", 0, 0, 0,
			"fx_5_0", 0, 0,
			&pFXBlob, 0),
			DXE_ERREURCREATION_FX);

		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(),
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

		const void* vsCodePtr = effectVSDesc2.pBytecode;
		const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

		DXEssayer(pD3DDevice->CreateInputLayout(
			CSommetSprite::layout,
			CSommetSprite::numElements,
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

	void CAfficheurSprite::Draw()
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext =
			pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetSprite);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride,
			&offset);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		ID3DX11EffectConstantBuffer* pCB =
			pEffet->GetConstantBufferByName("param");
		ID3DX11EffectShaderResourceVariable* variableTexture;
		variableTexture =
			pEffet->GetVariableByName("textureEntree")->AsShaderResource();

		pDispositif->ActiverMelangeAlpha();

		// Faire le rendu de tous nos billboards
		for (auto& etoile : tabEtoiles)
		{
			// Initialiser et sélectionner les «constantes» du VS - dans le monde cette fois.
			const XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
			const XMMATRIX matWorldViewProj = XMMatrixTranspose(etoile->matPosDim * viewProj);
			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &matWorldViewProj, 0, 0);

			pCB->SetConstantBuffer(pConstantBuffer);

			// Activation de la texture
			variableTexture->SetResource(etoile->pTextureD3D);

			pPasse->Apply(0, pImmediateContext);

			// **** Rendu de l'objet
			pImmediateContext->Draw(6, 0);
		}

		if (!pauseStatus)
		{
			// Faire le rendu de tous nos sprites
			for (auto& sprite : tabSprites)
			{
				// Initialiser et sélectionner les «constantes» de l'effet
				ShadersParams sp;
				sp.matWVP = XMMatrixTranspose(sprite->matPosDim);
				pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
					&sp, 0, 0);

				pCB->SetConstantBuffer(pConstantBuffer);

				// Activation de la texture
				variableTexture->SetResource(sprite->pTextureD3D);

				pPasse->Apply(0, pImmediateContext);

				// **** Rendu de l'objet
				pImmediateContext->Draw(6, 0);
			}

			for (auto& sprite : tabUISprite)
			{
				if (sprite->displayed)
				{
					// Initialiser et sélectionner les «constantes» de l'effet
					ShadersParams sp;
					sp.matWVP = XMMatrixTranspose(sprite->matPosDim);
					pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
						&sp, 0, 0);

					pCB->SetConstantBuffer(pConstantBuffer);

					// Activation de la texture
					variableTexture->SetResource(sprite->pTextureD3D);

					pPasse->Apply(0, pImmediateContext);

					// **** Rendu de l'objet
					pImmediateContext->Draw(6, 0);
				}
			}
		}
		else
		{
			for (auto& sprite : tabPauseSprite)
			{	
				// Initialiser et sélectionner les «constantes» de l'effet
				ShadersParams sp;
				sp.matWVP = XMMatrixTranspose(sprite->matPosDim);
				pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
					&sp, 0, 0);

				pCB->SetConstantBuffer(pConstantBuffer);

				// Activation de la texture
				variableTexture->SetResource(sprite->pTextureD3D);

				pPasse->Apply(0, pImmediateContext);

				// **** Rendu de l'objet
				pImmediateContext->Draw(6, 0);

			}

		}

		pDispositif->DesactiverMelangeAlpha();
	}

	void CAfficheurSprite::DrawZone(int _zone)
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext =
			pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetSprite);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride,
			&offset);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		ID3DX11EffectConstantBuffer* pCB =
			pEffet->GetConstantBufferByName("param");
		ID3DX11EffectShaderResourceVariable* variableTexture;
		variableTexture =
			pEffet->GetVariableByName("textureEntree")->AsShaderResource();

		pDispositif->ActiverMelangeAlpha();

		// Faire le rendu de tous nos panneaux
		for (auto& sign : tabSigns[_zone])
		{
			// Initialiser et sélectionner les «constantes» du VS - dans le monde cette fois.
			const XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
			const XMMATRIX matWorldViewProj = XMMatrixTranspose(sign->matPosDim * viewProj);
			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &matWorldViewProj, 0, 0);

			pCB->SetConstantBuffer(pConstantBuffer);

			// Activation de la texture
			variableTexture->SetResource(sign->pTextureD3D);

			pPasse->Apply(0, pImmediateContext);

			// **** Rendu de l'objet
			pImmediateContext->Draw(6, 0);
		}

		// Faire le rendu de tous nos billboards
		for (auto& billboard : tabBillboards[_zone])
		{
			// Initialiser et sélectionner les «constantes» du VS - dans le monde cette fois.
			const XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
			const XMMATRIX matWorldViewProj = XMMatrixTranspose(billboard->matPosDim * viewProj);
			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &matWorldViewProj, 0, 0);

			pCB->SetConstantBuffer(pConstantBuffer);

			// Activation de la texture
			variableTexture->SetResource(billboard->pTextureD3D);

			pPasse->Apply(0, pImmediateContext);

			// **** Rendu de l'objet
			pImmediateContext->Draw(6, 0);
		}

		pDispositif->DesactiverMelangeAlpha();
	}

	void CAfficheurSprite::displayPauseSprite()
	{
		pauseStatus = true;
	}

	void CAfficheurSprite::hidePauseSprite()
	{
		pauseStatus = false;
	}

	void CAfficheurSprite::AjouterSprite(int _zone, const std::string& NomTexture,
		int _x, int _y,
		int _dx, int _dy)
	{
		float x, y, dx, dy;
		float posX, posY;
		float facteurX, facteurY;

		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();

		std::wstring ws(NomTexture.begin(), NomTexture.end());

		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
		pSprite->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

		// Obtenir les dimensions de la texture si _dx et _dy sont à 0;
		if (_dx == 0 && _dy == 0)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pSprite->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			dx = float(desc.Width);
			dy = float(desc.Height);
		}
		else
		{
			dx = float(_dx);
			dy = float(_dy);
		}

		// Dimension en facteur
		facteurX = dx * 2.0f / pDispositif->GetLargeur();
		facteurY = dy * 2.0f / pDispositif->GetHauteur();

		// Position en coordonnées logiques
		// 0,0 pixel = -1,1   
		x = float(_x);
		y = float(_y);

		posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);

		// On l'ajoute à notre vecteur
		tabSprites.push_back(std::move(pSprite));
	}

	void CAfficheurSprite::AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, int _x, int _y)
	{
		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();
		pSprite->pTextureD3D = pTexture;
		//pSprite->displayed = true;

		// Obtenir la dimension de la texture;
		ID3D11Resource* pResource;
		ID3D11Texture2D* pTextureInterface = 0;
		pSprite->pTextureD3D->GetResource(&pResource);
		pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		D3D11_TEXTURE2D_DESC desc;
		pTextureInterface->GetDesc(&desc);

		DXRelacher(pResource);
		DXRelacher(pTextureInterface);

		const float dx = float(desc.Width);
		const float dy = float(desc.Height);

		// Dimension en facteur
		const float facteurX = dx * 2.0f / pDispositif->GetLargeur();
		const float facteurY = dy * 2.0f / pDispositif->GetHauteur();

		// Position en coordonnées logiques
		// 0,0 pixel = -1,1   
		const float x = float(_x);
		const float y = float(_y);

		const float posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		const float posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);

		// On l'ajoute à notre vecteur
		tabSprites.push_back(std::move(pSprite));
	}

	void CAfficheurSprite::AjouterPanneau(int _zone, bool _isPortal, const std::string& NomTexture,
		const XMFLOAT3& _position, bool _followsCam,
		float _dx, float _dy)
	{
		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();

		std::wstring ws(NomTexture.begin(), NomTexture.end());

		std::unique_ptr<CPanneau> pPanneau = std::make_unique<CPanneau>();
		pPanneau->portal = _isPortal;
		pPanneau->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

		// Obtenir la dimension de la texture si _dx et _dy sont à 0;
		if (_dx == 0.0f && _dy == 0.0f)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pPanneau->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			pPanneau->dimension.x = float(desc.Width);
			pPanneau->dimension.y = float(desc.Height);

			// Dimension en facteur
			pPanneau->dimension.x = pPanneau->dimension.x * 2.0f / pDispositif->GetLargeur();
			pPanneau->dimension.y = pPanneau->dimension.y * 2.0f / pDispositif->GetHauteur();
		}
		else
		{
			pPanneau->dimension.x = float(_dx);
			pPanneau->dimension.y = float(_dy);
		}

		// Position en coordonnées du monde
		const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
		pPanneau->position = _position;

		pPanneau->matPosDim = XMMatrixScaling(pPanneau->dimension.x,
			pPanneau->dimension.y, 1.0f) *
			XMMatrixTranslation(pPanneau->position.x,
				pPanneau->position.y, pPanneau->position.z) *
			viewProj;


		// On l'ajoute à notre vecteur
		if (_followsCam)
		{
			tabBillboards[_zone].push_back(std::move(pPanneau));
		}
		else
		{
			tabSigns[_zone].push_back(std::move(pPanneau));
		}
	}

	void CAfficheurSprite::AjouterEtoile(const std::string& NomTexture, const XMFLOAT3& _offset, float _dx, float _dy)
	{
		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();

		std::wstring ws(NomTexture.begin(), NomTexture.end());

		std::unique_ptr<CPanneau> pPanneau = std::make_unique<CPanneau>();
		pPanneau->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

		// Obtenir la dimension de la texture si _dx et _dy sont à 0;
		if (_dx == 0.0f && _dy == 0.0f)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pPanneau->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			pPanneau->dimension.x = float(desc.Width);
			pPanneau->dimension.y = float(desc.Height);

			// Dimension en facteur
			pPanneau->dimension.x = pPanneau->dimension.x * 2.0f / pDispositif->GetLargeur();
			pPanneau->dimension.y = pPanneau->dimension.y * 2.0f / pDispositif->GetHauteur();
		}
		else
		{
			pPanneau->dimension.x = float(_dx);
			pPanneau->dimension.y = float(_dy);
		}

		// Position en coordonnées du monde
		const XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
		auto playerPos = SceneManager::get().player->body->getGlobalPose().p;

		pPanneau->position = { _offset.x + playerPos.x, _offset.y + playerPos.y, _offset.z + playerPos.z };

		pPanneau->matPosDim = XMMatrixScaling(pPanneau->dimension.x,
			pPanneau->dimension.y, 1.0f) *
			XMMatrixTranslation(pPanneau->position.x,
				pPanneau->position.y, pPanneau->position.z) *
			viewProj;


		// On l'ajoute à notre vecteur
		tabEtoiles.push_back(std::move(pPanneau));
	}

	void CAfficheurSprite::AjouterPauseSprite(const std::string& NomTexture, int _x, int _y,
		int _dx, int _dy)
	{
		float x, y, dx, dy;
		float posX, posY;
		float facteurX, facteurY;

		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();

		std::wstring ws(NomTexture.begin(), NomTexture.end());

		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
		pSprite->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

		// Obtenir les dimensions de la texture si _dx et _dy sont à 0;
		if (_dx == 0 && _dy == 0)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pSprite->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			dx = float(desc.Width);
			dy = float(desc.Height);
		}
		else
		{
			dx = float(_dx);
			dy = float(_dy);
		}

		// Dimension en facteur
		facteurX = dx * 2.0f / pDispositif->GetLargeur();
		facteurY = dy * 2.0f / pDispositif->GetHauteur();

		// Position en coordonnées logiques
		// 0,0 pixel = -1,1   
		x = float(_x);
		y = float(_y);

		posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);

		// On l'ajoute à notre classe
		tabPauseSprite.push_back(std::move(pSprite));
	}

	void CAfficheurSprite::AjouterUISprite(const std::string& NomTexture, int _x, int _y, int _dx, int _dy, bool _displayed)
	{
		float x, y, dx, dy;
		float posX, posY;
		float facteurX, facteurY;

		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();

		std::wstring ws(NomTexture.begin(), NomTexture.end());

		std::unique_ptr<CUISprite> pSprite = std::make_unique<CUISprite>();;
		pSprite->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
		pSprite->displayed = _displayed;

		// Obtenir les dimensions de la texture si _dx et _dy sont à 0;
		if (_dx == 0 && _dy == 0)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pSprite->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			dx = float(desc.Width);
			dy = float(desc.Height);
		}
		else
		{
			dx = float(_dx);
			dy = float(_dy);
		}

		// Dimension en facteur
		facteurX = dx * 2.0f / pDispositif->GetLargeur();
		facteurY = dy * 2.0f / pDispositif->GetHauteur();

		// Position en coordonnées logiques
		// 0,0 pixel = -1,1   
		x = float(_x);
		y = float(_y);

		posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);

		// On l'ajoute à notre vecteur
		tabUISprite.push_back(std::move(pSprite));
	}

	void CAfficheurSprite::updateGauge(int _speed)
	{
		for_each(tabUISprite.begin() + 3, tabUISprite.begin() + 9, [](auto& sprite) {
			sprite->displayed = false;
			});
		switch (_speed / 30) 
		{
		case 0:
			tabUISprite[3]->displayed = true;
			break;
		case 1:
			tabUISprite[4]->displayed = true;
			break;
		case 2:
			tabUISprite[5]->displayed = true;
			break;
		case 3:
			tabUISprite[6]->displayed = true;
			break;
		case 4:
			tabUISprite[7]->displayed = true;
			break;
		default:
			tabUISprite[8]->displayed = true;
			break;
		}
	}

	// Methode anime custom pour faire tourner les panneaux en accord avec la camera
	void CAfficheurSprite::Anime(float) {

		for (auto& sprite : tabEtoiles)
		{
			XMFLOAT3 posBoard = sprite->position;
			// keeping the stars well placed
			//XMFLOAT3 starPos{ sprite->position.x, sprite->position.y, sprite->position.z };
			PxVec3 plrPosPX = SceneManager::get().player->body->getGlobalPose().p;
			XMFLOAT3 playerPos = { plrPosPX.x, plrPosPX.y, plrPosPX.z };

			// check for x position
			if (posBoard.x < playerPos.x - starAreaOffsetFromCenter)
			{
				posBoard.x = posBoard.x + 2 * starAreaOffsetFromCenter;
			}
			else if (posBoard.x > playerPos.x + starAreaOffsetFromCenter)
			{
				posBoard.x = posBoard.x - 2 * starAreaOffsetFromCenter;
			}
			
			// check for y position
			if (posBoard.y < playerPos.y - starAreaOffsetFromCenter)
			{
				posBoard.y = posBoard.y + 2 * starAreaOffsetFromCenter;
			}
			else if (posBoard.y > playerPos.y + starAreaOffsetFromCenter)
			{
				posBoard.y = posBoard.y - 2 * starAreaOffsetFromCenter;
			}

			// check for z position
			if (posBoard.z < playerPos.z - starAreaOffsetFromCenter)
			{
				posBoard.z = posBoard.z + 2 * starAreaOffsetFromCenter;
			}
			else if (posBoard.z > playerPos.z + starAreaOffsetFromCenter)
			{
				posBoard.z = posBoard.z - 2 * starAreaOffsetFromCenter;
			}

			auto cam = CMoteurWindows::GetInstance().GetFreeCamera();
			XMVECTOR camPos = cam.getPosition();

			PxVec3 vecDir = { posBoard.x - camPos.vector4_f32[0], posBoard.y - camPos.vector4_f32[1], posBoard.z - camPos.vector4_f32[2] };

			vecDir = vecDir.getNormalized();

			float angleY = -atan2(vecDir.z, vecDir.x) - XM_PIDIV2;
			PxQuat quatY = PxQuat(angleY, { 0.0f, 1.0f, 0.0f });

			float angleZ = -atan2(vecDir.y, 1.0f);
			PxQuat quatZ = PxQuat(angleZ, { -1.0f, 0.0f, 0.0f });

			PxQuat quat = quatY * quatZ;

			XMMATRIX rotTmp;

			//auto posTmp = XMMatrixTranslation(posBoard.x, posBoard.y, posBoard.z);
			rotTmp = XMMatrixRotationQuaternion(XMVectorSet(quat.x, quat.y, quat.z, quat.w));

			auto tmpPosMatrix = XMMatrixScaling(sprite->dimension.x,
				sprite->dimension.y, 1.0f) * rotTmp *
				XMMatrixTranslation(posBoard.x,
					posBoard.y, posBoard.z);

			sprite->position = posBoard;
			sprite.get()->matPosDim = tmpPosMatrix;
		}
	}

	// Methode anime custom pour faire tourner les panneaux en accord avec la camera
	void CAfficheurSprite::AnimeZone(int _zone, float tempsEcoule) {

		if (tabBillboards[_zone].empty())
			return;

		// Animer tous les billboards
		for (auto& sprite : tabBillboards[_zone])
		{
			PxTransform transformPlayer = SceneManager::get().player->body->getGlobalPose();
			CPanneau* pBoard = reinterpret_cast<CPanneau*>(sprite.get());
			XMFLOAT3 posBoard = pBoard->position;

			PxVec3 vecDir = { posBoard.x - transformPlayer.p.x, posBoard.y - transformPlayer.p.y, posBoard.z - transformPlayer.p.z };

			vecDir = vecDir.getNormalized();

			float angleY = -atan2(vecDir.z, vecDir.x) - XM_PIDIV2;
			PxQuat quatY = PxQuat(angleY, { 0.0f, 1.0f, 0.0f });

			float angleZ = -atan2(vecDir.y, 1.0f);
			PxQuat quatZ = PxQuat(angleZ, { -1.0f, 0.0f, 0.0f });

			PxQuat quat = quatY * quatZ;

			XMMATRIX rotTmp;

			//auto posTmp = XMMatrixTranslation(posBoard.x, posBoard.y, posBoard.z);

			if (pBoard->portal)
			{
				++pBoard->rotation;
				quat = quat * PxQuat((XM_PIDIV4/12) * (float)pBoard->rotation, { 0.0f, 0.0f, 1.0f });
				rotTmp = XMMatrixRotationQuaternion(XMVectorSet(quat.x, quat.y, quat.z, quat.w));
			}
			else
			{
				rotTmp = XMMatrixRotationQuaternion(XMVectorSet(quat.x, quat.y, quat.z, quat.w));
			}

			auto tmpPosMatrix = XMMatrixScaling(pBoard->dimension.x,
				pBoard->dimension.y, 1.0f) * rotTmp *
				XMMatrixTranslation(pBoard->position.x,
					pBoard->position.y, pBoard->position.z);

			sprite.get()->matPosDim = tmpPosMatrix;
		}
	}

} // namespace PM3D
