#include "StdAfx.h"
#include "resource.h"

#include "MoteurWindows.h"

namespace PM3D
{
	bool fenetre = false;
HINSTANCE CMoteurWindows::hAppInstance;	// handle Windows de l'instance actuelle de l'application

										//   FONCTION�: SetWindowsAppInstance(HANDLE, int)
										//
										//   BUT�: Prend en note le handle de l'instance
										//
void CMoteurWindows::SetWindowsAppInstance(HINSTANCE hInstance)
{
	hAppInstance = hInstance; // Stocke le handle d'instance de l'application, plusieurs fonctions sp�cifiques en auront besoin
}

//
//  FONCTION�: MyRegisterClass()
//
//  BUT�: inscrit la classe de fen�tre.
//
//  COMMENTAIRES�:
//
//    Cette fonction et son utilisation sont n�cessaires uniquement si vous souhaitez que ce code
//    soit compatible avec les syst�mes Win32 avant la fonction 'RegisterClassEx'
//    qui a �t� ajout�e � Windows�95. Il est important d'appeler cette fonction
//    afin que l'application dispose des petites ic�nes correctes qui lui sont
//    associ�es.
//
//	  FJ:  Copie-coller de la fonction MyRegisterClass g�n�r�e automatiquement
//
ATOM CMoteurWindows::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PETITMOTEUR3D));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_PETITMOTEUR3D);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//   FONCTION�: InitAppInstance()
//
//   BUT�:  Effectue des op�rations d'initialisation de l'application
//
//   COMMENTAIRES�:
//
//        Dans cette fonction, nous enregistrons le handle de l'instance dans une variable globale, puis
//        cr�ons et affichons la fen�tre principale du programme.
//
bool CMoteurWindows::InitAppInstance()
{
	TCHAR szTitle[MAX_LOADSTRING];					// Le texte de la barre de titre

													// Initialise les cha�nes globales
	LoadString(hAppInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hAppInstance, IDC_PETITMOTEUR3D, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hAppInstance);

	hMainWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hAppInstance, nullptr);

	if (!hMainWnd)
	{
		return false;
	}

	hAccelTable = LoadAccelerators(hAppInstance, MAKEINTRESOURCE(IDC_PETITMOTEUR3D));

	return true;
}

int CMoteurWindows::Show()
{
	ShowWindow(hMainWnd, SW_SHOWNORMAL);
	UpdateWindow(hMainWnd);

	return 0;
}

int CMoteurWindows::InitialisationsSpecific()
{
	// Initialisations  de l'application;
	InitAppInstance();
	Show();

	// Initialisation de DirectInput
	GestionnaireDeSaisie.Init(hAppInstance, hMainWnd);

	return 0;
}

//
//   FONCTION : RunSpecific 
//
//   BUT :	�l�ments internes de la boucle de l'application (Boucle message)
//
//   NOTES:		
//		Il s'agit d'une version modifi�e sp�cifiquement pour nos besoins des 
//      �l�ments de la boucle message de Windows
//
bool CMoteurWindows::RunSpecific()
{
	MSG msg;
	bool bBoucle = true;

	// Y-a-t'il un message Windows � traiter?
	if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (fenetre) {
			GestionnaireDeSaisie.Aquire(msg.hwnd);
		}
		else {
			GestionnaireDeSaisie.Unaquire();
		}
		// Est-ce un message de fermeture ?
		if (msg.message == WM_QUIT) bBoucle = false;

		// distribuer le message
		if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return bBoucle;
}

//
//  FONCTION : GetTimeSpecific 
//
//  BUT :	Fonction responsable d'obtenir un temps.
//
//
int64_t CMoteurWindows::GetTimeSpecific() const
{
	return m_Horloge.GetTimeCount();
}

double CMoteurWindows::GetTimeIntervalsInSec(int64_t start, int64_t stop) const
{
	return m_Horloge.GetTimeBetweenCounts(start, stop);
}

//  FONCTION : CreationDispositifSpecific 
//
//  BUT :	Fonction responsable de cr�er le 
//			dispositif selon certains param�tres
//  NOTES:		
//		 CDS_MODE: 	CDS_FENETRE 		application fen�tr�e
//					CDS_PLEIN_ECRAN 	application plein �cran
//
CDispositifD3D11* CMoteurWindows::CreationDispositifSpecific(const CDS_MODE cdsMode)
{
	return new CDispositifD3D11(cdsMode, hMainWnd);
}

void CMoteurWindows::BeginRenderSceneSpecific()
{
	ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
	ID3D11RenderTargetView* pRenderTargetView = pDispositif->GetRenderTargetView();


	// On efface la surface de rendu
	float Couleur[4] = { 0.0f, 0.5f, 0.0f, 1.0f };  //  RGBA - Vert pour le moment
	pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);

	// On r�-initialise le tampon de profondeur
	ID3D11DepthStencilView* pDepthStencilView = pDispositif->GetDepthStencilView();
	pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CMoteurWindows::EndRenderSceneSpecific()
{
}

//  FONCTION�: WndProc(HWND, unsigned, WORD, LONG)
//
//  BUT�:  traite les messages pour la fen�tre principale.
//
//  WM_COMMAND	- traite le menu de l'application
//  WM_PAINT	- dessine la fen�tre principale
//  WM_DESTROY	- g�n�re un message d'arr�t et retourne
//
//
LRESULT CALLBACK CMoteurWindows::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_KILLFOCUS:
		fenetre = false;
		break;
	case WM_SETFOCUS:
		fenetre = true;
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Analyse les s�lections de menu�:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hAppInstance, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// �vitez d'ajouter du code ici...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Gestionnaire de messages pour la bo�te de dialogue � propos de.
INT_PTR CALLBACK CMoteurWindows::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

} // namespace PM3D
