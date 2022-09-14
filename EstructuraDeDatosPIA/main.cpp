#include <Windows.h>
#include "resource.h"

using namespace std;

HWND ghDlg = 0;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PrincipalProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
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
			case ID_USUARIO_INICIODESESION:
				DialogBox(NULL, MAKEINTRESOURCE(IDD_INICIO_SESION), hWnd, (DLGPROC)LoginProc);
				break;


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

		} break;
		case WM_CLOSE:
		{
			EndDialog(hWnd, 0);
			return true;
		}

	}


	return false;
}