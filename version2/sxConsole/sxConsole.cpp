// sxConsole.cpp : Defines the entry point for the application.
//
#include "sxConsole.h"
#include <stdio.h>

#define _WIN32_IE 0x0400
#include <commctrl.h>
#pragma comment(lib , "comctl32.lib")

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_TAB				101
#define ID_BTN_SEND			102
#define ID_SC_LIST			103
#define ID_BTN_SEARCH		104
#define ID_LOG_TEXT			105
#define ID_TEXT				106

#define MEMO_MAX_LENGTH		10000

#define NET_DELAY_TIME		60
#define NET_TIME_OUT		15000

ConsoleNetwork s_consoleNetworkLocal;
extern ConsoleNetwork * g_network = &s_consoleNetworkLocal;

HINSTANCE	g_hInst			= NULL;
HWND		g_winMain		= NULL;
HWND		g_tab			= NULL;
HWND		g_logText		= NULL;
HWND		g_scList		= NULL;
HWND		g_btnSearch		= NULL;
HWND		g_text			= NULL;
HWND		g_btnSend		= NULL;
WNDPROC		g_text_proc		= NULL;

HGDIOBJ g_font_default		= GetStockObject( DEFAULT_GUI_FONT );
HFONT g_font_courier_new	= CreateFont(-12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,L"Courier New");

//////////////////////////////////////////////////////////////////////////
//  SOME GLOBAL VARIABLES AND FUNCTIONS
LRESULT WINAPI InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void callback_client(const class Client* client, const char* buffer, const sint bufSize);

void create_tab_control( void )
{ 
	// Initialize common controls.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof( INITCOMMONCONTROLSEX );
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx( &icex );
	
	DWORD tabStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_TABS;
	g_tab = CreateWindowEx( 0, WC_TABCONTROL, L"", tabStyle, 0, 0, 300, 300, g_winMain, (HMENU)ID_TAB, g_hInst, NULL); 
	if ( !g_tab )
	{ 
		return;
	}
	SendMessage( g_tab, WM_SETFONT, (WPARAM)g_font_default, MAKELPARAM(FALSE, 0) );

	//  add items of the tab
	TCITEM tie;	tie.mask = TCIF_TEXT | TCIF_IMAGE; tie.iImage = -1; 
	
	tie.pszText = L"Log"; 
	if ( TabCtrl_InsertItem( g_tab, 0, &tie ) == -1 ) { 
		DestroyWindow( g_tab );
		return; 
	}

	tie.pszText = g_network->isServer ? L"Clients" : L"Servers"; 
	if ( TabCtrl_InsertItem( g_tab, 1, &tie ) == -1 ) { 
		DestroyWindow( g_tab );
		return; 
	}
} 

void create_log_text_control( void )
{
	// Create an edit box
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL;
	g_logText = CreateWindowEx( WS_EX_CLIENTEDGE, L"EDIT", L"log ...\r\n", dwStyle, 10, 40, 200, 200, g_tab, (HMENU)ID_LOG_TEXT, g_hInst, NULL );
	SendMessage( g_logText, WM_SETFONT, (WPARAM)g_font_courier_new, MAKELPARAM( FALSE, 0 ) );

	SendMessage( g_logText, EM_LIMITTEXT, MEMO_MAX_LENGTH, 0 );
}

void create_list_control( void )
{
	// Create an list box
	DWORD dwStyle = WS_CHILD | WS_VSCROLL | WS_HSCROLL | LBS_HASSTRINGS | LBS_NOTIFY;
	g_scList = CreateWindowEx( WS_EX_CLIENTEDGE, L"LISTBOX", L"", dwStyle, 10, 40, 200, 200, g_tab, (HMENU)ID_SC_LIST, g_hInst, NULL );
	SendMessage( g_scList, WM_SETFONT, (WPARAM)g_font_courier_new, MAKELPARAM( FALSE, 0 ) );
}

int list_get_item_index( void )
{
	LRESULT index = SendMessage( g_scList, LB_GETCURSEL, 0, 0 );
	if ( index == LB_ERR ) return -1;
	return (int)index;
}

