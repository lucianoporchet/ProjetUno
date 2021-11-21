#pragma once

namespace PM3D
{

enum INFODISPO_TYPE
{
	ADAPTATEUR_COURANT
};

class CInfoDispositif
{
public:

	explicit CInfoDispositif(int NoAdaptateur);
	explicit CInfoDispositif(const DXGI_MODE_DESC& modeDesc);

	void GetDesc(DXGI_MODE_DESC& modeDesc) const { modeDesc = mode; }

private:
	bool valide;
	int largeur;
	int hauteur;
	int memoire;
	wchar_t nomcarte[128];
	DXGI_MODE_DESC mode;
};

} // namespace PM3D
