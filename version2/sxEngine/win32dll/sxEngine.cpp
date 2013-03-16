// sxEngine.cpp : Defines the entry point for the DLL application.
//

#include "../sxEngine.h"
#include "../System/win32/Win6.h"

extern Engine*		g_engine = 0;
EngineConfig		s_config;
ApplicationMainLoop s_mainloop = null;

//////////////////////////////////////////////////////////////////////////
//	MAIN ENGINE DLL
//////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain ( HMODULE hModule, dword  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		break;
	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}

WindowRect window_get_rect( handle windowHandle )
{
	WindowRect curRect;

	HWND hWnd = *( (HWND*)windowHandle );

	WINDOWINFO winfo;
	GetWindowInfo( hWnd, &winfo );

	curRect.left = winfo.rcClient.left;
	curRect.top = winfo.rcClient.top;
	curRect.width = winfo.rcWindow.right - winfo.rcClient.left - winfo.cxWindowBorders;
	curRect.height = winfo.rcWindow.bottom - winfo.rcClient.top - winfo.cyWindowBorders;
//	sint iW = (winfo.rcClient.left - winfo.rcWindow.left) + (winfo.rcWindow.right - winfo.rcClient.right);
//	sint iH = (winfo.rcClient.top  - winfo.rcWindow.top)  + (winfo.rcWindow.bottom - winfo.rcClient.bottom);

//	RECT prc;
//	GetWindowRect( hWnd, &prc );
// 	curRect.left	= prc.left + iW / 2;
// 	curRect.top		= prc.top + iH / 2;
// 	curRect.width	= prc.right - prc.left - iW;
// 	curRect.height	= prc.bottom - prc.top - iH;

	return curRect;
}

sint window_events( Window* Sender, const WindowEvent* data )
{
	if ( !data ) return 0;

	switch ( data->msg )
	{
	case WM_SIZE:
		switch ( data->wparam )
		{
		case SIZE_MAXIMIZED:	break;
		case SIZE_RESTORED:		break;
		case SIZE_MINIMIZED:
		default:				return data->msg;
		}

	case WM_EXITSIZEMOVE:
		{
			WindowRect curRect = window_get_rect( data->windowHandle );

			if ( Sender && Sender == g_engine->m_window )
			{
				if ( g_engine->m_input )
					g_engine->m_input->SendSignal( IST_SET_RECT, &curRect );

				if ( g_engine->m_device3D && ( g_engine->m_device3D->m_creationData.flag & SX_D3D_FULLSCREEN ) == 0  )
					g_engine->m_device3D->SetSize( curRect.width, curRect.height, -1 );
			}

			g_engine->m_logger->Log_( L"Window '%s' has been resized [ %d x %d ]", ( Sender ? Sender->m_name.Text() : L"???" ), curRect.width, curRect.height );
			return 0;
		}
		break;
	}

	if ( s_config.window_callback )
		return s_config.window_callback( Sender, data );

	return data->msg;
}

void engine_main_loop( float elpsTime )
{
	g_engine->m_input->Update( elpsTime );

	g_engine->m_network->Update( elpsTime );

	s_mainloop( elpsTime );
}



//////////////////////////////////////////////////////////////////////////
//	ENGINE CLASS
//////////////////////////////////////////////////////////////////////////
SEGAN_ENG_API Engine* engine_get_singleton( EngineConfig* config /*= null */ )
{
	if ( g_engine ) return g_engine;

	Logger_Callback loggercallback = null;
	if ( config )
	{
		s_config = *config;

		if ( config->logger )
		{
			loggercallback = config->logger->callback;
			config->logger->callback = null;
		}
	}
	

	//	create instance of engine
	g_engine = sx_new( Engine );
	ZeroMemory( g_engine, sizeof(Engine) );

	//	create default logger for engine
	g_logger = sx_new( Logger );
	g_logger->Initialize( config->logger );
	g_engine->m_logger = g_logger;
	g_logger->Log_(L"SeganX Engine 0.2 \r\n\r\n");


	//////////////////////////////////////////////////////////////////////////
	//	initialize systems
	//////////////////////////////////////////////////////////////////////////

	//	initialize OS objects and functions
	sx_os_initialize();

	//	initialize thread manager
	sx_thread_initilize();

	//	initialize application
	sx_app_initialize( window_events );

	//	initialize network
	sx_net_initialize( s_config.net_id );

	//	initialize device 3D system
	sx_d3d_initialize( s_config.d3d_flag );



	//////////////////////////////////////////////////////////////////////////
	//	create devices
	//////////////////////////////////////////////////////////////////////////

	//	create network class
	g_engine->m_network = sx_new( Network );

	//	create man window for engine
	if ( !s_config.window_main )
	{
		g_engine->m_window = sx_app_create_window( L"SeganX 0.2", WBT_ORDINARY_RESIZABLE, false );
		g_engine->m_window->SetVisible( true );
	}
	else
	{
		g_engine->m_window = s_config.window_main;
	}

	//	create rendering device
	g_engine->m_device3D = sx_d3d_create_device( s_config.d3d_flag );

	//	create input devices
	g_engine->m_input = sx_new( Input );
	g_engine->m_input->Attach( s_config.input_device[0] );
	g_engine->m_input->Attach( s_config.input_device[1] );
	g_engine->m_input->Attach( s_config.input_device[2] );
	g_engine->m_input->Attach( s_config.input_device[3] );


	//	create GUI device and GUI manager for the engine
	g_engine->m_deviceUI = sx_new( uiDevice );
	g_engine->m_gui = sx_new( GUIManager );

	g_logger->m_callback = loggercallback;
	return g_engine;
}


