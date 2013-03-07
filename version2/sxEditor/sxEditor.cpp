// sxEditor.cpp : Defines the entry point for the application.
//
#include "sxEditor.h"

//#include <Windows.h>
#include <stdio.h>


#define NET_DELAY_TIME	60
#define NET_TIMEOUT		10000

//////////////////////////////////////////////////////////////////////////
//  SOME GLOBAL VARIABLES
Client* client = null;

#if 1
d3dVertexBuffer* vbo_pos = null;
d3dVertexBuffer* vbo_col = null;
d3dVertexBuffer* vbo_tx0 = null;
d3dTexture*	tex_000	=	null;
d3dTexture*	tex_001	=	null;
#endif

int WindowEventCall( Window* Sender, const WindowEvent* data )
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
			WindowRect curRect;
			
			HWND hWnd = *( (HWND*)data->windowHandle );

			WINDOWINFO winfo;
			GetWindowInfo( hWnd, &winfo );
			sint iW = (winfo.rcClient.left - winfo.rcWindow.left) + (winfo.rcWindow.right - winfo.rcClient.right);
			sint iH = (winfo.rcClient.top  - winfo.rcWindow.top)  + (winfo.rcWindow.bottom - winfo.rcClient.bottom);
		
			RECT prc;
			GetWindowRect( hWnd, &prc );
			curRect.left	= prc.left;
			curRect.top		= prc.top;
			curRect.width	= prc.right - prc.left - iW;
			curRect.height	= prc.bottom - prc.top - iH;

#if 1
			if ( Sender && Sender == g_engine->m_window )
			{
				if ( g_engine->m_device3D && ( g_engine->m_device3D->m_creationData.flag & SX_D3D_FULLSCREEN ) == 0  )
					g_engine->m_device3D->SetSize( curRect.width, curRect.height, -1 );
			}
#else
			static int counter = 0;
			g_engine->m_logger->Log_( L"%d > Window has been resized [ %d x %d ]", counter++, curRect.width, curRect.height );
#endif
			return 0;
		}
		break;
	}

	return data->msg;
	
}

void AppMainLoop( float elpsTime )
{
	sx_mem_enable_debug( true, 2 );

	client->Update( elpsTime, NET_DELAY_TIME, NET_TIMEOUT );

#if 1
	if ( g_engine->m_device3D && g_engine->m_device3D->BeginScene() )
	{
		matrix mat = sx_perspective_fov( PI / 3.0f, (float)g_engine->m_device3D->m_viewport.height / (float)g_engine->m_device3D->m_viewport.width, 0.5f, 1000.0f );
		g_engine->m_device3D->SetMatrix( MM_PROJECTION, mat );

		static float timer = 0;
		timer =  (float)( 0.0003f * sx_os_get_timer() );
		//float eye[3] = { 5.0f * sx_sin(timer), 5.0f, 10.0f * sx_cos(timer)	};
		float eye[3] = { 2.0f , 5.0f, 5.0f };
		float at[3] = { 0.0f, 0.0f, 0.0f };
		float up[3] = { 0.0f, 1.0f, 0.0f };
		mat = sx_lookat( eye, at, up );
		g_engine->m_device3D->SetMatrix( MM_VIEW, mat );

		g_engine->m_device3D->SetTexture( null );

		g_engine->m_device3D->ClearScreen( 0xffbbbbff );

#if 0
		g_engine->m_device3D->SetTexture( tex_000 );

		g_engine->m_device3D->SetVertexBuffer( vbo_pos, 0 );
 		g_engine->m_device3D->SetVertexBuffer( vbo_tx0, 2 );
 		g_engine->m_device3D->SetVertexBuffer( vbo_col, 5 );

		g_engine->m_device3D->DrawPrimitive( PT_TRIANGLE_LIST, 0, 6 );

		if ( vbo_pos )
		{
			static float s_time = 0;
			s_time += elpsTime;
			float* pos = (float*)vbo_pos->Lock();
			pos[0] = sx_sin_fast( s_time * 0.001f );
			pos[1] = sx_cos_fast( s_time * 0.001f ) - 1.0f;
			pos[2] = sx_sin_fast( s_time * 0.005f );
			vbo_pos->Unlock();
		}
#endif

		g_engine->m_device3D->SetTexture( tex_001 );

		float d[3] = { 1.0f, 0.0f, 1.0f }, u[3] = { 0.0f, 1.0f, 0.0f };
		sx_set_direction( mat, d, u );
//		mat.SetTranslation( 0, 0, 1 );
//		mat.Identity();
//		mat.SetRotationPitchYawRoll( 0, 0, (float)sx_os_get_timer() * 0.005f );
//		matrix mview = g_engine->m_device3D->GetMatrix( MM_VIEW );
//		mat.Inverse( mview );
		g_engine->m_device3D->SetMatrix( MM_WORLD, mat );

		g_engine->m_device3D->EndScene();

		g_engine->m_device3D->Present();

		str128 str;
		str.Format( L"fps : %d - ft : %.2f", g_engine->m_device3D->m_debugInfo.fps, g_engine->m_device3D->m_debugInfo.frameTime );
		g_engine->m_window->SetTitle( str );
	}
#endif

	sx_mem_enable_debug( true, -1 );

	sx_os_sleep(1);
}