void list_update_client( float elpsTime )
{
	if ( g_network->isServer ) return;

	static float updateTime = 1000;
	updateTime += elpsTime;
	if ( updateTime < 200 ) return;
	updateTime = 0;

	int index = list_get_item_index();
	wchar currentName[255]; ZeroMemory( currentName, sizeof(currentName) );
	if ( index > -1 )
		SendMessage( g_scList, LB_GETTEXT, index, (LPARAM)currentName );

	index = 0;
	SendMessage( g_scList, LB_RESETCONTENT, 0, 0 );

	for ( int i=0; i<g_network->client.m_servers.Count(); i++ )
	{
		NetAddress* pInfo = &g_network->client.m_servers[i].address;
		
		str512 tmpStr;
		tmpStr.Format( L"%d.%d.%d.%d:%d %s", pInfo->ip[0], pInfo->ip[1], pInfo->ip[2], pInfo->ip[3], pInfo->port, g_network->client.m_servers[i].name );
		SendMessage( g_scList, LB_ADDSTRING, NULL, (LPARAM)tmpStr.Text() );
	}

	if ( currentName[0] )
		SendMessage( g_scList, LB_SELECTSTRING, -1, (LPARAM)currentName );

}

void list_update_server( float elpsTime )
{
	if ( !g_network->isServer ) return;

	static float updateTime = 1000;
	updateTime += elpsTime;
	if ( updateTime < 200 ) return;
	updateTime = 0;

	int index = list_get_item_index();
	wchar currentName[255]; ZeroMemory( currentName, sizeof(currentName) );
	if ( index > -1 )
		SendMessage( g_scList, LB_GETTEXT, index, (LPARAM)currentName );

	index = 0;
	SendMessage( g_scList, LB_RESETCONTENT, 0, 0 );

	for ( int i=0; i<g_network->server.m_clients.Count(); i++ )
	{
		Connection* pCon = g_network->server.m_clients[i];
		NetAddress* pAdr = &pCon->m_destination;

		//if ( pCon->m_name.Length() && pAdr->ip[3] && pAdr->port )
		{
			str512 tmpStr;
			tmpStr.Format( L"%d.%d.%d.%d:%d %s", pAdr->ip[0], pAdr->ip[1], pAdr->ip[2], pAdr->ip[3], pAdr->port, pCon->m_name.Text() ? pCon->m_name.Text() : L"" );
			SendMessage( g_scList, LB_ADDSTRING, NULL, (LPARAM)tmpStr.Text() );
		}
	}

	if ( currentName[0] )
		SendMessage( g_scList, LB_SELECTSTRING, -1, (LPARAM)currentName );
}

void create_text_control( void )
{
	// Create an edit box
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP;
	g_text = CreateWindowEx( WS_EX_CLIENTEDGE, L"EDIT", L"", dwStyle, 10, 100, 200, 30, g_winMain, (HMENU)ID_TEXT, g_hInst, NULL );
	SendMessage( g_text, WM_SETFONT, (WPARAM)g_font_courier_new, MAKELPARAM( FALSE, 0 ) );
	g_text_proc = (WNDPROC)SetWindowLong( g_text, GWL_WNDPROC, (LONG)&InputLineWndProc );
}

void create_btn_send( void )
{
	// Create a push button
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP;
	g_btnSend = CreateWindowEx( NULL, L"BUTTON", L"Send", dwStyle, 150, 150, 50, 30, g_winMain, (HMENU)ID_BTN_SEND, g_hInst, NULL );
	SendMessage( g_btnSend, WM_SETFONT, (WPARAM)g_font_default, MAKELPARAM( FALSE, 0 ) );	
}

void append_text_to_control( HWND control, const wchar* text )
{
	if ( !text ) return;
	
	int i;
	wchar buffer[2048];

	for ( i=0; text[i] && i<2045; i++)
		buffer[i] = text[i];
	buffer[i++] = '\r';
	buffer[i++] = '\n';
	buffer[i] = 0;

	//	verify that text box if full or not
	static int maxlen = 0;
	maxlen += i;
	if ( maxlen > MEMO_MAX_LENGTH )
	{
		SendMessage( control, EM_SETSEL, 0, 5000 );
		SendMessage( control, EM_REPLACESEL, TRUE, (LPARAM)L"" );
		maxlen -= 5000;
	}

	// append text to text edit
	SendMessage( control, EM_SETSEL, 99999999, -1 );
	SendMessage( control, EM_REPLACESEL, TRUE, (LPARAM)buffer );
}

void on_send_button( void )
{

	wchar buffer[256]; ZeroMemory( buffer, sizeof(buffer) );
	SendMessage( g_text, WM_GETTEXT, sizeof(buffer) / sizeof(buffer[0]), reinterpret_cast<LPARAM>(buffer) );
	SendMessage( g_text, WM_SETTEXT, null, null );

	if ( memcmp(buffer, L"changeName:", 22) == 0 )
	{
		g_network->client.m_name = ( buffer + 11 );
		g_network->server.m_name = ( buffer + 11 );

		append_text_to_control( g_logText, buffer );
		return;
	}

	int len = 0;
	char buf[256];
	for ( ; len<255 && buffer[len]; len++ )	buf[len]=(char)buffer[len];
	buf[len++]=0;

	if ( len < 2 ) return;

	g_network->server.Send( buf, len, true );
	g_network->client.Send( buf, len, true );

	str512 tmpStr;
	if ( g_network->isServer )
		tmpStr.Format( L"%s : %s", g_network->server.m_name.Text(), buffer );
	else
		tmpStr.Format( L"%s : %s", g_network->client.m_name.Text(), buffer );
	append_text_to_control( g_logText, tmpStr );
}

