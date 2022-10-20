#include <Windows.h>
#include "resource.h"
#include <fstream>
#include <CommCtrl.h>
#include <cstdlib>
#include <algorithm>
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
BOOL CALLBACK CarnetsAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CarnetsBusquedaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



namespace Estructuras {
	typedef struct Usuario {
		wchar_t usuario[200];
		wchar_t contrasena[200];
		wchar_t apellidoPaterno[200];
		wchar_t apellidoMaterno[200];
		wchar_t nombres[500];
		int ID;
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
		int ID;
		int usuarioID;
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
		int ID;
		int usuarioID;
		Cliente* siguiente;
		Cliente* anterior;
	};

	typedef struct Carnet {

		int usuarioID;
		int vacunaID;
		int clienteID;
		int dosis;
		SYSTEMTIME fechaDosis;
		int lote;
		wchar_t centroDeVacunacion[500];
		int ID;
		wchar_t clienteCURP[200];
		wchar_t clienteNombre[200];
		Carnet* siguiente;
		Carnet* anterior;
	};

	Usuario* uhead = nullptr;
	Usuario* usuarioActual;

	Vacuna* vhead = nullptr;
	Vacuna* vacunaActual;
	Vacuna* vacunaSeleccionada;

	Cliente* chead = nullptr;
	Cliente* clienteActual;
	Cliente* clienteSeleccionado;

	Carnet* rhead = nullptr;
	Carnet* carnetActual;


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
			srand(time(0));
			nuevo->ID = rand();
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
			nuevo->usuarioID = usuarioActual->ID;
			srand(time(0));
			nuevo->ID = rand();

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
			nuevo->usuarioID = usuarioActual->ID;
			srand(time(0));
			nuevo->ID = rand();

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

		void AltaDeCarnet(HWND hWnd) {
			Carnet* ultimo = rhead;
			Carnet* nuevo = new Carnet;

			int cSeleccionado = SendDlgItemMessage(hWnd, IDC_CARNET_CLIENTE, LB_GETCURSEL, NULL, NULL);
			int vSeleccionado = SendDlgItemMessage(hWnd, IDC_CARNET_VACUNAS, LB_GETCURSEL, NULL, NULL);
			if (cSeleccionado == -1 || vSeleccionado == -1) {
				MessageBox(hWnd, L"No se ha seleccionado un cliente y vacuna para el registro de carnet", L"Error", MB_OK | MB_ICONERROR);
				return;
			}	

			SendMessage(GetDlgItem(hWnd, CARNET_ALTA_CENTRO), WM_GETTEXT, sizeof(nuevo->centroDeVacunacion) / sizeof(nuevo->centroDeVacunacion[0]), (LPARAM)nuevo->centroDeVacunacion);
			SendDlgItemMessage(hWnd, IDC_FECHADOSIS, DTM_GETSYSTEMTIME, NULL, (LPARAM) & (nuevo->fechaDosis));
			nuevo->lote = GetDlgItemInt(hWnd, CARNET_ALTA_LOTE, NULL, false);
			nuevo->dosis = GetDlgItemInt(hWnd, CARNET_ALTA_DOSIS, NULL, false);
			nuevo->clienteID = clienteSeleccionado->ID;
			nuevo->vacunaID = vacunaSeleccionada->ID;
			nuevo->usuarioID = usuarioActual->ID;

			wcscpy_s(nuevo->clienteCURP, clienteSeleccionado->curp);
			wcscpy_s(nuevo->clienteNombre, clienteSeleccionado->nombres);

			srand(time(0));
			nuevo->ID = rand();


			nuevo->siguiente = nullptr;
			nuevo->anterior = nullptr;
			if (rhead == nullptr) {
				rhead = nuevo;
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

		void actualizarListBox(HWND hWnd) {
			SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_RESETCONTENT, NULL, NULL);
			Vacuna* vptr = vhead;
			if (vptr != nullptr) {
				while (vptr->siguiente != nullptr) {
					if (vptr->usuarioID == usuarioActual->ID) {
						SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_ADDSTRING, NULL, (LPARAM)vptr->marca);
					}
					vptr = vptr->siguiente;
				}
				if (vptr->usuarioID == usuarioActual->ID) {
					SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_ADDSTRING, NULL, (LPARAM)vptr->marca);
				}
			}

			SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_RESETCONTENT, NULL, NULL);
			Cliente* cptr = chead;
			if (cptr != nullptr) {
				while (cptr->siguiente != nullptr) {
					if (cptr->usuarioID == usuarioActual->ID) {
						SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_ADDSTRING, NULL, (LPARAM)(cptr->nombres));
					}
					cptr = cptr->siguiente;
				}
				if (cptr->usuarioID == usuarioActual->ID) {
					SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_ADDSTRING, NULL, (LPARAM)cptr->nombres);
				}
			}

			SendDlgItemMessage(hWnd, IDC_LIST_CARNETS, LB_RESETCONTENT, NULL, NULL);
			Carnet* rptr = rhead;
			if (rptr != nullptr) {
				while (rptr->siguiente != nullptr) {
					if (rptr->usuarioID == usuarioActual->ID) {
						wchar_t IDString[200];
						swprintf_s(IDString, L"%d", rptr->ID);
						SendDlgItemMessage(hWnd, IDC_LIST_CARNETS, LB_ADDSTRING, NULL, (LPARAM)(IDString));
					}
					rptr = rptr->siguiente;
				}
				if (rptr->usuarioID == usuarioActual->ID) {
					wchar_t IDString[200];
					swprintf_s(IDString, L"%d", rptr->ID);
					SendDlgItemMessage(hWnd, IDC_LIST_CARNETS, LB_ADDSTRING, NULL, (LPARAM)IDString);
				}
			}

		}

		void actualizarCarnetListBox(HWND hWnd) {
			SendDlgItemMessage(hWnd, IDC_CARNET_VACUNAS, LB_RESETCONTENT, NULL, NULL);
			Vacuna* vptr = vhead;
			if (vptr != nullptr) {
				while (vptr->siguiente != nullptr) {
					if (vptr->usuarioID == usuarioActual->ID) {
						SendDlgItemMessage(hWnd, IDC_CARNET_VACUNAS, LB_ADDSTRING, NULL, (LPARAM)vptr->marca);
					}
					vptr = vptr->siguiente;
				}
				if (vptr->usuarioID == usuarioActual->ID) {
					SendDlgItemMessage(hWnd, IDC_CARNET_VACUNAS, LB_ADDSTRING, NULL, (LPARAM)vptr->marca);
				}
			}

			SendDlgItemMessage(hWnd, IDC_CARNET_CLIENTE, LB_RESETCONTENT, NULL, NULL);
			Cliente* cptr = chead;
			if (cptr != nullptr) {
				while (cptr->siguiente != nullptr) {
					if (cptr->usuarioID == usuarioActual->ID) {
				
						SendDlgItemMessage(hWnd, IDC_CARNET_CLIENTE, LB_ADDSTRING, NULL, (LPARAM)(cptr->nombres));
					}
					cptr = cptr->siguiente;
				}
				if (cptr->usuarioID == usuarioActual->ID) {
					SendDlgItemMessage(hWnd, IDC_CARNET_CLIENTE, LB_ADDSTRING, NULL, (LPARAM)cptr->nombres);
				}
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
				while (ptr != nullptr && encontrado == false) {
					if (wcscmp(ptr->usuario, usuarioIngresado) == 0 && wcscmp(ptr->contrasena, contrasenaIngresada) == 0) {
						encontrado = true;
						usuarioActual = ptr;
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

		void GuardarClientes(HWND hWnd) {
			OPENFILENAME openfilenameGC;
			wchar_t guardarArchivoC[500];
			ZeroMemory(&openfilenameGC, sizeof(openfilenameGC));
			openfilenameGC.lStructSize = sizeof(openfilenameGC);
			openfilenameGC.hwndOwner = hWnd;
			openfilenameGC.lpstrFile = guardarArchivoC;
			openfilenameGC.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilenameGC.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilenameGC.lpstrFile[0] = NULL;
			openfilenameGC.nMaxFile = sizeof(guardarArchivoC);
			openfilenameGC.nFilterIndex = 0;
			if (GetSaveFileName(&openfilenameGC) == true) {
				ofstream escribir;
				escribir.open(guardarArchivoC, ios::out | ios::binary | ios::trunc);
				if (escribir.is_open()) {

					if (chead != NULL) {
						Cliente* cptr = chead;
						while (cptr->siguiente != NULL)
						{
							escribir.write(reinterpret_cast<char*>(cptr), sizeof(Cliente));
							cptr = cptr->siguiente;
						}
						escribir.write(reinterpret_cast<char*>(cptr), sizeof(Cliente));
					}
					escribir.close();
				}
			}
		}

		void GuardarVacunas(HWND hWnd) {
			OPENFILENAME openfilenameGV;
			wchar_t guardarArchivoV[500];
			ZeroMemory(&openfilenameGV, sizeof(openfilenameGV));
			openfilenameGV.lStructSize = sizeof(openfilenameGV);
			openfilenameGV.hwndOwner = hWnd;
			openfilenameGV.lpstrFile = guardarArchivoV;
			openfilenameGV.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilenameGV.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilenameGV.lpstrFile[0] = NULL;
			openfilenameGV.nMaxFile = sizeof(guardarArchivoV);
			openfilenameGV.nFilterIndex = 0;
			if (GetSaveFileName(&openfilenameGV) == true) {
				ofstream escribir;
				escribir.open(guardarArchivoV, ios::out | ios::binary | ios::trunc);
				if (escribir.is_open()) {

					if (vhead != NULL) {
						Vacuna* vptr = vhead;
						while (vptr->siguiente != NULL)
						{
							escribir.write(reinterpret_cast<char*>(vptr), sizeof(Vacuna));
							vptr = vptr->siguiente;
						}
						escribir.write(reinterpret_cast<char*>(vptr), sizeof(Vacuna));
					}
					escribir.close();
				}
			}
		}

		void GuardarCarnets(HWND hWnd) {
			OPENFILENAME openfilenameGR;
			wchar_t guardarArchivoR[500];
			ZeroMemory(&openfilenameGR, sizeof(openfilenameGR));
			openfilenameGR.lStructSize = sizeof(openfilenameGR);
			openfilenameGR.hwndOwner = hWnd;
			openfilenameGR.lpstrFile = guardarArchivoR;
			openfilenameGR.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilenameGR.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilenameGR.lpstrFile[0] = NULL;
			openfilenameGR.nMaxFile = sizeof(guardarArchivoR);
			openfilenameGR.nFilterIndex = 0;
			if (GetSaveFileName(&openfilenameGR) == true) {
				ofstream escribir;
				escribir.open(guardarArchivoR, ios::out | ios::binary | ios::trunc);
				if (escribir.is_open()) {

					if (rhead != NULL) {
						Carnet* rptr = rhead;
						while (rptr->siguiente != NULL)
						{
							escribir.write(reinterpret_cast<char*>(rptr), sizeof(Carnet));
							rptr = rptr->siguiente;
						}
						escribir.write(reinterpret_cast<char*>(rptr), sizeof(Carnet));
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

		void CargarClientes(HWND hWnd) {
			OPENFILENAME openfilename;
			wchar_t abrirArchivo[500];
			ZeroMemory(&openfilename, sizeof(openfilename));
			openfilename.lStructSize = sizeof(openfilename);
			openfilename.hwndOwner = hWnd;
			openfilename.lpstrFile = abrirArchivo;
			openfilename.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilename.lpstrFile[0] = NULL;
			openfilename.nMaxFile = sizeof(abrirArchivo);
			openfilename.nFilterIndex = 0;
			if (GetOpenFileName(&openfilename) == true) {
				ifstream leer;
				leer.open(abrirArchivo, ios::in | ios::binary);
				if (leer.is_open()) {
					Cliente* actual = new Cliente;
					leer.read(reinterpret_cast<char*>(actual), sizeof(Cliente));
					while (!leer.eof()) {
						actual->siguiente = nullptr;
						actual->anterior = nullptr;

						if (chead == nullptr) {
							chead = actual;
						}
						else {
							Cliente* ultimo = chead;
							while (ultimo->siguiente != nullptr) {
								ultimo = ultimo->siguiente;
							};
							ultimo->siguiente = actual;
						}
						actual = new Cliente;
						leer.read((char*)actual, sizeof(Cliente));
					}
				}
				leer.close();
			}
		}

		void CargarVacunas(HWND hWnd) {
			OPENFILENAME openfilename;
			wchar_t abrirArchivo[500];
			ZeroMemory(&openfilename, sizeof(openfilename));
			openfilename.lStructSize = sizeof(openfilename);
			openfilename.hwndOwner = hWnd;
			openfilename.lpstrFile = abrirArchivo;
			openfilename.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilename.lpstrFile[0] = NULL;
			openfilename.nMaxFile = sizeof(abrirArchivo);
			openfilename.nFilterIndex = 0;
			if (GetOpenFileName(&openfilename) == true) {
				ifstream leer;
				leer.open(abrirArchivo, ios::in | ios::binary);
				if (leer.is_open()) {
					Vacuna* actual = new Vacuna;
					leer.read(reinterpret_cast<char*>(actual), sizeof(Vacuna));
					while (!leer.eof()) {
						actual->siguiente = nullptr;
						actual->anterior = nullptr;

						if (vhead == nullptr) {
							vhead = actual;
						}
						else {
							Vacuna* ultimo = vhead;
							while (ultimo->siguiente != nullptr) {
								ultimo = ultimo->siguiente;
							};
							ultimo->siguiente = actual;
						}
						actual = new Vacuna;
						leer.read((char*)actual, sizeof(Vacuna));
					}
				}
				leer.close();
			}
		}

		void CargarCarnet(HWND hWnd) {
			OPENFILENAME openfilename;
			wchar_t abrirArchivo[500];
			ZeroMemory(&openfilename, sizeof(openfilename));
			openfilename.lStructSize = sizeof(openfilename);
			openfilename.hwndOwner = hWnd;
			openfilename.lpstrFile = abrirArchivo;
			openfilename.lpstrFilter = L"Archivo Binario\0*.bin";
			openfilename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openfilename.lpstrFile[0] = NULL;
			openfilename.nMaxFile = sizeof(abrirArchivo);
			openfilename.nFilterIndex = 0;
			if (GetOpenFileName(&openfilename) == true) {
				ifstream leer;
				leer.open(abrirArchivo, ios::in | ios::binary);
				if (leer.is_open()) {
					Carnet* actual = new Carnet;
					leer.read(reinterpret_cast<char*>(actual), sizeof(Carnet));
					while (!leer.eof()) {
						actual->siguiente = nullptr;
						actual->anterior = nullptr;

						if (rhead == nullptr) {
							rhead = actual;
						}
						else {
							Carnet* ultimo = rhead;
							while (ultimo->siguiente != nullptr) {
								ultimo = ultimo->siguiente;
							};
							ultimo->siguiente = actual;
						}
						actual = new Carnet;
						leer.read((char*)actual, sizeof(Carnet));
					}
				}
				leer.close();
			}
		}

		void ClienteSeleccionado(HWND hWnd,WPARAM wParam) {

			if (HIWORD(wParam) == LBN_SELCHANGE) {

				int seleccionado = SendDlgItemMessage(hWnd, IDC_CARNET_CLIENTE, LB_GETCURSEL, NULL, NULL);

				Cliente* ptr = chead;

				for (int i = 0; i < seleccionado; i++) {
					ptr = ptr->siguiente;
				}
				clienteSeleccionado = ptr;

				SendDlgItemMessage(hWnd, CARNETS_ALTA_CLIENTE, WM_SETTEXT, NULL, (LPARAM)clienteSeleccionado->nombres);

			}
		}

		void VacunaSeleccionada(HWND hWnd, WPARAM wParam) {

			if (HIWORD(wParam) == LBN_SELCHANGE) {

				int seleccionado = SendDlgItemMessage(hWnd, IDC_CARNET_VACUNAS, LB_GETCURSEL, NULL, NULL);

				Vacuna* ptr = vhead;

				
				for (int i = 0; i < seleccionado; i++) {
					ptr = ptr->siguiente;
				}
				vacunaSeleccionada = ptr;

				SendDlgItemMessage(hWnd, VACUNAS_ALTA_MARCA, WM_SETTEXT, NULL, (LPARAM)vacunaSeleccionada->marca);

			}
		}

		void ClienteActual(HWND hWnd, WPARAM wParam) {
			if (HIWORD(wParam) == LBN_SELCHANGE) {

				int seleccionado = SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_GETCURSEL, NULL, NULL);

				Cliente* ptr = chead;

				for (int i = 0; i < seleccionado; i++) {
					ptr = ptr->siguiente;
				}
				clienteActual = ptr;

			}
		}
		void VacunaActual(HWND hWnd, WPARAM wParam) {
			if (HIWORD(wParam) == LBN_SELCHANGE) {

				int seleccionado = SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_GETCURSEL, NULL, NULL);

				Vacuna* ptr = vhead;


				for (int i = 0; i < seleccionado; i++) {
					ptr = ptr->siguiente;
				}
				vacunaActual = ptr;

			}
		}
		void CarnetActual(HWND hWnd, WPARAM wParam) {
			if (HIWORD(wParam) == LBN_SELCHANGE) {

				int seleccionado = SendDlgItemMessage(hWnd, IDC_LIST_CARNETS, LB_GETCURSEL, NULL, NULL);

				Carnet* ptr = rhead;


				for (int i = 0; i < seleccionado; i++) {
					ptr = ptr->siguiente;
				}
				carnetActual = ptr;

			}
		}

		void EliminarCliente(HWND hWnd) {
			int seleccionado = SendDlgItemMessage(hWnd, IDC_LIST_CLIENTES, LB_GETCURSEL, NULL, NULL);
			if (seleccionado == -1) {
				MessageBox(hWnd, L"No se ha seleccionado cliente para eliminar", L"Error", MB_OK | MB_ICONERROR);
				return;
			}

			if (chead == NULL || clienteActual == NULL) {

				return;
			}
			if (clienteActual == chead) {
				chead = clienteActual->siguiente;
			}
			if (clienteActual->siguiente != NULL) {
				clienteActual->siguiente->anterior = clienteActual->anterior;
			}
			if (clienteActual->anterior != NULL) {
				clienteActual->anterior->siguiente = clienteActual->siguiente;
			}

			free(clienteActual);
			actualizarListBox(hWnd);
		}
		void EliminarVacuna(HWND hWnd) {
			int seleccionado = SendDlgItemMessage(hWnd, IDC_LIST_VACUNAS, LB_GETCURSEL, NULL, NULL);
			if (seleccionado == -1) {
				MessageBox(hWnd, L"No se ha seleccionado vacuna para eliminar", L"Error", MB_OK | MB_ICONERROR);
				return;
			}

			if (vhead == NULL || vacunaActual == NULL) {

				return;
			}
			if (vacunaActual == vhead) {
				vhead = vacunaActual->siguiente;
			}
			if (vacunaActual->siguiente != NULL) {
				vacunaActual->siguiente->anterior = vacunaActual->anterior;
			}
			if (vacunaActual->anterior != NULL) {
				vacunaActual->anterior->siguiente = vacunaActual->siguiente;
			}

			free(clienteActual);
			actualizarListBox(hWnd);
		}
		void EliminarCarnet(HWND hWnd) {
			int seleccionado = SendDlgItemMessage(hWnd, IDC_LIST_CARNETS, LB_GETCURSEL, NULL, NULL);
			if (seleccionado == -1) {
				MessageBox(hWnd, L"No se ha seleccionado carnet para eliminar", L"Error", MB_OK | MB_ICONERROR);
				return;
			}

			if (rhead == NULL || carnetActual == NULL) {

				return;
			}
			if (carnetActual == rhead) {
				rhead = carnetActual->siguiente;
			}
			if (carnetActual->siguiente != NULL) {
				carnetActual->siguiente->anterior = carnetActual->anterior;
			}
			if (carnetActual->anterior != NULL) {
				carnetActual->anterior->siguiente = carnetActual->siguiente;
			}

			free(clienteActual);
			actualizarListBox(hWnd);
		}
		int resultadoBinario(int arrayID[], int izquierda, int derecha, int target) {
			if (derecha >= izquierda) {
				int mitad = izquierda + (derecha - izquierda) / 2;
				if (arrayID[mitad] == target) {
					return mitad;
				}
				if (arrayID[mitad] > target) {
					return resultadoBinario(arrayID, izquierda, mitad - 1, target);
				}
				else {
					return resultadoBinario(arrayID, mitad + 1, derecha, target);
				}

			}
			return -1;
		}
		void busquedaBinaria(HWND hWnd) {

			wchar_t IDIngresada[200];
			SendMessage(GetDlgItem(hWnd, INICIO_USUARIO), WM_GETTEXT, sizeof(IDIngresada) / sizeof(IDIngresada[0]), (LPARAM)IDIngresada);
			int target;
			int arrayID[30];
			int n = sizeof(arrayID) / sizeof(arrayID[0]);
			int izquierda = 0;
			int derecha = (sizeof(arrayID) / sizeof(arrayID[0])) - 1;
			
			int aux = 0;
			Carnet* search = rhead;

			if (rhead == nullptr) {
				return;
			}
			else {
				while (search->siguiente != nullptr)
				{
					arrayID[aux] = rhead->ID;
					aux++;
					search = search->siguiente;
				}
				arrayID[aux] = rhead->ID;
			}
			sort(arrayID, arrayID + n);



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
			Estructuras::validacion.actualizarListBox(hWnd);
		} break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
				case IDC_LIST_VACUNAS:
					Estructuras::validacion.VacunaActual(hWnd,wParam);
					break;
				case IDC_LIST_CLIENTES:
					Estructuras::validacion.ClienteActual(hWnd, wParam);
					break;
				case IDC_LIST_CARNETS:
					Estructuras::validacion.CarnetActual(hWnd, wParam);
					break;
				case ID_GUARDARDATOS_GUARDARUSUARIOS:
					Estructuras::validacion.GuardarUsuarios(hWnd);
					break;
				case ID_GUARDARDATOS_GUARDARCLIENTES:
					Estructuras::validacion.GuardarClientes(hWnd);
					break;
				case ID_GUARDARDATOS_GUARDARVACUNAS:
					Estructuras::validacion.GuardarVacunas(hWnd);
					break;
				case ID_GUARDARDATOS_GUARDARCARNETS:
					Estructuras::validacion.GuardarCarnets(hWnd);
					break;
				case ID_CARGARDATOS_CARGARUSUARIOS:
					Estructuras::validacion.CargarUsuarios(hWnd);
					Estructuras::validacion.actualizarListBox(hWnd);
					break;
				case ID_CARGARDATOS_CARGARCLIENTES:
					Estructuras::validacion.CargarClientes(hWnd);
					Estructuras::validacion.actualizarListBox(hWnd);
					break;
				case ID_CARGARDATOS_CARGARVACUNAS:
					Estructuras::validacion.CargarVacunas(hWnd);
					Estructuras::validacion.actualizarListBox(hWnd);
					break;
				case ID_CARGARDATOS_CARGARCARNETS:
					Estructuras::validacion.CargarCarnet(hWnd);
					Estructuras::validacion.actualizarListBox(hWnd);
					break;
				case ID_VACUNAS_BAJADEVACUNA:
					Estructuras::validacion.EliminarVacuna(hWnd);
					break;
				case ID_CLIENTES_BAJADECLIENTE:
					Estructuras::validacion.EliminarCliente(hWnd);
					break;
				case ID_CARNETS_BAJADECARNET:
					Estructuras::validacion.EliminarCarnet(hWnd);
					break;
				case ID_USUARIO_REGISTRODEUSUARIO:
					DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_USUARIO_REGISTRO), hWnd, (DLGPROC)RegistroProc);
					break;
				case ID_USUARIO_CERRARSESION:
					EndDialog(hWnd, 0);
					return true;

				case ID_VACUNAS_ALTADEVACUNA:
					DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_VACUNAS_ALTA), hWnd, (DLGPROC)VacunasAltaProc);
					Estructuras::validacion.actualizarListBox(hWnd);
					break;
				case ID_CLIENTES_ALTADECLIENTE:
					DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_CLIENTES_ALTA), hWnd, (DLGPROC)ClientesAltaProc);
					Estructuras::validacion.actualizarListBox(hWnd);
					break;
				case ID_CARNETS_ALTADECARNET:
					DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_CARNET_ALTA), hWnd, (DLGPROC)CarnetsAltaProc);
					Estructuras::validacion.actualizarListBox(hWnd);
					break;

				case ID_CARNETS_BUSQUEDA:
					DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_CARNET_BUSQUEDA), hWnd, (DLGPROC)CarnetsBusquedaProc);
					break;
			}

			


		} break;
		case WM_CLOSE:
		{
			DestroyWindow(ghDlg);
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


BOOL CALLBACK CarnetsAltaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			Estructuras::validacion.actualizarCarnetListBox(hWnd);
		} break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {

				case IDC_CARNET_CLIENTE:
					Estructuras::validacion.ClienteSeleccionado(hWnd, wParam);
					
					break;
				case IDC_CARNET_VACUNAS:
					Estructuras::validacion.VacunaSeleccionada(hWnd, wParam);
					break;
				case BTN_ACEPTAR_CARNET_ALTA:
					Estructuras::validacion.AltaDeCarnet(hWnd);
					break;
				case BTN_CERRAR_CARNET_ALTA:
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

BOOL CALLBACK CarnetsBusquedaProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		
	} break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {

			case IDC_CARNET_BUSCARNOMBRE:

				break;
			case IDC_CARNET_BUSCARID:

				break;
			case BTN_CERRAR_CARNET_BUSQUEDA:
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