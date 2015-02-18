#include <stdio.h>
#include "resource.h"
#include "GameTypes.h"
#include "Game.h"
#include "GameConfig.h"
#include "Mechanic_Cinematic.h"
#include "gameup_import.h"
#include "ComicPlayer.h"
#include "GameStrings.h"

#if USE_HASH_LOCK
#include "hashlock/lock_wnd.h"
#endif

#if USE_SITE_STATS
#include <WinInet.h>
#pragma comment( lib, "wininet.lib" )
#endif

#if USE_STEAM_SDK
#include <steam/steam_api.h>
#pragma comment( lib, "steam_api.lib" )
#endif


#define NET_ACTIVATE	0
#define NET_DELAY_TIME	60
#define NET_TIMEOUT		60000

//////////////////////////////////////////////////////////////////////////
//  SOME GLOBAL VARIABLES
Client* client = null;

//////////////////////////////////////////////////////////////////////////
//		static internal variables and objects
static sx::sys::Window		s_window;			//	main application window

#if USE_GAMEUP
extern GameUp*				g_gameup = null;
#endif
//////////////////////////////////////////////////////////////////////////

#if USE_HASH_LOCK
bool load_hash_lock_code( wchar* dest )
{
	String fileName = sx::sys::GetDocumentsFolder();
	fileName.MakePathStyle();
	fileName << L"RushForGlory";
	fileName << L"/serial.lock";

	sx::sys::FileStream file;
	if ( file.Open( fileName, FM_OPEN_READ ) )
	{
		wchar tmp[128] = {0};
		file.Read( tmp, 128 );
		sx_decrypt( dest, tmp, 128, 128 );
		file.Close();
		return true;
	}
	return false;
}

void save_hash_lock_code( wchar* code )
{
	String fileName = sx::sys::GetDocumentsFolder();
	fileName.MakePathStyle();
	fileName << L"RushForGlory";
	fileName << L"/serial.lock";

	sx::sys::FileStream file;
	if ( file.Open( fileName, FM_CREATE ) )
	{
		wchar tmp[128] = {0};
		sx_encrypt( tmp, code, 128, 128 );
		file.Write( tmp, 128 );
		file.Close();
	}
}

#endif

#if USE_SITE_STATS
DWORD sx_sys_id( void )
{
	DWORD dwDiskSerial = 0;
	GetVolumeInformation(L"C:\\", 0, 0, &dwDiskSerial, 0, 0, 0, 0);
	return dwDiskSerial;
}
void sx_web_request( uint* receivedsize, wchar* result, const uint resultsize, const wchar* host, const wchar* uri, const void* data, const uint sizeinbytes )
{
	// Initialize the Internet DLL
	HINTERNET hSession = ::InternetOpen( L"RFG", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
	if ( hSession == NULL )
	{
		swprintf_s( result, 64, L"Internet session initialization failed!" );
		return;
	}

	//	connect to server
	HINTERNET hConnection = ::InternetConnect( hSession, host, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
	if ( hConnection == NULL )
	{
		swprintf_s( result, 64, L"Internet connection failed!" );
		::InternetCloseHandle(hSession);
		return;
	}

	// open a request
	HINTERNET hRequest = ::HttpOpenRequest( hConnection, TEXT("POST"), uri, TEXT("HTTP/1.1"), NULL, NULL, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI, NULL );
	if ( hRequest == NULL )
	{
		swprintf_s( result, 64, L"Internet opening request failed!" );
		::InternetCloseHandle(hConnection);
		::InternetCloseHandle(hSession);
		return;
	}

	wchar httpHeader[128] = {0};
	int lenhttpHeader = swprintf_s( httpHeader, 128, 
		//L"Host : %s\n"
		L"Accept: text/html, application/xhtml+xml, */*\n"
		//L"Accept-Language: en-US\n"
		//L"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko\n"
		//L"Accept-Encoding: gzip, deflate\n"
		//L"DNT: 1\n"
		//L"Connection: Keep-Alive\n"
		L"Content-Type: application/x-www-form-urlencoded\n"
		//, host
		);
	if ( HttpSendRequest( hRequest, httpHeader, lenhttpHeader, (char*)data, sizeinbytes ) == FALSE )
	{
		DWORD errcode = GetLastError();
		swprintf_s( result, 64, L"Internet sending request failed! code : %d ", errcode );
		::InternetCloseHandle(hConnection);
		::InternetCloseHandle(hSession);
		return;
	}

	//  read the file
	{
		byte* res = (byte*)result;
		uint maxresultsize = resultsize * sizeof(wchar);
		uint pos = 0;
		DWORD recieved;
		do 
		{
			recieved = 0;
			::InternetReadFile( hRequest, res, (maxresultsize > 1024) ? 1024 : maxresultsize, &recieved );
			pos += recieved;
			res += recieved;
			maxresultsize -= recieved;

		} while ( recieved );

		if ( receivedsize )
			*receivedsize = pos;
	}

	// close opened handles
	::InternetCloseHandle(hRequest);
	::InternetCloseHandle(hConnection);
	::InternetCloseHandle(hSession);
}
#endif

void clientCallback( Client* client, const byte* buffer, const uint size )
{
	String str = (char*)buffer;
	if ( str == L"game:exit" )
	{
		sx::sys::Application::Terminate();
	}
}

void loggerCallback( const wchar* message )
{
#if NET_ACTIVATE
	if ( !message || !client ) return;
	sx_callstack();

	char msg[512] = {0};

	int i = 0;
	wchar* c = (wchar*)message;
	while ( *c && i<511 )
	{
		msg[i] = (char)(*c);
		c++;
		i++;
	}
	msg[i++]=0;

	client->Send( msg, i, false );
	client->Update( 0, NET_DELAY_TIME, NET_TIMEOUT );
#endif
}

UINT MainMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATEAPP:
		{
			if (wParam == FALSE)
			{
				g_game->m_app_Paused = true;
			}
			else
			{
				g_game->m_app_Paused = false;
			}
		}
		break;
	}

	return msg;
}

