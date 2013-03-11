// sxEngine.cpp : Defines the entry point for the DLL application.
//

#include "../sxEngine.h"
#include "../System/win32/Win6.h"

extern Engine*	g_engine = 0;
EngineConfig	s_config;

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


//////////////////////////////////////////////////////////////////////////
//	ENGINE CLASS
//////////////////////////////////////////////////////////////////////////
SEGAN_ENG_API Engine* engine_get_singleton( EngineConfig* config /*= null */ )
{
	if ( g_engine ) return g_engine;

	if ( config )
		s_config = *config;

	//	create instance of engine
	g_engine = sx_new( Engine );
	ZeroMemory( g_engine, sizeof(Engine) );

	//	create default logger for engine
	g_logger = sx_new( Logger );
	g_logger->Initialize( config->logger );
	g_engine->m_logger = g_logger;
	g_logger->Log_(L"SeganX Engine 0.2 \r\n\r\n");


	//	initialize OS objects and functions
	sx_os_initialize();

	//	initialize thread manager
	sx_thread_initilize();

	//	initialize application
	sx_app_initialize( s_config.window_callback );

	//	initialize network
	sx_net_initialize( s_config.net_id );

	//	initialize device 3D system
	sx_d3d_initialize( s_config.d3d_flag );

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

	return g_engine;
}


SEGAN_ENG_API bool engine_initialize( void )
{
	//	initialize rendering device
	g_engine->m_device3D->Initialize( g_engine->m_window->GetHandle() );
	g_engine->m_device3D->SetSize( -1, -1, s_config.d3d_flag );


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

	//////////////////////////////////////////////////////////////////////////
	//	FINALIZE OBJECTS
	//////////////////////////////////////////////////////////////////////////

	g_logger->Log( L"Shutting down SeganX ...\r\n" );

	//g_engine->m_network->Finalize();
	//sx_d3d_destroy_device( g_engine->m_device3D );

	//	finalize device 3D system
	//sx_d3d_finalize();

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
	sx_app_run( mainloop );
}
