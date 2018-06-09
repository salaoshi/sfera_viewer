//----------------------------------------------------
//	Файл:	MAIN.CPP
//	Описание:Главный модуль приложения DirectDraw
//----------------------------------------------------

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include "main.h"
#include "resource.h"
#include <commdlg.h> //dialog
#include <commctrl.h>

//Обработчики сообщений Windows
BOOL DX_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void DX_OnDestroy(HWND hwnd);
void DX_OnTimer(HWND hwnd, UINT id);
void DX_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
void DX_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
void DX_OnIdle(HWND hwnd);
void DX_OnMove(HWND hwnd, int x, int y);
//void DX_WM_COMMAND(HWND hwnd,  DX_WM_COMMAND );
void RemoveDirectDraw();
int ReadSperaFile(WCHAR* file);
void DeleteFiles();
//Оконная процедура
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Характеристики таймера
#define TIMER_ID	1
#define TIMER_RATE	150

//Флаг активности нашего приложения
BOOL	bActive=FALSE;

 char about1[64]="Simple panorama player, alpha ver. 1.03";
 char about2[64]="Copyleft 2012 Sasha. Freeware.";

WCHAR file[MAX_PATH];
char ProgramPatch[MAX_PATH];

int Opened=0;
extern 	RECT rPic;
extern 	RECT rPic2;
extern  int  Width, Height ;
extern int  delta ;

HWND hWnd;
HINSTANCE hInst;
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst2, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	WNDCLASSEX wndClass;
	RECT rPic;

	memset(file,0,MAX_PATH);

	//----------------------------get path------------------------------------------	
		GetModuleFileName(hInst,ProgramPatch,sizeof(ProgramPatch));
					for(int d=MAX_PATH;d>0;d--)
						if(ProgramPatch[d]==0x5c)
						{ProgramPatch[d+1]=0;
						break;}
		//----------------------------get path------------------------------------------
		strcat(ProgramPatch,"\\jpg");	
		
		CreateDirectory(ProgramPatch, NULL);
	 

	MSG msg;
	hInst=hInst2;

	//Регистрация оконного класса
	wndClass.cbSize       =sizeof(wndClass);
	wndClass.style        =CS_HREDRAW|CS_VREDRAW;
	wndClass.lpfnWndProc  =WndProc;
	wndClass.cbClsExtra   =0;
	wndClass.cbWndExtra   =0;
	wndClass.hInstance    =hInst;
	wndClass.hIcon        =LoadIcon(NULL,"windows.ico");
	wndClass.hCursor      =LoadCursor(NULL,IDC_ARROW);
	wndClass.hbrBackground=NULL;//Обратите внимание!!!
	wndClass.lpszMenuName = (LPCSTR)IDC_SFERA_VIEWER;//NULL;
	wndClass.lpszClassName=ClassName;
	wndClass.hIconSm      =LoadIcon(NULL,"windows.ico");

	RegisterClassEx(&wndClass);

	 //hInst = hInstance; 
	SetRect(&rPic, 0, 0,FRAME_WIDTH, FRAME_HEIGHT);
	AdjustWindowRect( &rPic,WS_POPUP|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_OVERLAPPEDWINDOW,1); 

	 //Создание окна на основе класса
	hWnd=CreateWindowEx(
		0,//WS_EX_TOPMOST,	//Дополнительный стиль окна
		ClassName,		//Класс окна
		AppName,		//Текст заголовка
		WS_POPUP|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_OVERLAPPEDWINDOW,	//Стиль окна
		(GetSystemMetrics(SM_CXSCREEN)-(rPic.right-rPic.left))/2,
		(GetSystemMetrics(SM_CYSCREEN)-(rPic.bottom-rPic.top))/2,//Координаты X и Y
		(rPic.right-rPic.left),//+100,
		(rPic.bottom-rPic.top),//+58,		//Ширина и высота
		NULL,			//Дескриптор родительского окна
		NULL,			//Дескриптор меню
		hInst,			//Описатель экземпляра
		NULL);			//Дополнительные данные

	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);

if(lpszCmdLine[0])
{
int i,d,len;	

	for(i=0;i<(int)strlen(lpszCmdLine);i++)
	{
		if(lpszCmdLine[i]=='\r')lpszCmdLine[i]=0;
		if(lpszCmdLine[i]=='\n')lpszCmdLine[i]=0;
		if(lpszCmdLine[i]=='"')
		{
			len=(int)strlen(lpszCmdLine);

			for(d=i;d<len;d++)
			lpszCmdLine[d]=lpszCmdLine[d+1];

			for(d=i;d<len;d++)
			if(lpszCmdLine[d]=='"') lpszCmdLine[d]=0;
		}
	}

	MultiByteToWideChar(CP_UTF8,0, &lpszCmdLine[0], strlen(&lpszCmdLine[0]), file,MAX_PATH-1);

	PostMessage(hWnd,WM_COMMAND, CMD_OPEN,0);
}

	//Инициализация компонентов, связанных с DirectDraw
