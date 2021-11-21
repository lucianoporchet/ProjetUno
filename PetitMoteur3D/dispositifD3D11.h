
#pragma once

#include "dispositif.h"

namespace PM3D
{
	//
	//  Classe CDispositifD3D11
	//
	//  BUT : 	Classe permettant d'implanter un dispositif de rendu 
	// 			Direct3D
	//
	class CDispositifD3D11 final : public CDispositif
	{
	public:
		CDispositifD3D11(const CDS_MODE cdsMode, const HWND hWnd);
		virtual ~CDispositifD3D11();

		virtual void PresentSpecific() override;

		// Fonction d'accès aux membres protégés
		ID3D11Device*           GetD3DDevice() { return pD3DDevice; }
		ID3D11DeviceContext*    GetImmediateContext() { return pImmediateContext; }
		IDXGISwapChain*         GetSwapChain() { return pSwapChain; }
		ID3D11RenderTargetView* GetRenderTargetView() { return pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() { return pDepthStencilView; }

		void ActiverMelangeAlpha();
		void DesactiverMelangeAlpha();

		void SetViewPortDimension(float largeur_in, float hauteur_in);
		void ResetViewPortDimension();
		void SetNormalRSState();

		void SetRenderTargetView(ID3D11RenderTargetView* pRenderTargetView_in,
			ID3D11DepthStencilView* pDepthStencilView_in);

	private:
		void InitDepthBuffer();
		void InitBlendStates();

		ID3D11Device*           pD3DDevice;
		ID3D11DeviceContext*    pImmediateContext;
		IDXGISwapChain*         pSwapChain;
		ID3D11RenderTargetView* pRenderTargetView;

		// Pour le tampon de profondeur
		ID3D11Texture2D* pDepthTexture;
		ID3D11DepthStencilView* pDepthStencilView;

		// Variables d'état
		ID3D11RasterizerState* mSolidCullBackRS;

		// Pour le mélange alpha (transparence)
		ID3D11BlendState* alphaBlendEnable;
		ID3D11BlendState* alphaBlendDisable;
	};

} // namespace PM3D
