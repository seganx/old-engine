// sxEditor.cpp : Defines the entry point for the application.
//
#include "sxEditor.h"

//#include <Windows.h>
#include <stdio.h>


#define NET_DELAY_TIME	60
#define NET_TIMEOUT		10000

//////////////////////////////////////////////////////////////////////////
//  SOME GLOBAL VARIABLES

#if 1
d3dVertexBuffer* vbo_pos = null;
d3dVertexBuffer* vbo_col = null;
d3dVertexBuffer* vbo_tx0 = null;
d3dTexture*	tex_000	=	null;
d3dTexture*	tex_001	=	null;
#endif

sint window_event_call( Window* Sender, const WindowEvent* data )
{
	return data->msg;
}

void app_main_loop( float elpsTime )
{
	sx_mem_enable_debug( true, 2 );

#if 1
	if ( g_engine->m_device3D && g_engine->m_device3D->BeginScene() )
	{
		matrix mat = sx_perspective_fov( PI / 3.0f, (float)g_engine->m_device3D->m_viewport.height / (float)g_engine->m_device3D->m_viewport.width, 0.5f, 1000.0f );
		g_engine->m_device3D->SetMatrix( MM_PROJECTION, mat );

		static float timer = 0;
		timer =  (float)( 0.0003f * sx_os_get_timer() );
		//timer = 2.0f;
		

		static float cam_r = 20.0f, cam_p = 0.0f, cam_t = 0.5f;
		static float3 cam_at;
		Camera camera;
		camera.m_at = cam_at;
		camera.SetSpherical( cam_r, cam_p, cam_t );
		g_engine->m_device3D->SetMatrix( MM_VIEW, camera.GetViewMatrix() );
		if ( sx_key_hold( IK_MOUSE_LEFT, 0 ) || sx_key_down( IK_MOUSE_LEFT, 0 ) )
		{
			cam_p += g_engine->m_input->GetValues()->rl_x * 0.005f;
			cam_t -= g_engine->m_input->GetValues()->rl_y * 0.005f;
		}
		if ( sx_key_hold( IK_MOUSE_RIGHT, 0 ) || sx_key_down( IK_MOUSE_RIGHT, 0 ) )
		{
			cam_r += g_engine->m_input->GetValues()->rl_y * 0.05f;
		}
		if ( sx_key_hold( IK_MOUSE_MIDDLE, 0 ) || sx_key_down( IK_MOUSE_MIDDLE, 0 ) )
		{
			sx_inverse( mat, g_engine->m_device3D->GetMatrix(MM_VIEW) );
			float3 tv, mv( -sx_mouse_rlx(0), sx_mouse_rly(0), 0.0f );
			sx_transform_normal( tv, mv, mat );
			cam_at += tv * ( cam_r * 0.003f );
		}

		g_engine->m_device3D->SetTexture( null );

		g_engine->m_device3D->ClearScreen( 0x000000ff );

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
		float d[3] = { 1.0f, sx_sin(timer*7.0f), sx_sin(timer*3.0f) }, u[3] = { 0.0f, 1.0f, 0.0f };
		sx_set_direction( mat, d, u );

		sx_debug_draw_grid( 10, 0xaaaaaaaa );

		Ray ray = sx_ray( sx_mouse_absx(0), sx_mouse_absy(0), 
			(float)g_engine->m_device3D->m_viewport.width,
			(float)g_engine->m_device3D->m_viewport.height,
			g_engine->m_device3D->GetMatrix(MM_VIEW),
			g_engine->m_device3D->GetMatrix(MM_PROJECTION));

		AABox box1( -1, 0, -1, 1, 2, 1 );
		AABox box2( 2, 0, 2, 4, 2, 4 );
		OBBox box3 = sx_transform( box2, mat );

		bool intersect = sx_intersect( ray, box1, 10.0f );
		sx_debug_draw_box( box1, intersect ? 0xffff0000 : 0xffffff00 );

		float3 ipoint, inorm;
		intersect = sx_intersect( ray, box3, &ipoint, &inorm );
		sx_debug_draw_box( box3, intersect ? 0xffff0000 : 0xffffff00 );

		if ( intersect )
		{
			sx_debug_draw_sphere( Sphere( ipoint, 0.1f ), 0xffaaaaff, 4, 6 );
			sx_debug_draw_line( ipoint, ipoint + inorm, 0xffaaaaff );
		}

		sx_debug_draw_box( sx_cover( box1, box3 ), 0xffffffff );


#if 0
		sx_debug_draw_circle( float3( 5, 0, 4 ), 3, 0xffffffff );

		Sphere sph1( -13, 0, -3, 1 );
		Sphere sph2( -15 + 6.0f * sx_sin_fast(timer*6.0f), 0, 9.0f * sx_sin_fast(timer*5.0f), 2 );
		sx_debug_draw_sphere( sph1, 0xff00ffff, 7, 9 );
		sx_debug_draw_sphere( sph2, 0xff00ffff );
		sx_debug_draw_sphere( sx_cover( sph2, sph1 ), 0xffffffff );


		Element e1, e2, eb;
		e1.CreateVertices( 6 );
		e1.m_pos[0].Set(  50,  50, 0.0f );
		e1.m_pos[1].Set( -50,  50, 0.0f );
		e1.m_pos[2].Set( -50, -50, 0.0f );
		e1.m_pos[3].Set( -50, -50, 0.0f );
		e1.m_pos[4].Set(  50, -50, 0.0f );
		e1.m_pos[5].Set(  50,  50, 0.0f );

		e2.CreateVertices( 3 );
		e2.m_pos[0].Set(  160, 150, 0.0f );
		e2.m_pos[1].Set(  60,  150, 0.0f );
		e2.m_pos[2].Set(  60,  50, 0.0f );

		sx_element_begin_batch();
		sx_element_add_batch( &e1 );
		sx_element_add_batch( &e2 );
		sx_element_end_batch( &eb );
		sx_debug_draw_gui_element( &eb );
#endif

		sx_debug_draw_compass();

		g_engine->m_device3D->EndScene();

		g_engine->m_device3D->Present();

		str128 str;
		str.Format( L"fps : %d - ft : %.2f - mouse : %d , %d - intersect : %d ", 
			g_engine->m_device3D->m_debugInfo.fps, g_engine->m_device3D->m_debugInfo.frameTime,
			int(sx_mouse_absx(0)), int(sx_mouse_absy(0)), intersect );
		g_engine->m_window->SetTitle( str );
	}
#endif

	sx_mem_enable_debug( true, -1 );

	sx_os_sleep(1);
}

