#include <Windows.h>
#include "resource.h"

using namespace std;

HWND ghDlg = 0;
HINSTANCE hInstGlobal;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PrincipalProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistroProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK VacunasAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ClientesAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	hInstGlobal = hInstance;
	ghDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PANTALLA_INICIAL), 0, (DLGPROC)PrincipalProc);
	ShowWindow(ghDlg, nCmdShow);
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (GetMessage(&msg, 0, 0, 0)) {
		if (ghDlg == 0 || !IsDialogMessage(ghDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL CALLBACK PrincipalProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{

		} break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
			case ID_DATOS_GUARDARDATOS:
				break;
			case ID_DATOS_CARGARDATOS:
				break;
			case ID_USUARIO_INICIODESESION:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_USUARIO_INICIO_SESION), hWnd, (DLGPROC)LoginProc);
				break;
			case ID_USUARIO_REGISTRODEUSUARIO:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_USUARIO_REGISTRO), hWnd, (DLGPROC)RegistroProc);
				break;
			case ID_USUARIO_CERRARSESION:
				break;

			case ID_VACUNAS_ALTADEVACUNA:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_VACUNAS_ALTA), hWnd, (DLGPROC)VacunasAltaProc);
				break;
			case ID_CLIENTES_ALTADECLIENTE:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_CLIENTES_ALTA), hWnd, (DLGPROC)ClientesAltaProc);
				break;
			}


		} break;
		case WM_CLOSE:
		{
			DestroyWindow(ghDlg);
			return true;
		}
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			return true;
		}
	
	}


	return false;
}

BOOL CALLBACK LoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{

		} break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
				case BTN_ACEPTAR_USUARIO_INICIO:
				break;

				case BTN_CERRAR_USUARIO_INICIO:
					EndDialog(hWnd, 0);
					return true;
			}

		} break;
		case WM_CLOSE:
		{
			EndDialog(hWnd, 0);
			return true;
		}

	}


	return false;
}

BOOL CALLBACK RegistroProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{

	} break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
			case BTN_ACEPTAR_USUARIO_REGISTRO:
				break;
			case BTN_CERRAR_USUARIO_REGISTRO:
				EndDialog(hWnd, 0);
				return true;
		}

	} break;
	case WM_CLOSE:
	{
		EndDialog(hWnd, 0);
		return true;
	}

	}


	return false;
}

BOOL CALLBACK VacunasAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{

	} break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
			case BTN_ACEPTAR_VACUNAS_ALTA:
				break;
			case BTN_CERRAR_VACUNAS_ALTA:
				EndDialog(hWnd, 0);
				return true;
		}

	} break;
	case WM_CLOSE:
	{
		EndDialog(hWnd, 0);
		return true;
	}

	}


	return false;
}

BOOL CALLBACK ClientesAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{

	} break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
			case BTN_ACEPTAR_CLIENTES_ALTA:
				break;
			case BTN_CERRAR_CLIENTES_ALTA:
				EndDialog(hWnd, 0);
				return true;
		}

	} break;
	case WM_CLOSE:
	{
		EndDialog(hWnd, 0);
		return true;
	}

	}


	return false;
}

