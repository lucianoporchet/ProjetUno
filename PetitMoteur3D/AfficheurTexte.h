#pragma once

#include <Gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace PM3D
{
class CDispositifD3D11;

class CAfficheurTexte
{
public:
	CAfficheurTexte(CDispositifD3D11* pDispositif, int largeur, int hauteur, Gdiplus::Font* pPolice);
	~CAfficheurTexte();
	void Ecrire(const std::wstring& s);
	ID3D11ShaderResourceView* GetTextureView() { return pTextureView; }

	static void Init();
	static void Close();

private:
	UINT TexWidth;
	UINT TexHeight;

	ID3D11Texture2D *pTexture;
	IDXGISurface1* pSurface;
	ID3D11ShaderResourceView* pTextureView;
	CDispositifD3D11* pDispo;

	Gdiplus::Font* pFont;
	std::unique_ptr<Gdiplus::Bitmap> pCharBitmap;
	std::unique_ptr<Gdiplus::Graphics> pCharGraphics;
	std::unique_ptr<Gdiplus::SolidBrush> pBlackBrush;

	// Variables statiques pour GDI+
	static	ULONG_PTR token;
};

} // namespace PM3D