#if 1

void clientCallback( Client* client, const byte* buffer, const uint size )
{

}

void loggerCallback( const wchar* message )
{
	if ( !client ) return;

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

	client->Update( 0, NET_DELAY_TIME, NET_TIMEOUT );
	client->Send( msg, i, false );
	client->Update( 0, NET_DELAY_TIME, NET_TIMEOUT );
}

void mem_CallBack( const wchar* file, const uint line, const uint size, const uint tag, const bool corrupted )
{
	if ( !client ) return;

	int len = 0;
	char msg[512] = {0};
	if ( corrupted )
		len = sprintf_s( msg, 512, "ERROR : memory corruption detected on %S - line %d - size %d - tag %d", file, line, size, tag );
	else
		len = sprintf_s( msg, 512, "WARNING : memory leak detected on %S - line %d - size %d - tag %d", file, line, size, tag );
	client->Send( msg, len+1 );
	client->Update( 0, NET_DELAY_TIME, NET_TIMEOUT );
}

#endif


//////////////////////////////////////////////////////////////////////////
//	START POINT OF PROGRAM
//////////////////////////////////////////////////////////////////////////
sint APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, sint nCmdShow)
{
	LoggerConfig loggerconfig;
	loggerconfig.name = L"sx_editor";
	loggerconfig.fileName = L"sx_editor_log.txt";
	loggerconfig.callback = &loggerCallback;
	loggerconfig.mode = LM_FILE;

	EngineConfig config;
	config.net_id = 0x2727;
	config.logger = &loggerconfig;
	config.window_callback = &WindowEventCall;
	config.d3d_flag = SX_D3D_CREATE_GL | SX_D3D_VSYNC;// | SX_D3D_FULLSCREEN;

	sx_engine_get_singleton( &config );

	//////////////////////////////////////////////////////////////////////////
	//	connect to console
#if 1
	{
		client = sx_new( Client );
		client->m_name.Format( L"editor %s", sx_os_get_user_name() );
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
			sx_os_sleep(2);
		}
	}
#endif

	sx_engine_initialize();

#if 1
	{
		Window* winChild = sx_app_create_window( L"test", WBT_WINTOOL_RESIZABLE, true );
		winChild->SetCursor( WCT_HAND );
		winChild->SetRect( 50, 500, 200, 150 );
		winChild->SetVisible( true );

		client->Send( "window created !", 17 );
	}
#endif


