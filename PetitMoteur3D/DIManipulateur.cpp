#include "stdafx.h"
#include "DIManipulateur.h"
#include "util.h"
#include "resource.h"

namespace PM3D
{

bool CDIManipulateur::bDejaInit = false;

CDIManipulateur::CDIManipulateur()
	: pDirectInput(nullptr)
	, pClavier(nullptr)
	, pSouris(nullptr)
	, pJoystick(nullptr)
{
}

CDIManipulateur::~CDIManipulateur()
{
	if (pClavier)
	{
		pClavier->Unacquire();
		pClavier->Release();
		pClavier = nullptr;
	}

	if (pSouris)
	{
		pSouris->Unacquire();
		pSouris->Release();
		pSouris = nullptr;
	}

	if (pJoystick)
	{
		pJoystick->Unacquire();
		pJoystick->Release();
		pJoystick = nullptr;
	}

	if (pDirectInput)
	{
		pDirectInput->Release();
		pClavier = nullptr;
	}

}

bool CDIManipulateur::Init(HINSTANCE hInstance, HWND hWnd)
{
	// Un seul objet DirectInput est permis
	if (!bDejaInit)
	{
		// Objet DirectInput
		DXEssayer(DirectInput8Create(hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&pDirectInput,
			nullptr), ERREUR_CREATION_DIRECTINPUT);

		// Objet Clavier
		DXEssayer(pDirectInput->CreateDevice(GUID_SysKeyboard,
			&pClavier,
			nullptr),
			ERREUR_CREATION_CLAVIER);

		DXEssayer(pClavier->SetDataFormat(&c_dfDIKeyboard),
			ERREUR_CREATION_FORMATCLAVIER);

		pClavier->SetCooperativeLevel(hWnd,
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		pClavier->Acquire();

		// Objet Souris
		DXEssayer(pDirectInput->CreateDevice(GUID_SysMouse, &pSouris, nullptr),
			ERREUR_CREATION_SOURIS);

		DXEssayer(pSouris->SetDataFormat(&c_dfDIMouse), ERREUR_CREATION_FORMATSOURIS);

		pSouris->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		ShowCursor(FALSE); 
		//SetCursorPos( / 2, CLIENT_HEIGHT / 2);

		pSouris->Acquire();

		// Objet Joystick

		bDejaInit = true;
	}

	return true;
}

void CDIManipulateur::StatutClavier()
{
	assert(bDejaInit);
	pClavier->GetDeviceState(sizeof(tamponClavier), (void*)& tamponClavier);
}

bool CDIManipulateur::ToucheAppuyee(UINT touche) const
{
	assert(bDejaInit);
	return (tamponClavier[touche] & 0x80);
}

void CDIManipulateur::SaisirEtatSouris()
{
	assert(bDejaInit);
	pSouris->GetDeviceState(sizeof(mouseState), (void*)&mouseState);
}

void CDIManipulateur::Aquire(HWND hWnd)
{
	if (bDejaInit) {
		pClavier->SetCooperativeLevel(hWnd,
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		pClavier->Acquire();

		pSouris->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		pSouris->Acquire();
	}


}

void CDIManipulateur::Unaquire()
{
	if (bDejaInit) {

		pClavier->Unacquire();
		pSouris->Unacquire();
	}

}

void CDIManipulateur::setSourisPosition(int x, int y)
{
	SetCursorPos(x, y);
}

void CDIManipulateur::setPauseMenu(bool toShow) {
	ShowCursor(toShow);
	isPause = toShow;
}

bool CDIManipulateur::getIsPauseStatus()
{
	return isPause;
}

} // namespace PM3D
