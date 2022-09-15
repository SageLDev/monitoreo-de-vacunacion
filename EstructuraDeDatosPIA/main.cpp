#include <Windows.h>
#include "resource.h"
#include <fstream>

using namespace std;



namespace Estructuras {
	typedef struct Usuario {
		wchar_t usuario[200];
		wchar_t contrasena[200];
		wchar_t apellidoPaterno[200];
		wchar_t apellidoMaterno[200];
		wchar_t nombres[500];
		Usuario* siguiente;
		Usuario* anterior;
	};

	typedef struct Vacuna {
		wchar_t tipo[200];
		wchar_t marca[200];
		int dosis;
		int clave;
		float precio;
		wchar_t descripcion;
		Vacuna* siguiente;
		Vacuna* anterior;
	};

	Usuario* uhead = nullptr;
	Usuario* usuarioActual;

	Vacuna* vhead = nullptr;
	Vacuna* vacunaActual;


	class Validaciones {
	public:
		void Registrar(HWND hWnd) {
			Usuario* ultimo = uhead;
			Usuario* nuevo = new Usuario;
			SendMessage(GetDlgItem(hWnd, REGISTRO_USUARIO), WM_GETTEXT, sizeof(nuevo->usuario) / sizeof(nuevo->usuario[0]), (LPARAM)nuevo->usuario);
			SendMessage(GetDlgItem(hWnd, REGISTRO_CONTRA), WM_GETTEXT, sizeof(nuevo->contrasena) / sizeof(nuevo->contrasena[0]), (LPARAM)nuevo->contrasena);
			SendMessage(GetDlgItem(hWnd, REGISTRO_APELLIDO_PATERNO), WM_GETTEXT, sizeof(nuevo->apellidoPaterno) / sizeof(nuevo->apellidoPaterno[0]), (LPARAM)nuevo->apellidoPaterno);
			SendMessage(GetDlgItem(hWnd, REGISTRO_APELLIDO_MATERNO), WM_GETTEXT, sizeof(nuevo->apellidoMaterno) / sizeof(nuevo->apellidoMaterno[0]), (LPARAM)nuevo->apellidoMaterno);
			SendMessage(GetDlgItem(hWnd, REGISTRO_NOMBRE), WM_GETTEXT, sizeof(nuevo->nombres) / sizeof(nuevo->nombres[0]), (LPARAM)nuevo->nombres);
			nuevo->siguiente = nullptr;
			nuevo->anterior = nullptr;
			if (uhead == nullptr) {
				uhead = nuevo;
			}
			else {
				while (ultimo->siguiente != nullptr)
				{
					ultimo = ultimo->siguiente;
				}
				ultimo->siguiente = nuevo;
				nuevo->anterior = ultimo;
			}

			EndDialog(hWnd, 0);
		}

		void Login(HWND hWnd) {
			wchar_t usuarioIngresado[200];
			wchar_t contrasenaIngresada[200];
			SendMessage(GetDlgItem(hWnd, INICIO_USUARIO), WM_GETTEXT, sizeof(usuarioIngresado) / sizeof(usuarioIngresado[0]), (LPARAM)usuarioIngresado);
			SendMessage(GetDlgItem(hWnd, INICIO_CONTRA), WM_GETTEXT, sizeof(contrasenaIngresada) / sizeof(contrasenaIngresada[0]), (LPARAM)contrasenaIngresada);

			Usuario* ptr = uhead;
			bool encontrado;
			if (ptr == nullptr) {
				MessageBox(hWnd, L"Favor de registrarse o cargar los datos del programa", L"No hay usuarios registrados", MB_OK);
			}
			else
			{
				while (ptr != nullptr) {
					if (wcscmp(ptr->usuario, usuarioIngresado) == 0 && wcscmp(ptr->contrasena, contrasenaIngresada) == 0) {
						encontrado = true;
						usuarioActual = ptr;
						EndDialog(hWnd, 0);
						return;
					}
					else {
						encontrado = false;
					}
					ptr = ptr->siguiente;
				}
				if (encontrado == false) {
					MessageBox(hWnd, L"No encontramos un usuario con esas credenciales.", L"Usuario no encontrado", MB_OK);
				}

			}
		}

		void GuardarUsuarios(HWND hWnd) {
			OPENFILENAME openfilenameGU;
			wchar_t guardarArchivoU[500];
			ZeroMemory(&openfilenameGU, sizeof(openfilenameGU));
			openfilenameGU.lStructSize = sizeof(openfilenameGU);
			openfilenameGU.hwndOwner = hWnd;
			openfilenameGU.lpstrFile = guardarArchivoU;
			openfilenameGU.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilenameGU.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilenameGU.lpstrFile[0] = NULL;
			openfilenameGU.nMaxFile = sizeof(guardarArchivoU);
			openfilenameGU.nFilterIndex = 0;
			if (GetSaveFileName(&openfilenameGU) == true) {
				ofstream escribir;
				escribir.open(guardarArchivoU, ios::out | ios::binary | ios::trunc);
				if (escribir.is_open()) {

					if (uhead != NULL) {
						Usuario* uptr = uhead;
						while (uptr->siguiente != NULL)
						{
							escribir.write(reinterpret_cast<char*>(uptr), sizeof(Usuario));
							uptr = uptr->siguiente;
						}
						escribir.write(reinterpret_cast<char*>(uptr), sizeof(Usuario));
					}
					escribir.close();
				}
			}
		}
		void CargarUsuarios(HWND hWnd) {
			OPENFILENAME openfilenameCU;
			wchar_t abrirArchivoU[500];
			ZeroMemory(&openfilenameCU, sizeof(openfilenameCU));
			openfilenameCU.lStructSize = sizeof(openfilenameCU);
			openfilenameCU.hwndOwner = hWnd;
			openfilenameCU.lpstrFile = abrirArchivoU;
			openfilenameCU.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilenameCU.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilenameCU.lpstrFile[0] = NULL;
			openfilenameCU.nMaxFile = sizeof(abrirArchivoU);
			openfilenameCU.nFilterIndex = 0;
			if (GetOpenFileName(&openfilenameCU) == true) {
				ifstream leer;
				leer.open(abrirArchivoU, ios::in | ios::binary);
				if (leer.is_open()) {
					Usuario* actual = new Usuario;
					leer.read(reinterpret_cast<char*>(actual), sizeof(Usuario));
					while (!leer.eof()) {
						actual->siguiente = nullptr;
						actual->anterior = nullptr;

						if (uhead == nullptr) {
							uhead = actual;
						}
						else {
							Usuario* ultimo = uhead;
							while (ultimo->siguiente != nullptr) {
								ultimo = ultimo->siguiente;
							};
							ultimo->siguiente = actual;
						}
						actual = new Usuario;
						leer.read((char*)actual, sizeof(Usuario));
					}
				}
				leer.close();
			}
		}
	};
	Validaciones validacion;

}








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
			case ID_GUARDARDATOS_GUARDARUSUARIOS:
				Estructuras::validacion.GuardarUsuarios(hWnd);
				break;
			case ID_CARGARDATOS_CARGARUSUARIOS:
				Estructuras::validacion.CargarUsuarios(hWnd);
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
					Estructuras::validacion.Login(hWnd);
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
				Estructuras::validacion.Registrar(hWnd);
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