void MainLoop(float elpsTime)
{
	sx_callstack();

#if NET_ACTIVATE
	client->Update( elpsTime, NET_DELAY_TIME, NET_TIMEOUT );
#endif

	if ( elpsTime > 1000 ) return;

#if USE_STEAM_SDK
	SteamAPI_RunCallbacks();
#endif

	if ( g_game->m_app_Paused )
	{
		Sleep(25);
		if ( g_game->m_game_currentLevel && !g_game->m_game_paused )
			g_game->PostMessage( 0, GMT_GAME_PAUSED, NULL );
		sx::snd::Device::Pause( true );
	}

	if ( g_game->m_app_Loading )
	{
		for (int i=0; i<elpsTime; i++)
			sx::sys::TaskManager::Update(0);
	}


	if ( !g_game->m_app_Paused )
	{
		WindowRect wr;
		s_window.GetRect(wr);
		SetCursorPos( wr.Left + wr.Width/2, wr.Top + wr.Height/2 );

		sx::io::Input::Update(elpsTime);
		int2 ioSize( (int)Config::GetData()->display_Size.x, (int)Config::GetData()->display_Size.y );
		sx::io::Input::SendSignal(0, IST_SET_SIZE, &ioSize);
	}

	sx::gui::Control::GetCapturedControl() = NULL;

	g_game->Update(elpsTime);

	g_game->Render( 0 );

}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	sx_engine_init();

	sx_callstack();

#if USE_GAMEUP
	GameUp localgameup;
	g_gameup = &localgameup;
	if ( !gameup_init( g_gameup ) ) return 0;
#endif

#if USE_HASH_LOCK
	wchar hash_lock_code[64] = {0};
	bool hash_lock_loaded = load_hash_lock_code( hash_lock_code );
	int hash_res[3] = {0};
#endif


	//  make single application
#if 1
	String mutexName = L"SeganX Game :: "; mutexName << GAME_TITLE;
	HANDLE mutex = CreateMutex(NULL, TRUE, *mutexName);
	if ( !mutex || GetLastError() == ERROR_ALREADY_EXISTS )
	{
		sx_engine_finit();
		return 0;
	}
#endif
	sx::cmn::Randomize();
	

