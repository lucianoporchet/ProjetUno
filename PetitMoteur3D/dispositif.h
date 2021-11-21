#pragma once

namespace PM3D
{
// Constantes pour mode fenêtré ou plein écran
enum CDS_MODE
{
	CDS_FENETRE,
	CDS_PLEIN_ECRAN
};

//
//  Classe : CDispositif
//
//  BUT : 	Classe servant à construire un objet Dispositif 
//				qui implantera les aspects "génériques" du dispositif de 
//				rendu
//
class CDispositif
{
public:
	virtual ~CDispositif() = default;

	virtual void Present();
	virtual void PresentSpecific() = 0;

	uint32_t GetLargeur() const { return largeurEcran; }
	uint32_t GetHauteur() const { return hauteurEcran; }

protected:
	uint32_t largeurEcran;
	uint32_t hauteurEcran;
};

} // namespace PM3D
