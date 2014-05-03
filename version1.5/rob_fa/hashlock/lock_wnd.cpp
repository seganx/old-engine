#include <Windows.h>

#include "lock_wnd.h"


#define IDC_MAIN_BUTTON		104			// Button identifier
#define IDC_MAIN_EDIT		105			// Edit box identifier
#define IDC_STEAM_BUTTON	106			// Button identifier


HWND						hEdit;
HWND						hUnlock;
HWND						hSteam;
HFONT						hfnt = NULL;
int							accepted = 0;
wchar_t						verified_code[64] = {0};

void get_hash( char* dest, char* src )
{
	unsigned int c = 18373632;
	for ( int i=0; i<5; ++i )
	{
		for ( int j=0; j<5; ++j )
		{
			c += src[j] * src[j] * 999234657 +  src[i];
		}
		dest[i] = '0' + ( c % 10 );
	}
}

bool verify_hash_lock_code ( const wchar_t * code )
{
	wchar_t *c = (wchar_t*)code;
	char part1[6] = {0};
	char part2[6] = {0};
	char part3[6] = {0};
	char part4[6] = {0};

	for ( int i=0; i<5; i++ ) part1[i] = (char)*c++; c++;
	for ( int i=0; i<5; i++ ) part2[i] = (char)*c++; c++;
	for ( int i=0; i<5; i++ ) part3[i] = (char)*c++; c++;
	for ( int i=0; i<5; i++ ) part4[i] = (char)*c++; c++;

	char tmp[6] = {0};
	get_hash( tmp, part1 );
	if ( strcmp( tmp, part2 ) ) return false;
	get_hash( tmp, part3 );
	if ( strcmp( tmp, part4 ) ) return false;
	
	return true;
}



LRESULT CALLBACK WinProc_unlock(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);
			HFONT hFont = CreateFont(32,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY, VARIABLE_PITCH,TEXT("Arial"));

			// Create a push button
			hUnlock = CreateWindowEx(
				0,
				L"BUTTON",
				L"Unlock",
				WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_FLAT,
				271,
				478,
				174,
				34,
				hWnd,
				(HMENU)IDC_MAIN_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			SendMessage(hUnlock,
				WM_SETFONT,
				(WPARAM)hFont,
				MAKELPARAM(FALSE,0));

			// Create an edit box
			hEdit=CreateWindowEx(
				NULL,
				L"EDIT",
				L"",
				WS_CHILD|WS_VISIBLE,
				32,
				378,
				413,
				34,
				hWnd,
				(HMENU)IDC_MAIN_EDIT,
				GetModuleHandle(NULL),
				NULL);
			SendMessage(hEdit,
				WM_SETFONT,
				(WPARAM)hFont,
				MAKELPARAM(FALSE,0));
			SetFocus( hEdit );
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_MAIN_BUTTON:
			{
				wchar_t buffer[256] = {0};
				SendMessage(hEdit,
					WM_GETTEXT,
					sizeof(buffer)/sizeof(buffer[0]),
					reinterpret_cast<LPARAM>(buffer));
				
				//	verify the code
				if ( verify_hash_lock_code( buffer ) )
				{
					accepted = 1;
					memcpy( verified_code, buffer, 120 );
					PostQuitMessage(0);
				}
				else
				{
					MessageBox( hWnd, L"The key is not valid !", L"Error", MB_ICONERROR );
					SendMessage( hEdit, WM_SETFOCUS, 0, 0 );
					SendMessage( hEdit, EM_SETSEL, 0, 1000 );
					SetFocus( hEdit );
				}

			}
			break;
		}
		break;

	case WM_SETFONT:
		hfnt = (HFONT) wParam;
		break;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}