#if USE_STEAM_SDK
	if ( SteamAPI_RestartAppIfNecessary(STEAM_APP_ID) )
	{
		MessageBox( NULL, L"Can't initialize Steam !", L"Steam API", MB_OK | MB_ICONERROR );
		sx_engine_finit();
		return 0;
	}
	if ( !SteamAPI_Init() )
	{
		MessageBox( NULL, L"Can't initialize Steam !", L"Steam API", MB_OK | MB_ICONERROR );
		sx_engine_finit();
		return 0;
	}

	//SteamUtils()->SetOverlayNotificationPosition(k_EPositionBottomLeft);
#endif

	//////////////////////////////////////////////////////////////////////////
	//	connect to console
#if NET_ACTIVATE
	{
		//	initialize net system
		sx_net_initialize( 0x27272727 );

		client = sx_new( Client );
		client->m_name.Format( L"RoB %s", sx::sys::GetUserName() );
		client->Start( 2727, clientCallback );
		client->Listen();
		int tryToConnect = 0;
		while ( tryToConnect < 500 )
		{
			client->Update( 10, NET_DELAY_TIME, NET_TIMEOUT );

			if ( client->m_servers.Count() )
			{
				client->Connect( client->m_servers[0].address );

				client->Update( 10, NET_DELAY_TIME, NET_TIMEOUT );
				break;
			}

			tryToConnect++;
			Sleep(10);
		}
	}
	sxLog::SetCallback( loggerCallback );
#endif

#if USE_HASH_LOCK
	uint hash_systime = sx::sys::GetSysTime_u();
	int hashlockmain = 0;
	if ( hash_lock_loaded )
		hashlockmain = 1;
	else
		hashlockmain = show_hash_lock( hInstance, hash_lock_code );
	if ( !hashlockmain )
	{
		sx_engine_finit();
		return 0;
	}
#endif

	//  load configuration
	{
#if USE_GAMEUP
		bool b1 = ( g_gameup->get_lock_code(0) == g_gameup->get_lock_code(5) );
		if ( b1 )
#endif

#if USE_HASH_LOCK
		if ( hashlockmain )
#endif
			Config::LoadConfig();
	}

#if USE_HASH_LOCK
	uint verified_hash_lock_code = verify_hash_lock_code( hash_systime, hash_lock_code, hash_res );
#endif

	{
		String str = sx::sys::GetAppFolder();
		str.MakePathStyle();
#if USE_GAMEUP
		if ( g_gameup->get_lock_code(1) == g_gameup->get_lock_code(2) )
#endif
#if USE_HASH_LOCK
		if ( hash_res[0] == HASH_VERIFY_RES1(hash_systime) )
#endif
		str << L"project1";
		sx::sys::FileManager::Project_Open(str, FMM_ARCHIVE);
	}

#if USE_HASH_LOCK
	if ( verified_hash_lock_code )
		save_hash_lock_code( hash_lock_code );
#endif

	//  create application window
	{
		WindowRect wr;
		wr.Width = Config::GetData()->display_Size.x;
		wr.Height = Config::GetData()->display_Size.y;
		wr.Left = (sx::sys::GetDesktopWidth() - wr.Width) / 2;
		wr.Top = (sx::sys::GetDesktopHeight() - wr.Height) / 4;
		s_window.SetRect(wr);
		s_window.SetBorder( WBT_ORDINARY );
		s_window.SetTitle( GAME_TITLE );
		s_window.SetIcon( LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME)) );
		s_window.SetCursor( LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR_EMPTY)) );
		s_window.SetVisible( true );
#if USE_GAMEUP
		if ( g_gameup->get_lock_code(6) == g_gameup->get_lock_code(3) )
#endif
#if USE_HASH_LOCK
			if ( hash_res[1] == HASH_VERIFY_RES2(hash_systime) )
