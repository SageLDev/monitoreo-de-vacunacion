#include <Windows.h>
#include "resource.h"
#include <fstream>
#include <CommCtrl.h>

// GitHub repo: https://github.com/SageLDev/monitoreo-de-vacunacion

using namespace std;


HWND ghDlg = 0;
HINSTANCE hInstGlobal;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PrincipalProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegistroProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK VacunasAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ClientesAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


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
		wchar_t descripcion[500];
		Vacuna* siguiente;
		Vacuna* anterior;
	};

	typedef struct Cliente {
		wchar_t apellidoPaterno[200];
		wchar_t apellidoMaterno[200];
		wchar_t nombres[200];
		wchar_t curp[200];
		wchar_t rfc[200];
		wchar_t calle[500];
		wchar_t colonia[500];
		wchar_t ciudad[500];
		wchar_t estado[500];
		wchar_t civil[500];
		wchar_t telefono[500];
		wchar_t sexo[10];
		wchar_t ocupacion[500];
		wchar_t perfilDeRiesgo[500];
		SYSTEMTIME nacimiento;
		wchar_t imagen[500];
		bool vacunado = false;
		Cliente* siguiente;
		Cliente* anterior;
	};

	Usuario* uhead = nullptr;
	Usuario* usuarioActual;

	Vacuna* vhead = nullptr;
	Vacuna* vacunaActual;

	Cliente* chead = nullptr;
	Cliente* clienteActual;

	wchar_t direccionImagenCliente[500];

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

		void AltaDeVacuna(HWND hWnd) {
			Vacuna* ultimo = vhead;
			Vacuna* nuevo = new Vacuna;
			wchar_t precio[20];



			SendMessage(GetDlgItem(hWnd, VACUNAS_ALTA_TIPO), WM_GETTEXT, sizeof(nuevo->tipo) / sizeof(nuevo->tipo[0]), (LPARAM)nuevo->tipo);
			SendMessage(GetDlgItem(hWnd, VACUNAS_ALTA_MARCA), WM_GETTEXT, sizeof(nuevo->marca) / sizeof(nuevo->marca[0]), (LPARAM)nuevo->marca);
			SendMessage(GetDlgItem(hWnd, VACUNAS_ALTA_DESCRIPCION), WM_GETTEXT, sizeof(nuevo->descripcion) / sizeof(nuevo->descripcion[0]), (LPARAM)nuevo->descripcion);
			SendMessage(GetDlgItem(hWnd, VACUNAS_ALTA_PRECIO), WM_GETTEXT, sizeof(precio) / sizeof(precio[0]), (LPARAM)precio);
			nuevo->precio = wcstof(precio, NULL);
			nuevo->dosis = GetDlgItemInt(hWnd, VACUNAS_ALTA_DOSIS, NULL, false);
			nuevo->clave = GetDlgItemInt(hWnd, VACUNAS_ALTA_CLAVE, NULL, false);

			if (nuevo->precio <= 0) {
				MessageBox(hWnd, L"Favor de introducir un precio mayor a 0", L"Precio invalido", MB_OK);
			}
			else {
				nuevo->siguiente = nullptr;
				nuevo->anterior = nullptr;
				if (vhead == nullptr) {
					vhead = nuevo;
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
		}

		void CargarIdentidad(HWND hWnd) {
			OPENFILENAME openfilename;
			ZeroMemory(&openfilename, sizeof(openfilename));
			openfilename.lStructSize = sizeof(openfilename);
			openfilename.hwndOwner = hWnd;
			openfilename.lpstrFile = direccionImagenCliente;
			openfilename.lpstrFilter = L"Mapa de Bits\0*.bmp";
			openfilename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilename.lpstrFile[0] = NULL;
			openfilename.nMaxFile = sizeof(direccionImagenCliente);
			openfilename.nFilterIndex = 0;
			if (GetOpenFileName(&openfilename) == true) {
				HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, direccionImagenCliente, IMAGE_BITMAP, 200, 200, LR_LOADFROMFILE);
				SendDlgItemMessage(hWnd, BTN_CLIENTES_ALTA_IDENTIDAD, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
			}
		}

		void AltaDeCliente(HWND hWnd) {
			Cliente* ultimo = chead;
			Cliente* nuevo = new Cliente;

			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_APELLIDO_PATERNO), WM_GETTEXT, sizeof(nuevo->apellidoPaterno) / sizeof(nuevo->apellidoPaterno[0]), (LPARAM)nuevo->apellidoPaterno);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_APELLIDO_MATERNO), WM_GETTEXT, sizeof(nuevo->apellidoMaterno) / sizeof(nuevo->apellidoMaterno[0]), (LPARAM)nuevo->apellidoMaterno);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_NOMBRE), WM_GETTEXT, sizeof(nuevo->nombres) / sizeof(nuevo->nombres[0]), (LPARAM)nuevo->nombres);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_CURP), WM_GETTEXT, sizeof(nuevo->curp) / sizeof(nuevo->curp[0]), (LPARAM)nuevo->curp);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_RFC), WM_GETTEXT, sizeof(nuevo->rfc) / sizeof(nuevo->rfc[0]), (LPARAM)nuevo->rfc);


			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_CALLE), WM_GETTEXT, sizeof(nuevo->calle) / sizeof(nuevo->calle[0]), (LPARAM)nuevo->calle);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_COLONIA), WM_GETTEXT, sizeof(nuevo->colonia) / sizeof(nuevo->colonia[0]), (LPARAM)nuevo->colonia);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_CIUDAD), WM_GETTEXT, sizeof(nuevo->ciudad) / sizeof(nuevo->ciudad[0]), (LPARAM)nuevo->ciudad);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_ESTADO), WM_GETTEXT, sizeof(nuevo->estado) / sizeof(nuevo->estado[0]), (LPARAM)nuevo->estado);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_CIVIL), WM_GETTEXT, sizeof(nuevo->civil) / sizeof(nuevo->civil[0]), (LPARAM)nuevo->civil);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_TELEFONO), WM_GETTEXT, sizeof(nuevo->telefono) / sizeof(nuevo->telefono[0]), (LPARAM)nuevo->telefono);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_SEXO), WM_GETTEXT, sizeof(nuevo->sexo) / sizeof(nuevo->sexo[0]), (LPARAM)nuevo->sexo);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_OCUPACION), WM_GETTEXT, sizeof(nuevo->ocupacion) / sizeof(nuevo->ocupacion[0]), (LPARAM)nuevo->ocupacion);
			SendMessage(GetDlgItem(hWnd, CLIENTES_ALTA_PERFIL_RIESGO), WM_GETTEXT, sizeof(nuevo->perfilDeRiesgo) / sizeof(nuevo->perfilDeRiesgo[0]), (LPARAM)nuevo->perfilDeRiesgo);
			SendDlgItemMessage(hWnd, CLIENTES_ALTA_NACIMIENTO, DTM_GETSYSTEMTIME, NULL, (LPARAM) & (nuevo->nacimiento));
			wcscpy_s(nuevo->imagen, direccionImagenCliente);

			Cliente* aux = chead;
			bool error = false;

			if (chead != nullptr) {
				if (nuevo->curp == aux->curp) {
					MessageBox(hWnd, L"Favor de introducir una CURP unica y sin espacios", L"CURP invalido", MB_OK);
					error = true;
				}
				if (nuevo->rfc == aux->rfc) {
					MessageBox(hWnd, L"Favor de introducir un RFC unica y sin espacios", L"RFC invalido", MB_OK);
					error = true;
				}
				while (aux->siguiente != nullptr)
				{
					if (nuevo->curp == aux->curp) {
						MessageBox(hWnd, L"Favor de introducir una CURP unica y sin espacios", L"CURP invalido", MB_OK);
						error = true;
					}
					if (nuevo->rfc == aux->rfc) {
						MessageBox(hWnd, L"Favor de introducir un RFC unica y sin espacios", L"RFC invalido", MB_OK);
						error = true;
					}
					aux = aux->siguiente;
				}
			}

			if (error == false) {
				nuevo->siguiente = nullptr;
				nuevo->anterior = nullptr;
				if (chead == nullptr) {
					chead = nuevo;
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



		}

		void Login(HWND hWnd) {
			wchar_t usuarioIngresado[200];
			wchar_t contrasenaIngresada[200];
			SendMessage(GetDlgItem(hWnd, INICIO_USUARIO), WM_GETTEXT, sizeof(usuarioIngresado) / sizeof(usuarioIngresado[0]), (LPARAM)usuarioIngresado);
			SendMessage(GetDlgItem(hWnd, INICIO_CONTRA), WM_GETTEXT, sizeof(contrasenaIngresada) / sizeof(contrasenaIngresada[0]), (LPARAM)contrasenaIngresada);

			Usuario* ptr = uhead;
			bool encontrado = false;
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
					}
					else {
						encontrado = false;
					}
					ptr = ptr->siguiente;
				}
				if (encontrado == false) {
					MessageBox(hWnd, L"No encontramos un usuario con esas credenciales.", L"Usuario no encontrado", MB_OK);
				}
				else {
					DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PANTALLA_INICIAL), hWnd, (DLGPROC)PrincipalProc);
				}
			}
			
		}

		void actualizarListBoxVacunas(HWND hWnd) {
			SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_RESETCONTENT, NULL, NULL);
			Vacuna* ptr = vhead;
			if (ptr != nullptr) {
				while (ptr->siguiente != nullptr) {
					SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_ADDSTRING, NULL, (LPARAM)ptr->marca);
					ptr = ptr->siguiente;
				}
				SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_ADDSTRING, NULL, (LPARAM)ptr->marca);
			}
		}

		void actualizarListBoxClientes(HWND hWnd) {
			SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_RESETCONTENT, NULL, NULL);
			Cliente* ptr = chead;
			if (ptr != nullptr) {
				while (ptr->siguiente != nullptr) {
					SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_ADDSTRING, NULL, (LPARAM)ptr->nombres);
					ptr = ptr->siguiente;
				}
				SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_ADDSTRING, NULL, (LPARAM)ptr->nombres);
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



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	hInstGlobal = hInstance;
	ghDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_USUARIO_INICIO_SESION), 0, (DLGPROC)LoginProc);
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
			case ID_USUARIO_REGISTRODEUSUARIO:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_USUARIO_REGISTRO), hWnd, (DLGPROC)RegistroProc);
				break;
			case ID_USUARIO_CERRARSESION:
				EndDialog(hWnd, 0);
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_USUARIO_INICIO_SESION), hWnd, (DLGPROC)LoginProc);
				return true;

			case ID_VACUNAS_ALTADEVACUNA:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_VACUNAS_ALTA), hWnd, (DLGPROC)VacunasAltaProc);
				Estructuras::validacion.actualizarListBoxVacunas(hWnd);
				break;
			case ID_CLIENTES_ALTADECLIENTE:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_CLIENTES_ALTA), hWnd, (DLGPROC)ClientesAltaProc);
				Estructuras::validacion.actualizarListBoxClientes(hWnd);
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

				case ID_GUARDARDATOS_GUARDARUSUARIOS:
				Estructuras::validacion.GuardarUsuarios(hWnd);
				break;

				case ID_CARGARDATOS_CARGARUSUARIOS:
					Estructuras::validacion.CargarUsuarios(hWnd);
				break;

				case ID_USUARIO_REGISTRODEUSUARIO:
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_USUARIO_REGISTRO), hWnd, (DLGPROC)RegistroProc);
				break;

				case BTN_ACEPTAR_USUARIO_INICIO:
					Estructuras::validacion.Login(hWnd);
					Estructuras::validacion.actualizarListBoxClientes(hWnd);
				break;

				case BTN_CERRAR_USUARIO_INICIO:
					DestroyWindow(ghDlg);
					return true;
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
				Estructuras::validacion.AltaDeVacuna(hWnd);
				
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
				case BTN_CLIENTES_ALTA_CARGAR_IDENTIDAD: {
					Estructuras::validacion.CargarIdentidad(hWnd);
				}
					break;
				case BTN_ACEPTAR_CLIENTES_ALTA:
					Estructuras::validacion.AltaDeCliente(hWnd);
	
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

