// sxEditor.cpp : Defines the entry point for the application.
//
#include "sxEditor.h"

Window*			window = null;
d3dRenderer*	render = null;

int windowcallback( class Window* sender, const WindowEvent* data )
{
	switch ( data->msg )
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
		break;

	case WM_SIZE:
		{
			WINDOWINFO winfo;
			GetWindowInfo( data->windowHandle, &winfo );
			int width = winfo.rcClient.right - winfo.rcClient.left;
			int height = winfo.rcClient.bottom - winfo.rcClient.top;
			if ( render )
				render->set_size( width, height, SX_D3D_VSYNC, data->windowHandle );
		}
		break;
	}
	return data->msg;
}


void mainloop( float elpstime )
{
	render->update( elpstime );

	render->begin_draw( 0xff332211 );
	render->draw_grid( 7, 0xff888888 );
	render->draw_sphere( Sphere( -3, 1, 2, 1 ), SX_D3D_WIREFRAME, 0xff6688aa );
	render->draw_circle( float3( 1,1,1 ), 1, SX_D3D_BILLBOARD, 0xff33cc33 );
	render->render( elpstime, 0 );
	render->draw_compass();
	render->end_draw();

	sx_os_sleep( 25 );
}


//////////////////////////////////////////////////////////////////////////
//	START POINT OF PROGRAM
//////////////////////////////////////////////////////////////////////////
sint APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, sint nCmdShow)
{

	sx_detect_crash();

	window = sx_create_window( null, &windowcallback, true, false );
	//window->set_border( WBT_NONE );
	window->set_visible( true );


	render = sx_create_renderer( 0 );
	render->initialize( 0 );
	render->set_size( window->m_rect.width, window->m_rect.height, SX_D3D_VSYNC, window->get_handle() );



	//////////////////////////////////////////////////////////////////////////
	//	going to main loop in window
	static float blendedElapesTime = 0;
	float initTime = (float)sx_os_get_timer();
	float elpsTime = 0;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, null, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			msg.hwnd = null;	
			msg.message = 0;
		}

		// calculate elapsed time
		float curTime = (float)sx_os_get_timer();
		elpsTime = curTime - initTime;
		initTime = curTime;

		//  avoid update when system timer has been reseted after about 47 days
		if ( 0.0f < elpsTime && elpsTime < 2000.0f )
		{
			// call the main loop function
			blendedElapesTime += (elpsTime - blendedElapesTime) * 0.1f;
			mainloop( blendedElapesTime );
		}
	}

	return 0;
}