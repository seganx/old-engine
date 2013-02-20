#include "resource.h"
#include "GameTypes.h"
#include "Game.h"
#include "GameConfig.h"



#define NET_ACTIVATE	1
#define NET_DELAY_TIME	60
#define NET_TIMEOUT		60000

//////////////////////////////////////////////////////////////////////////
//  SOME GLOBAL VARIABLES
Client* client = null;

//////////////////////////////////////////////////////////////////////////
//		static internal variables and objects
static sx::sys::Window		s_window;			//	main application window
//////////////////////////////////////////////////////////////////////////

class FirstPresents
{
public:
	FirstPresents( void ): m_index(0), m_time(0)
	{
	
	}

	~FirstPresents()
	{
		for ( int i=0; i<m_list.Count(); i++ )
		{
			sx::gui::Destroy( m_list[i] );
		}
	}

	void AddPresents( const WCHAR* texture, const float size )
	{
		sx::gui::Panel* panel = (sx::gui::Panel*)sx::gui::Create( GUI_PANEL );
		panel->SetSize( float2( size, size ) );
		panel->GetElement(0)->SetTextureSrc( texture );
		panel->GetElement(0)->Color().a = 0;

		m_list.PushBack( panel );
	}

	void Update( float elpstime )
	{
		
		if ( m_index > m_list.Count() ) return;

		m_time += elpstime;
		if ( m_time > 3000 )
		{
			m_time = 0;
			m_index++;
		}

		for ( int i=0; i<m_list.Count(); i++ )
		{
			sx::gui::Control* pc = m_list[i];

			if ( m_index == i )
			{
				pc->GetElement(0)->Color().a += elpstime * 0.001f;
				if ( pc->GetElement(0)->Color().a > 1.0f )
					pc->GetElement(0)->Color().a = 1.0f;
			}
			else
			{
				pc->GetElement(0)->Color().a -= elpstime * 0.001f;
				if ( pc->GetElement(0)->Color().a < 0.0f )
					pc->GetElement(0)->Color().a = 0.0f;
			}

			pc->Update( elpstime );
		}
	}

	void Draw( void )
	{
		if ( !sx::core::Renderer::CanRender() ) return;
		sx::core::Renderer::Begin();

		sx::d3d::UI3D::ReadyToDebug( D3DColor(0,0,0,0) );
		sx::d3d::Device3D::Clear_Target(0);
		sx::d3d::Device3D::RS_Alpha( SX_MATERIAL_ALPHABLEND );

		for ( int i=0; i<m_list.Count(); i++ )
		{
			m_list[i]->Draw(0);
		}

		sx::core::Renderer::End();
	}

	bool Presenting( void )
	{
		return ( m_index <= m_list.Count() && m_time < 3000 );
	}


public:
	Array<sx::gui::Control*>	m_list;
	int							m_index;
	float						m_time;
};

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
	client->Update( elpsTime, NET_DELAY_TIME, NET_TIMEOUT );

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
		client->m_name.Format( L"editor %s", sx::sys::GetUserName() );
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
			Sleep(50);
		}
	}
	sxLog::SetCallback( loggerCallback );
#endif

	//  load configuration
	Config::LoadConfig();

	// TEST
	String str = sx::sys::GetAppFolder();
	str.MakePathStyle();
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
	sx::sys::Application::Create_Window(&s_window);

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
	sx::io::Input::Attach( newDevice );

	//  connect mouse
	newDevice = sx_new( sx::io::Mouse(0) );
	sx::io::Input::Attach( newDevice );

	//  initialize IO services
	sx::io::Input::Initialize( s_window.GetHandle() );

	//  turn screen saver off
	sx::sys::ScreenSaverSetActivity(false);

	//  keep system wakeful
	sx::sys::KeepSystemWakeful();

	//  initialize game object to create necessary resources
	Game::Initialize( &s_window );

	//  TEST 
	g_game->m_game_nextLevel = 0;	//  set level to first test
	//g_game->m_upgrades.trap_cooltime = 8.0f;
	//g_game->m_upgrades.trap_count = 5;


	//	show presents
#if 0
	float present_size = (float)min( Config::GetData()->display_Size.x, Config::GetData()->display_Size.y );
	FirstPresents *presents = sx_new( FirstPresents );
	presents->AddPresents( L"gui_loading_level_0.txr", present_size );
	presents->AddPresents( L"gui_loading_level_1.txr", present_size );
	presents->AddPresents( L"gui_loading_level_2.txr", present_size );

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

	//  close handles
	CloseHandle( mutex );

#if NET_ACTIVATE
	sx_delete_and_null( client );
	sx_net_finalize();
#endif

	sx_detect_crash();

	return 0;
}
