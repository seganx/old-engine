// sxEditor.cpp : Defines the entry point for the application.
//
#include "sxEditor.h"
#include "MainEditor.h"
#include "MainWindow.h"
#include "EditorSettings.h"
#include "EditorScene.h"
#include "EditorObject.h"

#if NET_ACTIVATE
extern Client* g_client = null;
#endif


//! if LM_WINDOWS activated a pointer of this structure will post in WPARAM
struct msgLogger
{
	const WCHAR*	loggerName;		//  name of the logger
	const WCHAR*	logMessage;		//  null terminated string contain message of the logger

	msgLogger(const WCHAR* name, const WCHAR* msg): loggerName(name), logMessage(msg) {}
};

extern EditorWorkingSpace	g_CurSpace;		//  specify the current working space


//////////////////////////////////////////////////////////////////////////
//  SOME GLOBAL VARIABLES
int		g_AllowRender = EditorSettings::GetSleepTime();

void Loop_Start( void )
{
	g_AllowRender = EditorSettings::GetSleepTime();
}

void Loop_Stop( int value /*= 0*/ )
{
	g_AllowRender = value;
}

void clientCallback( Client* client, const byte* buffer, const uint size )
{

}


void loggerCallback( const wchar* message )
{
	sx_callstack();
#if NET_ACTIVATE
	if ( !message || !g_client ) return;

	Editor::SetLabelTips( message, 10000.0f );

// 	char msg[512] = {0};
// 
// 	int i = 0;
// 	wchar* c = (wchar*)message;
// 	while ( *c && i<511 )
// 	{
// 		msg[i] = (char)(*c);
// 		c++;
// 		i++;
// 	}
// 	msg[i++]=0;
// 
// 	g_client->Send( msg, i, false );
// 	g_client->Update( 0, NET_DELAY_TIME, NET_TIMEOUT );
#else
	if ( !message ) return;
	Editor::SetLabelTips( message, 10000.0f );
#endif
}

UINT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	if ( hWnd == MainWin::s_Win.GetHandle() )
	{
		switch (msg)
		{

		case WM_SX_D3D_RESET:
			{
				WindowRect rc; MainWin::s_Win.GetRect(rc);
				Editor::Resize(rc.Width, rc.Height);
			}
			break;

		case WM_ACTIVATE:
			{
				WindowRect rc;
				MainWin::s_Win.GetRect(rc);
				if ( rc.Width < 50 || rc.Height < 50 || HIWORD( wParam ) )
				{
					Loop_Stop(EDITOR_PAUSE);
				}
				else
				{
					switch ( LOWORD( wParam ) )
					{
					case WA_ACTIVE:		
						Loop_Start();
						break;

					case WA_INACTIVE:
						Loop_Stop(EDITOR_SLEEP);
						break;
					}
				}
			}
			return 0;

		case WM_ACTIVATEAPP:
			{
				WindowRect rc;
				MainWin::s_Win.GetRect(rc);
				if ( rc.Width < 50 || rc.Height < 50)
					Loop_Stop(EDITOR_PAUSE);
				else if (wParam == FALSE)
					Loop_Stop(EDITOR_SLEEP);
				else
					Loop_Start();
			}
			return 0;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		//case WM_MOUSEWHEEL:
		//case WM_KEYDOWN: if ( GetForegroundWindow() == hWnd ) Loop_Start();	break;
		case WM_KEYUP:	/*if ( LOWORD(wParam) == VK_ESCAPE )		sx::sys::Application::Terminate();*/		break;
		}
	}

	return msg;
}

void MainLoop(float elpsTime)
{
	sx_callstack();

#if NET_ACTIVATE
	g_client->Update( elpsTime, NET_DELAY_TIME, NET_TIMEOUT );
#endif

	//	force to fast task update
	for ( int i=0; i<60; i++ )
		sx::sys::TaskManager::Update( elpsTime );

	MainWin::s_Title = L"SeganX Editor";

	//////////////////////////////////////////////////////////////////////////
	//  control timing
	if ( g_AllowRender == EDITOR_PAUSE )
	{
		MainWin::s_Title << L" - pause ";
		Sleep( 200 );
		sx::snd::Device::Pause(true);
		elpsTime = 0;
	}
	else if ( g_AllowRender <= EDITOR_SLEEP )
	{
		MainWin::s_Title << L" - sleep ";
		Sleep( 100 );
		sx::snd::Device::Pause(true);
		elpsTime = 0;
	}
	else
	{
		MainWin::s_Title << L" - rendering ";
		g_AllowRender -= (int)sx::cmn::Round(elpsTime);
		sx::snd::Device::Pause(false);
	}

	switch (g_CurSpace)
	{
	case EWS_OBJECT:
		if ( EditorObject::frm_Toolbar->m_path_Save.Text() )
		{
			str1024 str = EditorObject::frm_Toolbar->m_path_Save.Text();
			if ( str.Length() > 30 )
			{
				str.Delete(0, str.Length() - 30 );
				str.Insert( L"..." );
			}
			MainWin::s_Title << L" [ " << str.Text() << L" ] ";
		}
		else
			MainWin::s_Title << L" [ Untitled ] ";
		break;

	case EWS_SCENE:
		if ( EditorScene::frm_Toolbar->m_path_Save.Text() )
		{
			str1024 str = EditorScene::frm_Toolbar->m_path_Save.Text();
			if ( str.Length() > 30 )
			{
				str.Delete(0, str.Length() - 30 );
				str.Insert( L"..." );
			}
			MainWin::s_Title << L" [ " << str.Text() << L" ] ";
		}
		else
			MainWin::s_Title << L" [ Untitled ] ";
		break;

	case EWS_HUD:
		break;
	case EWS_GAME:
		break;

	}


	if ( MainWin::s_Title != MainWin::s_Win.GetTitle() )
		MainWin::s_Win.SetTitle( MainWin::s_Title );

	if ( g_AllowRender != EDITOR_PAUSE )
	{
		Editor::Loop(elpsTime);
	}
}

