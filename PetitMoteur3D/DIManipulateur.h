#pragma once
#include "Horloge.h"

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
	void setSourisPosition(int x, int y);
	void setPauseMenu(bool);
	bool getIsPauseStatus();
	bool hasBeenEnoughTimeSinceLastPause();

	const DIMOUSESTATE& EtatSouris() const { return mouseState; }

private:
	IDirectInput8 * pDirectInput;
	IDirectInputDevice8* pClavier;
	IDirectInputDevice8* pSouris;
	IDirectInputDevice8* pJoystick;

	static bool bDejaInit;
	bool isPause = false;

	char tamponClavier[256];
	DIMOUSESTATE mouseState;
	Horloge horloge;
	int64_t lastPaused;
};

} // namespace PM3D