#if 1
	{
		sx_mem_enable_debug( true, 1 );

		g_engine->m_device3D->CreateVertexBuffer( vbo_pos );
		if ( vbo_pos )
		{
			d3dVertexBufferDesc desc;
			desc.flag = SX_D3D_RESOURCE_DYNAMIC;
			desc.size = 6 * 3 * sizeof(float);
			float PositionData[] =
			{
				-2.0f,	-2.0f,	0.0f,
				 2.0f,	-2.0f,	0.0f,
				 2.0f,	 2.0f,	0.0f,
				 2.0f,	 2.0f,	0.0f,
				-2.0f,	 2.0f,	0.0f,
				-2.0f,	-2.0f,	0.0f,
			};
			vbo_pos->SetDesc( desc, PositionData );
		}

		g_engine->m_device3D->CreateVertexBuffer( vbo_tx0 );
		if ( vbo_tx0 )
		{
			d3dVertexBufferDesc desc;
			desc.flag = 0;
			desc.size = 6 * 2 * sizeof(float);
			float uvData[] =
			{
				0.0f,	0.0f,
				1.0f,	0.0f,
				1.0f,	1.0f,
				1.0f,	1.0f,
				0.0f,	1.0f,
				0.0f,	0.0f
			};
			vbo_tx0->SetDesc( desc, uvData );
		}

		g_engine->m_device3D->CreateVertexBuffer( vbo_col );
		if ( vbo_col )
		{
			d3dVertexBufferDesc desc;
			desc.flag = 0;
			desc.size = 6 * 4 * 2;
			byte ColorData[] =
			{
				//	color 0									//	color 1
				255,	100,	100,	255,				255,	255,	255,	255,
				100,	255,	100,	255,				255,	255,	255,	255,
				100,	100,	255,	255,				255,	255,	255,	255,
				100,	100,	255,	255,				255,	255,	255,	255,
				100,	255,	100,	255,				255,	255,	255,	255,
				255,	100,	100,	255,				255,	255,	255,	255
			};
			vbo_col->SetDesc( desc, ColorData );
		}

		g_engine->m_device3D->CreateTexture( tex_000 );
		if ( tex_000 )
		{
			d3dTextureDesc desc = *tex_000->GetDesc();
			desc.width = 256;
			desc.height = 256;
			desc.mipmaps = 1;
			tex_000->SetDesc( desc );

			FileStream file;
			if ( file.Open( L"D:/tex_000.txt", FM_OPEN_READ ) )
			{
				for ( uint l=0; l<=tex_000->GetDesc()->mipmaps; l++ )
				{
					const uint size = tex_000->GetDataSize(l);
					byte* data = (byte*)sx_mem_alloc( size );
					file.Read( data, size );
					tex_000->SetImage( data, l );
					sx_mem_free( data );
				}
				file.Close();
			}
			else
			{
				for ( uint l=0; l<=tex_000->GetDesc()->mipmaps; l++ )
				{
					const uint size = tex_000->GetDataSize(l);
					{
						byte* data = (byte*)sx_mem_alloc( size );
						for ( uint i=0; i<size; i++ )
							data[i] = sx_random_i( i % 255 );
						tex_000->SetImage( data, l );
						sx_mem_free( data );
					}
				}

			}
		}

		g_engine->m_device3D->CreateTexture( tex_001 );
		if ( tex_001 )
		{
			d3dTextureDesc desc = *tex_001->GetDesc();
			desc.format = FMT_DXT1;
			desc.width = 128;
			desc.height = 128;
			desc.filter = FILTER_LINEAR_ANISOTROPIC;
			tex_001->SetDesc( desc );

			const uint size = 128*128*4;
			byte data[size];
			for ( uint i=0; i<size; i++ )
				data[i] = i % 255;//int( 255 * sin( cos( i / 10.0f ) * 10.0f ) );

			tex_001->SetImage( data, 0 );
		}

		sx_engine_start( &AppMainLoop );

		if ( tex_000 )
		{
			FileStream file;
			file.Open( L"D:/tex_000.txt", FM_CREATE );

			for ( uint l=0; l<=tex_000->GetDesc()->mipmaps; l++ )
			{
				uint size = tex_000->GetDataSize(l);
				byte* data = (byte*)sx_mem_alloc( size );
				ZeroMemory( data, size );

				tex_000->GetImage( data, l );
				file.Write( data, size );

				sx_mem_free( data );
			}

			file.Close();
		}

		g_engine->m_device3D->DestroyTexture( tex_000 );
		g_engine->m_device3D->DestroyTexture( tex_001 );
		g_engine->m_device3D->DestroyVertexBuffer( vbo_col );
		g_engine->m_device3D->DestroyVertexBuffer( vbo_tx0 );
		g_engine->m_device3D->DestroyVertexBuffer( vbo_pos );
		
		sx_d3d_destroy_device( g_engine->m_device3D );
	}
#else
	sx_engine_start( &AppMainLoop );
#endif

	sx_mem_report_debug( &mem_CallBack, 0 );

#if 1
	//////////////////////////////////////////////////////////////////////////
 	for ( int i=0; i<60; i++ )
 	{
 		client->Update( 16, 60, 5000 );
 		sx_os_sleep(1);
 	}
 	sx_delete_and_null( client );
#endif

	sx_engine_finalize();

	sx_detect_crash();

	return 0;
}