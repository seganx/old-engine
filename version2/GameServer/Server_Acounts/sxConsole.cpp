#include <stdio.h>
#include "sxConsole.h"

#include <commctrl.h>
#pragma comment(lib , "comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_LOG_TEXT			105
#define MEMO_MAX_LENGTH		50000

static HINSTANCE	s_hInst		= NULL;
static HWND			s_logText	= NULL;

static HGDIOBJ g_font_default = GetStockObject(DEFAULT_GUI_FONT);
static HFONT g_font_courier_new = CreateFont(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Courier New");


void create_log_text_control(HWND parentWindow)
{
	// Create an edit box
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL;
	s_logText = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", dwStyle, 10, 10, 380, 250, parentWindow, (HMENU)ID_LOG_TEXT, s_hInst, NULL);
	SendMessage(s_logText, WM_SETFONT, (WPARAM)g_font_courier_new, MAKELPARAM(FALSE, 0));
	SendMessage(s_logText, EM_LIMITTEXT, MEMO_MAX_LENGTH, 0);
}


LRESULT WINAPI DefaultMsgProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
			create_log_text_control(hWnd);
		break;

		case WM_SIZE:
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			SetWindowPos(s_logText, 0, 10, 10, rc.right - 20, rc.bottom - 20, 0);
		}
		break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_QUIT, 0, 0);
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND sx_create_window(const wchar* windowsName)
{
	WNDCLASS m_windowClass;
	sx_mem_set(&m_windowClass, 0, sizeof(m_windowClass));

	m_windowClass.style = CS_CLASSDC;
	m_windowClass.hInstance = s_hInst = GetModuleHandle(null);

	// Register the window class
	m_windowClass.lpfnWndProc = DefaultMsgProc;
	m_windowClass.lpszClassName = windowsName;
	m_windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	m_windowClass.hCursor = LoadCursor(null, IDC_ARROW);
	m_windowClass.hIcon = LoadIcon(null, IDI_APPLICATION);
	RegisterClass(&m_windowClass);

	// Create the window
	HWND res = CreateWindowW(
		m_windowClass.lpszClassName, 
		windowsName, 
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX,
		0,
		0,
		400,
		300,
		null,
		null,
		m_windowClass.hInstance,
		null);

	ShowWindow(res, SW_SHOWDEFAULT);
	UpdateWindow(res);

	return res;
}

void sx_window_set_text(const wchar* str)
{
	static wchar currtext[2048] = {0};
	if ( sx_str_cmp(currtext, str) )
	{
		sx_str_copy( currtext, 2048, str );
		SendMessage( s_logText, WM_SETTEXT, 0, (LPARAM)currtext );
	}
}