SEGAN_ENG_API bool engine_initialize( void )
{
	//	initialize network device
	g_engine->m_network->Initialize();

	//	initialize rendering device
	g_engine->m_device3D->Initialize( g_engine->m_window->GetHandle() );
	g_engine->m_device3D->SetSize( -1, -1, s_config.d3d_flag );

	//	initialize input device
	WindowRect rect = window_get_rect( g_engine->m_window->GetHandle() );
	g_engine->m_input->SendSignal( IST_SET_RECT, &rect );

#if 0
	static bool engine_initialized = false;
	if ( engine_initialized )
	{
		g_logger->Log(L"Warning : The engine has been initialized !");
		return false;
	}
	engine_initialized = true;

	if ( !g_engine->m_device3D ) return false;



	//////////////////////////////////////////////////////////////////////////
	//	INITIALIZE OBJECTS
	//////////////////////////////////////////////////////////////////////////
	g_logger->Log_(L"SeganX Engine 0.2 \r\n\r\n");
	
	SystemInfo sysinfo;
	sx_os_get_system_info( sysinfo );

	if ( sysinfo.LidPresent )
		g_logger->Log_( L"------- Running on Laptop PC --------\r\n\r\n" );
	else
		g_logger->Log_( L"------- Running on Desktop PC -------\r\n\r\n" );
	
	g_logger->Log_( L"%s\r\n\r\n", sysinfo.OS.Description );
	g_logger->Log_( L"CPU information:\r\n");
	g_logger->Log_( L"    Brand :     %s\r\n", sysinfo.CPU.BrandName );
	g_logger->Log_( L"    Clock :             %d MHz\r\n", sysinfo.CPU.Clock );
	g_logger->Log_( L"    Cache :             %d KB\r\n", sysinfo.CPU.Cache );
	g_logger->Log_( L"    Number of core(s) : %d\r\n", sysinfo.CPU.ProcessorCount );
	g_logger->Log_( L"    Features :         " );
	if (sysinfo.CPU.mmx)	g_logger->Log_( L" MMX" );
	if (sysinfo.CPU.sse)	g_logger->Log_( L" SSE" );
	if (sysinfo.CPU.sse2)	g_logger->Log_( L" SSE2" );
	if (sysinfo.CPU.now3d)	g_logger->Log_( L" 3DNow" );
	
	g_logger->Log_( L"\r\n\r\n" );
	g_logger->Log_( L"Physical memory information:\r\n" );
	g_logger->Log_( L"    Total Memory :      %d KB\r\n", sysinfo.MemorySize );
	g_logger->Log_( L"    Available Memory :  %d KB\r\n", sx_os_get_available_memory() );
	
	g_logger->Log_( L"\r\n\r\n" );
#endif

	return true;
}

void engine_finalize( void )
{
	if ( !g_logger ) return;
	g_logger->m_callback = null;
	g_logger->Log( L"Shutting down SeganX ...\r\n" );

	//////////////////////////////////////////////////////////////////////////
	//	finalize devices
	//////////////////////////////////////////////////////////////////////////
	
	g_engine->m_network->Finalize();
	g_engine->m_gui->Clear();


	//////////////////////////////////////////////////////////////////////////
	//	destroy devices
	//////////////////////////////////////////////////////////////////////////

	//	destroy GUI manager
	sx_delete_and_null( g_engine->m_gui );

	//	destroy GUI device
	sx_delete_and_null( g_engine->m_deviceUI );

	//	destroy input system
	sx_delete_and_null( g_engine->m_input );

	// destroy network device
	sx_delete_and_null( g_engine->m_network );

	//	destroy 3D device
	sx_d3d_destroy_device( g_engine->m_device3D );

	//	destroy window
	if ( ! s_config.window_main )
		sx_app_destroy_window( g_engine->m_window );
	g_engine->m_window = null;


	//////////////////////////////////////////////////////////////////////////
	//	finalize systems
	//////////////////////////////////////////////////////////////////////////

	//	finalize 3D system
	sx_d3d_finalize();

	//	finalize network
	sx_net_finalize();

	//	finalize application
	sx_app_finalize();

	//	finalize thread manager
	sx_thread_finalize();
	
	//	finalize OS objects
	sx_os_finalize();

	//  delete logger
	g_logger->Log_( L"\r\nSeganX terminated." );
	sx_delete_and_null( g_logger );

	//	delete engine instance
	sx_delete_and_null( g_engine );
}

SEGAN_ENG_API void engine_start( ApplicationMainLoop mainloop )
{
	s_mainloop = mainloop;
	sx_app_run( engine_main_loop );
}