#endif
				sx::sys::Application::Create_Window(&s_window);
		SetForegroundWindow( s_window.GetHandle() );
		ShowCursor( FALSE );
	}

	//  initialize scene manager
	sx::core::Scene::Initialize( sx_new( sx::core::SceneManager_SBVH ) );

	//  initialize renderer
	sx::core::Renderer::Initialize( sx_new( sx::core::Pipeline_Forward) );

	//  create main graphic device
	sx::core::Renderer::SetSize( s_window.GetHandle(), Config::GetData()->display_Size.x, Config::GetData()->display_Size.y, Config::GetData()->device_CreationFlag );

	//  initialize sounds and music
	sx::snd::Device::Create( s_window.GetHandle(), SX_SND_3D /*| SX_SND_SYNC*/ );

	//  at first connect keyboard
	{
		sx::io::PInputDeviceBase newDevice = sx_new( sx::io::Keyboard(0) );

#if USE_GAMEUP
		if ( g_gameup->get_lock_code(0) == g_gameup->get_lock_code(7) )
#endif
#if USE_HASH_LOCK
		if ( verified_hash_lock_code == HASH_VERIFY_RES3(hash_systime) )
#endif
			sx::io::Input::Attach( newDevice );

		//  connect mouse
		if ( newDevice )
			newDevice = sx_new( sx::io::Mouse(0) );

		sx::io::Input::Attach( newDevice );
	}

	//  initialize IO services
	if ( s_window.GetHandle() )
	sx::io::Input::Initialize( s_window.GetHandle() );

	//  turn screen saver off
	sx::sys::ScreenSaverSetActivity(false);

	//  keep system wakeful
	sx::sys::KeepSystemWakeful();

	//  initialize game object to create necessary resources
#if USE_GAMEUP
	{
		int a = g_gameup->get_lock_code(4);
		int b = g_gameup->get_lock_code(1) * 3;
		Game::Initialize( (a == (b * 3)) ? &s_window : null );

		if ( !b1 )
		{
			sx_engine_finit();
			return 0;
		}
	}
#elif USE_HASH_LOCK
	{ 
		int a = hash_res[0];
		int b = hash_res[1];
		Game::Initialize( (a*b == HASH_VERIFY_RES3(hash_systime)) ? &s_window : null );
	}
#else
	Game::Initialize( &s_window );
#endif

	//	show presents
#if 1
	{
		ComicPlayer comic;
		comic.Load( g_game->m_strings->Get(2001)->text );

		float initTime = sx::sys::GetSysTime();
		float elpsTime = 0;
		while ( comic.Playing() )
		{
			// calculate elapsed time
			elpsTime = sx::sys::GetSysTime() - initTime;
			initTime = sx::sys::GetSysTime();
			comic.Update( elpsTime );
			comic.Draw();
		}
	}
	{
		FirstPresents *presents = sx_new( FirstPresents );

#if USE_ALAWAR_LOGO
		presents->AddPresents( L"gui_alawar.txr", 512, null, 0, 0 );
#endif

		presents->AddPresents( L"gui_parseh.txr", 512, null, 0, 0 );
		presents->AddPresents( L"gui_esra.txr", 1024, null, 0, 0 );

		float initTime = sx::sys::GetSysTime();
		float elpsTime = 0;
		while ( presents->Presenting() )
		{
			// calculate elapsed time
			elpsTime = sx::sys::GetSysTime() - initTime;
			initTime = sx::sys::GetSysTime();
			presents->Update( elpsTime );
			presents->Draw();
		}
		sx_delete_and_null( presents );
	}
#endif

	//  run the game
	sx::sys::Application::Run( MainLoop, MainMsgProc );

	//  finalize game object to destroy created resources
	Game::Finalize();

	Config::SaveConfig();

#if USE_STEAM_SDK
	SteamAPI_Shutdown();
#endif

	//  turn screen saver off
	sx::sys::ScreenSaverSetDefault();

#if USE_STEAM_LINK
	ShowCursor( TRUE );
	show_steam( hInstance );
#endif

#if USE_GAMEUP
	gameup_finit( g_gameup );
#endif

#if 1
	//  close handles
	CloseHandle( mutex );
#endif

#if NET_ACTIVATE
	sx_delete_and_null( client );
	sx_net_finalize();
#endif

	sx_detect_crash();


#if USE_SITE_STATS
	{
		uint tag = SITE_STATS_TAG;
		uint ressize = 0;
		char res[128] = {0};
		char post[256] = {0};
		uint postlen = sprintf_s(post, 256, "cpuid=%lu&tag=%u", sx_sys_id(), tag);
		sx_web_request( &ressize, (wchar*)res, 128, L"info.rushforglorygame.com", L"foo.php", post, postlen );
	}
#endif

	sx_engine_finit();
	return 0;
}
