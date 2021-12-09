#include "stdafx.h"
#include "Terrain.h"


#include "sommetbloc.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"


namespace PM3D
{

	struct ShadersParams
	{
		XMMATRIX matWorldViewProj; // la matrice totale
		XMMATRIX matWorld; // matrice de transformation dans le monde
		XMVECTOR vLumiere; // la position de la source d’éclairage (Point)
		XMVECTOR vCamera; // la position de la caméra
		XMVECTOR vAEcl; // la valeur ambiante de l’éclairage
		XMVECTOR vAMat; // la valeur ambiante du matériau
		XMVECTOR vDEcl; // la valeur diffuse de l’éclairage
		XMVECTOR vDMat; // la valeur diffuse du matériau
	};


	//  FONCTION : CBloc, constructeur paramètré 
	//  BUT :	Constructeur d'une classe de bloc
	//  PARAMÈTRES:
	//		dx, dy, dz:	dimension en x, y, et z
	//		pDispositif: pointeur sur notre objet dispositif
	CTerrain::CTerrain(std::string indexFile, std::string sommetsFile,
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
	
		freeCam = true;
		std::vector<std::future<void>> futures;

		futures.push_back(std::async(std::launch::async, readSommets, sommetsFile, &v_sommets, &widthT));
		futures.push_back(std::async(std::launch::async, readIndex, indexFile, &v_index));
		futures[0].wait();
		futures[1].wait();

		//CMoteurWindows::GetInstance().GetLevelCamera().setSommets(v_sommets, widthT);
		matWorld = XMMatrixRotationX(-XM_PI / 2);
		// Création du vertex buffer et copie des sommets
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CSommetBloc) * (UINT)v_sommets.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &v_sommets[0];

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(unsigned int) * (UINT)v_index.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &v_index[0];

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);
		

		// Inititalisation des shaders
		InitEffet();
	}

	
	void CTerrain::readSommets(std::string s, std::vector<CSommetBloc>* v, float* width) {
		std::ifstream in{ s };
		float x, y, z, nx, ny, nz;
		while (in >> x >> y >> z >> nx >> ny >> nz)
			v->push_back(CSommetBloc{ XMFLOAT3{ x, y, z }, XMFLOAT3{nx, ny, nz} });
		*width = x+1;
		in.close();
	}

	void CTerrain::readIndex(std::string ix, std::vector<unsigned int>* v) {
		std::ifstream in2{ ix };
		unsigned int i;
		while (in2 >> i)
			v->push_back(i);
		in2.close();
	}

	void CTerrain::Anime(float)
	{
		//rotation = rotation + ((XM_PI * 2.0f) / 3.0f * tempsEcoule);

		//// modifier la matrice de l'objet bloc
		//matWorld = XMMatrixRotationX(rotation);

		// Pour les mouvements, nous utilisons le gestionnaire de saisie
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie =
			rMoteur.GetGestionnaireDeSaisie();
		//// Vérifier l’état de la touche gauche
		//if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT))
		//{
		//	rotation = rotation + ((XM_PI * 2.0f) / 7.0f * tempsEcoule);
		//	// modifier la matrice de l’objet X
		//	matWorld = XMMatrixRotationY(rotation);
		//}
		//// Vérifier l’état de la touche droite
		//if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT))
		//{
		//	rotation = rotation - ((XM_PI * 2.0f) / 7.0f * tempsEcoule);
		//	// modifier la matrice de l’objet X
		//	matWorld = XMMatrixRotationY(rotation);
		//}
		// ******** POUR LA SOURIS ************
	//// Vérifier si déplacement vers la gauche
	//	if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) &&
	//		(rGestionnaireDeSaisie.EtatSouris().lX < 0))
	//	{
	//		rotation = rotation + ((XM_PI * 2.0f) / 4.0f * tempsEcoule);
	//		// modifier la matrice de l’objet X
	//		matWorld = XMMatrixRotationY(rotation);
	//	}
	//	// Vérifier si déplacement vers la droite
	//	if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) &&
	//		(rGestionnaireDeSaisie.EtatSouris().lX > 0))
	//	{
	//		rotation = rotation - ((XM_PI * 2.0f) / 4.0f * tempsEcoule);
	//		// modifier la matrice de l’objet X
	//		matWorld = XMMatrixRotationY(rotation);
	//	}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_1))
		{
			freeCam = true;
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_2))
		{
			freeCam = false;
		}

	}

	void CTerrain::Draw()
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
		XMVECTOR camPos;
		if (freeCam) {
			viewProj = *moteur.GetFreeCamera().pMatViewProj;
			camPos = moteur.GetFreeCamera().getPosition();
		}
		else{
			//viewProj = *moteur.GetLevelCamera().pMatViewProj;
			//camPos = moteur.GetLevelCamera().getPosition();
		}

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);
		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f);
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		sp.vAMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
		// Nous n’avons qu’un seul CBuffer
		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
		pCB->SetConstantBuffer(pConstantBuffer);
		// **** Rendu de l’objet
		pPasse->Apply(0, pImmediateContext);
		pImmediateContext->DrawIndexed((UINT)v_index.size(), 0, 0);
	}

	CTerrain::~CTerrain()
	{
		DXRelacher(pPixelShader);
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexShader);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}


	void CTerrain::InitEffet() {
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
		DXEssayer(D3DCompileFromFile(L"MiniPhong.fx", 0, 0, 0,
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

	}

} // namespace PM3D
