// sxEditor.cpp : Defines the entry point for the application.
//
#include "sxEditor.h"



int cb_window( class Window* sender, const WindowEvent* data )
{
	switch ( data->msg )
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
		break;
	}
	return data->msg;
}

//////////////////////////////////////////////////////////////////////////
//	START POINT OF PROGRAM
//////////////////////////////////////////////////////////////////////////
sint APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, sint nCmdShow)
{

	sx_detect_crash();

	Window* win = sx_create_window( null, &cb_window, true, false );
	//win->set_border( WBT_NONE );
	win->set_visible( true );


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
			//if ( mainLoop ) mainLoop( blendedElapesTime );
		}
	}

	return 0;
}