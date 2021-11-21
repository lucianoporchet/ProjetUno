#pragma once

namespace PM3D
{

class CDIManipulateur
{
public:
	CDIManipulateur();
	~CDIManipulateur();

	bool CDIManipulateur::Init(HINSTANCE hInstance, HWND hWnd);
	void StatutClavier();
	bool ToucheAppuyee(UINT touche) const;
	void SaisirEtatSouris();

	void Aquire(HWND hWnd);
	void Unaquire();

	const DIMOUSESTATE& EtatSouris() const { return mouseState; }

private:
	IDirectInput8 * pDirectInput;
	IDirectInputDevice8* pClavier;
	IDirectInputDevice8* pSouris;
	IDirectInputDevice8* pJoystick;

	static bool bDejaInit;

	char tamponClavier[256];
	DIMOUSESTATE mouseState;
};

} // namespace PM3D