int show_hash_lock( HINSTANCE instance, wchar_t* dest )
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass,sizeof(WNDCLASSEX));
	wClass.cbClsExtra=NULL;
	wClass.cbSize=sizeof(WNDCLASSEX);
	wClass.cbWndExtra=NULL;
	wClass.hbrBackground=CreatePatternBrush(LoadBitmap( GetModuleHandle(NULL), MAKEINTRESOURCE(123451) ));
	wClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wClass.hIcon=NULL;
	wClass.hIconSm=NULL;
	wClass.hInstance= instance;
	wClass.lpfnWndProc=(WNDPROC)WinProc_unlock;
	wClass.lpszClassName=L"RfG_HashLockClass";
	wClass.lpszMenuName=NULL;
	wClass.style=CS_HREDRAW|CS_VREDRAW;

	if(!RegisterClassEx(&wClass))
	{
		int nResult=GetLastError();
		MessageBox(NULL,
			L"Window class creation failed\r\n",
			L"Window Class Failed",
			MB_ICONERROR);
	}

	RECT rc;
	GetWindowRect( GetDesktopWindow(), &rc );

	HWND hWnd=CreateWindowEx(NULL,
		L"RfG_HashLockClass",
		L"Rush for Glory",
		WS_POPUP,
		( rc.right - rc.left ) / 2 - 220,
		0,
		480,
		( rc.bottom - rc.top ),
		NULL,
		NULL,
		instance,
		NULL);

	ShowWindow( hWnd,TRUE );

	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));
	while( GetMessage(&msg,NULL,0,0) )
	{
		switch ( msg.message )
		{
		case WM_KEYDOWN:
			{
				if ( msg.wParam == VK_ESCAPE )
				{
					PostQuitMessage(0);
					continue;
				}
				else if ( msg.wParam == VK_RETURN )
				{
					SendMessage( hEdit, WM_KILLFOCUS, 0, 0 );
					SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(IDC_MAIN_BUTTON,BN_CLICKED), (LPARAM)hUnlock );
					continue;
				}
			}
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CloseWindow( hWnd );
	DestroyWindow(hWnd);
	UnregisterClass( L"RfG_SteamClass", instance );

	if ( accepted )
	{
		for ( int i=0; i<64; ++i )
			dest[i] = verified_code[i];
	}

	return accepted;
}


LRESULT CALLBACK WinProc_Steam(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);
			HFONT hFont = CreateFont(32,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY, VARIABLE_PITCH,TEXT("Arial"));

			// Create a push button
			hUnlock = CreateWindowEx(
				0,
				L"BUTTON",
				L"Vote at Steam Greenlight",
				WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_FLAT,
				32,
				358,
				413,
				34,
				hWnd,
				(HMENU)IDC_STEAM_BUTTON,
				GetModuleHandle(NULL),
				NULL);
			SendMessage(hUnlock,
				WM_SETFONT,
				(WPARAM)hFont,
				MAKELPARAM(FALSE,0));
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_STEAM_BUTTON:
			{
				ShellExecute( hWnd, L"OPEN", L"http://steamcommunity.com/sharedfiles/filedetails/?id=224709930", 0, 0, SHOW_OPENWINDOW );
				PostQuitMessage(0);
			}
			break;
		}
		break;

	case WM_SETFONT:
		hfnt = (HFONT) wParam;
		break;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}

void show_steam( HINSTANCE instance )
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass,sizeof(WNDCLASSEX));
	wClass.cbClsExtra=NULL;
	wClass.cbSize=sizeof(WNDCLASSEX);
	wClass.cbWndExtra=NULL;
	wClass.hbrBackground=CreatePatternBrush(LoadBitmap( GetModuleHandle(NULL), MAKEINTRESOURCE(123452) ));
	wClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wClass.hIcon=NULL;
	wClass.hIconSm=NULL;
	wClass.hInstance= instance;
	wClass.lpfnWndProc=(WNDPROC)WinProc_Steam;
	wClass.lpszClassName=L"RfG_SteamClass";
	wClass.lpszMenuName=NULL;
	wClass.style=CS_HREDRAW|CS_VREDRAW;

	if(!RegisterClassEx(&wClass))
	{
		int nResult=GetLastError();
		MessageBox(NULL,
			L"Window class creation failed\r\n",
			L"Window Class Failed",
			MB_ICONERROR);
	}

	RECT rc;
	GetWindowRect( GetDesktopWindow(), &rc );

	HWND hWnd=CreateWindowEx(NULL,
		L"RfG_SteamClass",
		L"Rush for Glory",
		WS_POPUP,
		( rc.right - rc.left ) / 2 - 220,
		0,
		480,
		( rc.bottom - rc.top ),
		NULL,
		NULL,
		instance,
		NULL);

	ShowWindow( hWnd,TRUE );

	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));
	while( GetMessage(&msg,NULL,0,0) )
	{
		switch ( msg.message )
		{
		case WM_KEYDOWN:
			{
				if ( msg.wParam == VK_ESCAPE )
				{
					PostQuitMessage(0);
				}
				else if ( msg.wParam == VK_RETURN )
				{
					SendMessage( hWnd, WM_COMMAND, MAKEWPARAM(IDC_STEAM_BUTTON, BN_CLICKED), (LPARAM)hSteam );
					PostQuitMessage(0);
				}
			}
			break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CloseWindow( hWnd );
	DestroyWindow( hWnd );
	UnregisterClass( L"RfG_SteamClass", instance );
}