void callback_logger_server( const wchar* message )
{
	append_text_to_control( g_logText, message );
}

void callback_logger_client( const wchar* message )
{
	append_text_to_control( g_logText, message );
}

void callback_server( Server* server, Connection* client, const byte* buffer, const uint size )
{
	wchar wbuf[1024]; ZeroMemory( wbuf, sizeof(wbuf) );
	for ( uint i=0; i<size && i<1023 && buffer[i]; i++ )
		wbuf[i] = buffer[i];

 	str2048 tmpText;
	tmpText.Format( L"%s : %s", client->m_name.Text(), wbuf );
 	append_text_to_control( g_logText, tmpText );
}

void callback_client( Client* client, const byte* buffer, const uint size )
{
	wchar wbuf[1024]; ZeroMemory( wbuf, sizeof(wbuf) );
	for ( int i=0; i<1023 && buffer[i]; i++ )
		wbuf[i] = buffer[i];

	str2048 tmpText;
	tmpText.Format( L"%s : %s", client->m_connection.m_name.Text(), wbuf );
	append_text_to_control( g_logText, tmpText );
}

static float msgtime = 0;
static int msgId = 0;
void MainLoop( float elpsTime )
{
	NetState stateChanged = STOPPED;

	if ( g_network->isServer )
	{
		NetState state = g_network->server.m_clients[0]->m_state;
		g_network->server.Update( elpsTime, NET_DELAY_TIME, NET_TIME_OUT );
		if ( state != g_network->server.m_clients[0]->m_state )
			stateChanged = g_network->server.m_clients[0]->m_state;
		list_update_server( elpsTime );
	}
	else
	{
		NetState state = g_network->client.m_connection.m_state;
		if ( 1 && state == CONNECTED )
		{
			float pressure = g_network->client.GetPressure();
			msgtime += elpsTime - pressure * 0.2f;
//			if ( msgtime < 0 ) msgtime = 0;
			if ( msgtime > ( NET_DELAY_TIME + pressure * 100.0f ) )
			{
				msgtime = 0;

				for( int i=0; i<10; i++ )
				{
					msgId++;
					char buf[512];
					for ( int c=0; c < 128; ++c )
						buf[c] = (char)( ( (c+1) * msgId ) );
					buf[127] = 0;
					g_network->client.Send( buf, 128, true );
				}
			}
		}
		g_network->client.Update( elpsTime, NET_DELAY_TIME, NET_TIME_OUT );
		if ( state != g_network->client.m_connection.m_state )
			stateChanged = g_network->client.m_connection.m_state;
		list_update_client( elpsTime );
	}

	switch ( stateChanged )
	{
	case LISTENING:		append_text_to_control( g_logText, L"Listening ..." );		break;
	case CONNECTING:	append_text_to_control( g_logText, L"Connecting ..." );		break;
	case CONNECTED:		append_text_to_control( g_logText, L"Connected ..." );		break;
	case DISCONNECTED:	append_text_to_control( g_logText, L"Disconnected ..." );	break;
	}
	
	sx_os_sleep( 1 );
}

LRESULT WINAPI InputLineWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int key;
	switch ( uMsg )
	{
	case WM_KEYFIRST:
	case WM_KEYLAST:
		key = (int)wParam;

		if ( key == VK_RETURN ) {
			on_send_button();
			MSG emsg;
			PeekMessage( &emsg, hWnd, 0, 0, PM_REMOVE );
			return 0;
		}
		else if ( key == VK_TAB ) {
			SetFocus( g_btnSend );
			MSG emsg;
			PeekMessage( &emsg, hWnd, 0, 0, PM_REMOVE );
			return 0;
		}
		break;
	}
	return CallWindowProc( g_text_proc, hWnd, uMsg, wParam, lParam );
}