#if 1

void logger_callback( const wchar* message )
{
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

	g_engine->m_network->Update(0);
	g_engine->m_network->Send( msg, i, false );
	g_engine->m_network->Update(0);
}

void mem_callback( const wchar* file, const uint line, const uint size, const uint tag, const bool corrupted )
{
	int len = 0;
	char msg[512] = {0};
	if ( corrupted )
		len = sprintf_s( msg, 512, "ERROR : memory corruption detected on %S - line %d - size %d - tag %d", file, line, size, tag );
	else
		len = sprintf_s( msg, 512, "WARNING : memory leak detected on %S - line %d - size %d - tag %d", file, line, size, tag );
	g_engine->m_network->Send( msg, len+1 );
	g_engine->m_network->Update(0);
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
	loggerconfig.callback = &logger_callback;
	loggerconfig.mode = LM_FILE;

	Mouse_editor ioMouse(0);

	EngineConfig config;
	config.net_id = 0x2727;
	config.logger = &loggerconfig;
	config.window_callback = &window_event_call;
	config.d3d_flag = SX_D3D_CREATE_GL | SX_D3D_VSYNC;// | SX_D3D_FULLSCREEN;
	config.input_device[0] = &ioMouse;

	sx_engine_get_singleton( &config );

	//////////////////////////////////////////////////////////////////////////
	//	connect to console
#if 1
	{
		Client* client = g_engine->m_network->m_client;
		client->m_name.Format( L"editor %s", sx_os_get_user_name() );
		client->Start( 2727, null );
		client->Listen();
		int tryToConnect = 0;
		while ( tryToConnect < 500 )
		{
			g_engine->m_network->Update( 10 );

			if ( client->m_servers.Count() )
			{
				client->Connect( client->m_servers[0].address );

				g_engine->m_network->Update( 10 );
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

		g_engine->m_network->Send( "window created !", 17 );
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

		sx_engine_start( &app_main_loop );

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
	sx_engine_start( &app_main_loop );
#endif

	sx_mem_report_debug( &mem_callback, 0 );

	//////////////////////////////////////////////////////////////////////////
 	for ( int i=0; i<60; i++ )
 	{
 		g_engine->m_network->Update( 16 );
 		sx_os_sleep(1);
 	}

	sx_engine_finalize();

	sx_detect_crash();

	return 0;
}