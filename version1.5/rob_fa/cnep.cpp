#include "resource.h"
#include "GameTypes.h"
#include "Game.h"
#include "GameConfig.h"
#include "Mechanic_Cinematic.h"
#include "gameup_import.h"
#include <stdio.h>

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

	if ( g_game->m_app_Paused )
	{
		Sleep(50);
		if ( g_game->m_game_currentLevel && !g_game->m_game_paused )
			g_game->PostMessage( 0, GMT_GAME_PAUSED, NULL );
		sx::snd::Device::Pause( true );
		return;
	}

	if ( g_game->m_app_Loading )
	{
		for (int i=0; i<elpsTime; i++)
			sx::sys::TaskManager::Update(0);
	}

	WindowRect wr;
	s_window.GetRect(wr);
	SetCursorPos( wr.Left + wr.Width/2, wr.Top + wr.Height/2 );

	sx::io::Input::Update(elpsTime);
	int2 ioSize( (int)Config::GetData()->display_Size.x, (int)Config::GetData()->display_Size.y );
	sx::io::Input::SendSignal(0, IST_SET_SIZE, &ioSize);

	sx::gui::Control::GetCapturedControl() = NULL;

	g_game->Update(elpsTime);

	g_game->Render( 0 );

}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	sx_callstack();

#if USE_GAMEUP
	GameUp localgameup;
	g_gameup = &localgameup;
	if ( !gameup_init( g_gameup ) ) return 0;
#endif

	//  make single application
	String mutexName = L"SeganX Game :: "; mutexName << GAME_TITLE;
	HANDLE mutex = CreateMutex(NULL, TRUE, *mutexName);
	if ( !mutex || GetLastError() == ERROR_ALREADY_EXISTS )
		return 0;
	sx::cmn::Randomize();
	

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

	//  load configuration
#if USE_GAMEUP
	bool b1 = ( g_gameup->get_lock_code(0) == g_gameup->get_lock_code(5) );
	if ( b1 )
#endif
		Config::LoadConfig();

	// TEST
	String str = sx::sys::GetAppFolder();
	str.MakePathStyle();
#if USE_GAMEUP
	if ( g_gameup->get_lock_code(1) == g_gameup->get_lock_code(2) )
#endif
		str << L"project1";
	sx::sys::FileManager::Project_Open(str, FMM_ARCHIVE);


	//  create application window
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
		sx::sys::Application::Create_Window(&s_window);
	ShowCursor( FALSE );


	//  initialize scene manager
	sx::core::Scene::Initialize( sx_new( sx::core::SceneManager_SBVH ) );

	//  initialize renderer
	sx::core::Renderer::Initialize( sx_new( sx::core::Pipeline_Forward) );

	//  create main graphic device
	sx::core::Renderer::SetSize( s_window.GetHandle(), 0, 0, Config::GetData()->device_CreationFlag );

	//  initialize sounds and music
	sx::snd::Device::Create( s_window.GetHandle(), SX_SND_3D /*| SX_SND_SYNC*/ );

	//  at first connect keyboard
	sx::io::PInputDeviceBase newDevice = sx_new( sx::io::Keyboard(0) );

#if USE_GAMEUP
	if ( g_gameup->get_lock_code(0) == g_gameup->get_lock_code(7) )
#endif
		sx::io::Input::Attach( newDevice );

	//  connect mouse
	if ( newDevice )
		newDevice = sx_new( sx::io::Mouse(0) );
	sx::io::Input::Attach( newDevice );

	//  initialize IO services
	if ( s_window.GetHandle())
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

		if ( !b1 ) return 0;
	}
#else
	Game::Initialize( &s_window );
#endif

	//  TEST 
	g_game->m_game_nextLevel = 0;	//  set level to first test
	//g_game->m_upgrades.trap_cooltime = 8.0f;
	//g_game->m_upgrades.trap_count = 5;


	//	show presents
#if 1
	{
		FirstPresents *presents;
		presents = sx_new( FirstPresents );
		presents->AddPresents( L"gui_parseh.txr", 512 );
		presents->AddPresents( L"gui_esra.txr", 1024 );

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

	//  clear the game
	g_game->ClearLevel();

	//  finalize game object to destroy created resources
	Game::Finalize();

	Config::SaveConfig();

	//  finalize some remain things
	sx::core::Renderer::Finalize();
	sx::core::Scene::Finalize();
	sx::snd::Device::Destroy();
	sx::io::Input::Finalize();

	//  turn screen saver off
	sx::sys::ScreenSaverSetDefault();

#if USE_GAMEUP
	gameup_finit( g_gameup );
#endif

	//  close handles
	CloseHandle( mutex );

#if NET_ACTIVATE
	sx_delete_and_null( client );
	sx_net_finalize();
#endif

	sx_detect_crash();

	return 0;
}