/*	if (!InitDirectDraw(hWnd))
		return FALSE;

	//Определение координат окна для вывода изображения
	SetRectangle();*/

	//Главный цикл программы
	while (TRUE)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			if (bActive)//Только если приложение активно!
			{
				DX_OnIdle(hWnd);
			}
	}
	return (msg.wParam);
}

/////////////////////////////////////////////////
//	Оконная процедура
////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg,
						 WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	OPENFILENAMEW ofn; 
	switch(msg)
	{
		HANDLE_MSG(hWnd,WM_CREATE,DX_OnCreate);
		HANDLE_MSG(hWnd,WM_DESTROY,DX_OnDestroy);
		HANDLE_MSG(hWnd,WM_TIMER,DX_OnTimer);
		HANDLE_MSG(hWnd,WM_ACTIVATE,DX_OnActivate);
		HANDLE_MSG(hWnd,WM_KEYDOWN,DX_OnKey);
		HANDLE_MSG(hWnd,WM_MOVE,DX_OnMove);

		case WM_SIZE:
			SetRectangle();
 			break;

 		case WM_COMMAND:

			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{

				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;

				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;

				case IDM_OPEN_FILE:

						if(Opened)   
							RemoveDirectDraw();

									ZeroMemory(&ofn, sizeof(OPENFILENAME));
									ZeroMemory(file, MAX_PATH);
							
							
									//ofn.nFilterIndex  =prev_opened_type;
									ofn.lStructSize = sizeof(OPENFILENAME); 
									ofn.hwndOwner = hWnd; 
									ofn.lpstrFile = file; 
									ofn.nMaxFile = MAX_PATH; 
									ofn.lpstrFilter =   &   L"Sfera Files (*.sfera\0*.SFERA\0"\
															L"All Files (*.*)\0*.*\0";
															
									//ofn.nFilterIndex = 0; 
									ofn.lpstrTitle=&L"Open sfera File"; 
									//ofn.lpstrInitialDir=OpenFileDirectory; 
									ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_ENABLESIZING | OFN_HIDEREADONLY;
									//ofn.lpstrFileTitle=source_file;	
									//ofn.lpTemplateName=source_file; 
									
									//	ofn.lpstrFileTitle    = szFileTitle;//get filename witoput path
									//	ofn.nMaxFileTitle     = sizeof(szFileTitle);
							
									if(!GetOpenFileNameW(&ofn)) 
											break;
						case	CMD_OPEN:

										if(!ReadSperaFile(file))
												break;
										Opened=1;
									
										if (!InitDirectDraw(hWnd))
											break;

										SetWindowPos((HWND)hWnd,NULL,	0,0, Width,Height, SWP_NOMOVE |SWP_NOZORDER);
								
										SetRectangle();
											break;




				default:
				   return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

/* Обработчики сообщений */
//----------------------------------------------------
/*void DX_WM_COMMAND (HWND hwnd, WPARAM wParam, LPARAM lParam)
{int wmId, wmEvent;

			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				//case IDM_ABOUT:
				  // DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				 //  break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			break;
}*/

//----------------------------------------------------

//----------------------------------------------------
BOOL DX_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{

//	if(ReadSperaFile( "Untitled-1.sfera"))


	if (!SetTimer(hwnd,TIMER_ID,TIMER_RATE,NULL))
		return (FALSE);

	if(file[0]) 
		PostMessage(hWnd,WM_COMMAND, CMD_OPEN,0);

	return (TRUE);
}

//----------------------------------------------------
void DX_OnDestroy(HWND hwnd)
{
	//Убрать после себя
	KillTimer(hwnd,TIMER_ID);
	DeleteFiles();
	RemoveDirectDraw();
	PostQuitMessage(0);
}

//----------------------------------------------------
void DX_OnTimer(HWND hwnd, UINT id)
{
	if (bActive)
		NextTick();
}

//----------------------------------------------------
void DX_OnIdle(HWND hwnd)
{
	if(Opened)
		DrawFrame();
}

//----------------------------------------------------
void DX_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	//При нажатии пробела прекратить работу программы
	if (vk==VK_ESCAPE)
		DestroyWindow(hwnd);
	if (vk==VK_SPACE)
 delta++;
}

//----------------------------------------------------
void DX_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
	//Обновить флаг состояния активности
	bActive =! fMinimized;
}
//----------------------------------------------------
void DX_OnMove(HWND hwnd, int x, int y)
{
	//Изменение коорданат вывода изображения
	SetRectangle();
}


LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HFONT hCurFont;
	HDC hdc;
	int y=0;
	switch (message)
	{
		case WM_PAINT:
			                      
			 hdc = BeginPaint(hDlg, &ps);

			 hCurFont=CreateFontW(15,0,0,0,FW_NORMAL,0,0,0,
				RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY,VARIABLE_PITCH|FF_ROMAN,L"Arial");
				
			SelectObject(hdc,hCurFont);

			SetBkMode(hdc, 0);

			TextOut(hdc, 20,20 ,about1,strlen(about1));
			TextOut(hdc, 20,40 ,about2,strlen(about2));
		 

			DeleteObject(hCurFont);

			EndPaint(hDlg, &ps);

			break;

		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}