//////////////////////////////////////////////////////////////////////////
//	START POINT OF PROGRAM
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	sx_callstack();

	//  load the settings of the editor from file and store them to the editor settings class
	//  TODO

	//////////////////////////////////////////////////////////////////////////
	//	connect to console
#if NET_ACTIVATE
	{
		//	initialize net system
		sx_net_initialize( 0x27272727 );

		g_client = sx_new( Client );
		g_client->m_name.Format( L"editor %s", sx::sys::GetUserName() );
		g_client->Start( 2727, clientCallback );
		g_client->Listen();
		int tryToConnect = 0;
		while ( tryToConnect < 500 )
		{
			g_client->Update( 10, NET_DELAY_TIME, NET_TIMEOUT );

			if ( g_client->m_servers.Count() )
			{
				g_client->Connect( g_client->m_servers[0].address );

				g_client->Update( 10, NET_DELAY_TIME, NET_TIMEOUT );
				break;
			}

			tryToConnect++;
			Sleep(10);
		}
	}
#endif
	sxLog::SetCallback( loggerCallback );

	sx::d3d::Texture::Manager::LoadInThread() = true;
	sx::d3d::Geometry::Manager::LoadInThread() = true;

	//  init main window of editor. this will use settings class of editor
	MainWin::InitWindow();

	//  initialize engines component from the Game class	
	//	TODO

	//////////////////////////////////////////////////////////////////////////
	//	THESE SHOULD MOVE TO GAME

	//  initialize scene manager
	sx::core::Scene::Initialize( sx_new( sx::core::SceneManager_SBVH ) );

	//  initialize renderer
	sx::core::Renderer::Initialize( sx_new( sx::core::Pipeline_Forward) );

	//  create main graphic device
	sx::core::Renderer::SetSize(MainWin::s_Win.GetHandle(), 0, 0, SX_D3D_VSYNC /*| SX_D3D_FULLSCREEN*/);

	//  initialize sounds and music
	sx::snd::Device::Create( MainWin::s_Win.GetHandle(), SX_SND_3D | SX_SND_SOFTWARE );

	//	initialize physics engine
	//	TODO

	//  at first connect keyboard
	sx::io::PInputDeviceBase newDevice = sx_new( sx::io::Keyboard(0) );
	sx::io::Input::Attach( newDevice );

	//  connect mouse
	newDevice = sx_new( sx::io::Mouse(0) );
	sx::io::Input::Attach( newDevice );

	//  finally attach mouse editor to synchronize cursor with windows default cursor
	newDevice = sx_new( sx::io::Mouse_editor(0) );
	sx::io::Input::Attach( newDevice );

	//  initialize IO services
	sx::io::Input::Initialize( MainWin::s_Win.GetHandle() );

	//	initialize script engine
	//	TODO

	//////////////////////////////////////////////////////////////////////////
	sxLog::Log_(L"\r\n-------------------------------------\r\n");
	sxLog::Log(L"Starting the SeganX editor application.\r\n");

	str1024 str = sx::sys::GetAppFolder();
	str.MakePathStyle();
	str << L"Project1";
	Editor::OpenProject(str);

	//  initialize editor
	Editor::Initialize();

	//  resize the editor
	WindowRect rc; MainWin::s_Win.GetRect(rc);
	Editor::Resize(rc.Width, rc.Height);

	//  start the application
	sx::sys::Application::Run(MainLoop, MsgProc);

	//  finalize editor
	Editor::Finalize();

	//  finalize some remain things
	sx::core::Renderer::Finalize();
	sx::core::Scene::Finalize();
	sx::snd::Device::Destroy();

#if NET_ACTIVATE
	sx_delete_and_null( g_client );
	sx_net_finalize();
#endif

	sx_detect_crash();

	return 0;
}