int EventCallback( class Window* Sender, const WindowEvent* data )
{
	if ( Sender && !g_winMain )
		g_winMain = *( (HWND*)Sender->GetHandle() );

	switch(data->msg)
	{
	case WM_CREATE:
		{
			create_tab_control();
			create_log_text_control();
			create_list_control();
			create_text_control();
			create_btn_send();
		}
		break;

	case WM_SIZE:
		{
			RECT rc; 

			GetClientRect( g_winMain, &rc );
			SetWindowPos( g_tab,	0,	0,	0, rc.right, rc.bottom, 0 );
			SetWindowPos( g_logText, 0, 10, 40, rc.right-20, rc.bottom-120, 0 );
			SetWindowPos( g_scList, 0, 10, 40, rc.right-20, rc.bottom-120, 0 );
			SetWindowPos( g_text,	0,	10, rc.bottom-75, rc.right-20, 20, 0 );
			SetWindowPos( g_btnSend,0,	rc.right-110, rc.bottom-40, 100, 30, 0 );
		}
		break;

	case WM_NOTIFY:
		{
			NMHDR* notify = (NMHDR*)(data->lparam);
			if( notify->hwndFrom == g_tab )
			{
				if ( notify->code == TCN_SELCHANGE )
				{
					int curIndex = TabCtrl_GetCurSel( g_tab );
					if ( curIndex )
					{
						ShowWindow( g_logText,	SW_HIDE );
						ShowWindow( g_scList,	SW_SHOW );

						if( g_network->isServer )
						{
							//g_network->server.Listen(2727);
						}
						else
						{
							g_network->client.Disconnect();
							g_network->client.Listen();
						}
					}
					else
					{
						ShowWindow( g_logText,	SW_SHOW );
						ShowWindow( g_scList,	SW_HIDE );

						if ( !g_network->isServer )
						{
							int index = list_get_item_index();
							if ( index > -1 && index < g_network->client.m_servers.Count() )
							{
								NetAddress adr;
								memcpy( adr.ip, g_network->client.m_servers[index].address.ip, 4 );
								adr.port = g_network->client.m_servers[index].address.port;
								g_network->client.Connect( adr );
							}
						}
					}
				}
			}
		}
		break;


	case WM_COMMAND:
		if ( LOWORD( data->wparam ) == ID_BTN_SEND )
			on_send_button();
		break;
	}

	return data->msg;
}




//////////////////////////////////////////////////////////////////////////
//	START POINT OF PROGRAM
//////////////////////////////////////////////////////////////////////////
sint APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, sint nCmdShow)
{
	
	{
		str256 commandLine = lpCmdLine;

		//  parse the command line
		if ( commandLine.Find(L"client") > -1 )
			g_network->isServer = false;
		else
			g_network->isServer = true;
	}

	String consoleName = L"SeganX Console :: "; consoleName << ( g_network->isServer ? L"server" : L"client" );

	//  make single application
	HANDLE mutex = CreateMutex(NULL, TRUE, *consoleName);
	if ( !mutex || GetLastError() == ERROR_ALREADY_EXISTS )
		return 0;
	g_hInst = hInstance;

// 	// make sure we're running in the exe's path
// 	wchar path[MAX_PATH];
// 	if ( GetModuleFileName(null, path, sizeof(path)) )
// 	{
// 		wchar* slash = wcsrchr(path, PATH_PART);
// 		if (slash) *slash = '\0';
// 		SetCurrentDirectory(path);
// 	}

	//////////////////////////////////////////////////////////////////////////

	//  initialize engine
	{
		str256 logFile; logFile.Format( L"console_%s.txt", ( g_network->isServer ? L"server" : L"client" ) );
		LoggerConfig logconf;
		logconf.callback = g_network->isServer ? &callback_logger_server : &callback_logger_client;
		logconf.fileName = (wchar*)logFile.Text();
		logconf.mode = LM_FILE;
		logconf.name = (wchar*)consoleName.Text();

		EngineConfig enconf;
		enconf.logger = &logconf;
		enconf.window_event_callback = &EventCallback;
		g_engine = sx_engine_get_singleton( &enconf );
		sx_engine_initialize();
	}
	//////////////////////////////////////////////////////////////////////////

	MonitorInfo* monInfo = sx_os_get_desktop();
	Window* winMain = sx_app_create_window( consoleName, WBT_ORDINARY_RESIZABLE, false );
	winMain->SetRect( monInfo->workingWidth - 400, monInfo->workingHeight - 500, 350, 400 );
	winMain->SetVisible( true );

	if ( g_network->isServer )
	{
		g_network->server.m_name = L"server";
		g_network->server.Start( 2828, 2727, 10, callback_server );
		g_network->server.Listen();
	}
	else
	{
		g_network->client.m_name = L"client";
		g_network->client.Start( 2727, callback_client );
		g_network->client.Listen();

	}

	sx_engine_start( &MainLoop );

	if ( g_network->isServer )
		g_network->server.Stop();
	else
		g_network->client.Stop();

	sx_engine_finalize();

	//  close handles
	CloseHandle(mutex);

	sx_detect_crash();

	return 0;
}