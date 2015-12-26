// sxEditor.cpp : Defines the entry point for the application.
//
#include "sxEditor.h"

#define				CNODES  50
#define				SSIZE	3

Window*				window = null;
d3dRenderer*		render = null;
uiManager*			gui = null;
Partition<uint>*	scene = null;

void*				node[CNODES] = { 0 };

int windowcallback(class Window* sender, const WindowEvent* data)
{
	switch (data->msg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
			break;

		case WM_SIZE:
		{
			WINDOWINFO winfo;
			GetWindowInfo(data->windowHandle, &winfo);
			int width = winfo.rcClient.right - winfo.rcClient.left;
			int height = winfo.rcClient.bottom - winfo.rcClient.top;
			if (render)
				render->set_size(width, height, SX_D3D_VSYNC, data->windowHandle);
		}
			break;
	}
	return data->msg;
}


void mainloop(double elpstime)
{
	render->update(elpstime);

#if 0
	gui->update(elpstime, (float)window->m_rect.width, (float)window->m_rect.height);

	gui->draw( 0 );

	d3dContext elmnt;
	elmnt.vcount = gui->m_drawable->m_vcount;
	elmnt.pos = gui->m_drawable->m_pos;
	render->m_elements.clear();
	render->m_elements.push_back( &elmnt );
#endif

	render->begin_draw(0xff332211);

	//render->draw_grid( 7, 0xff888888 );
	//render->draw_sphere( Sphere( -3, 1, 2, 1 ), SX_D3D_WIREFRAME, 0xff6688aa );
	//render->draw_circle( float3( 1,1,1 ), 1, SX_D3D_BILLBOARD, 0xff33cc33 );

	for (int i = 0; i < CNODES; i++)
	{
		float delta = 1 + i * 0.01f;
		scene->update_node(node[i],
			sx_sin(g_timer->m_time * 0.1f * delta + i) * SSIZE + SSIZE,
			sx_sin(g_timer->m_time * 0.2f * delta + i) * 3 + 3,
			sx_sin(g_timer->m_time * 0.3f * delta + i) * SSIZE + SSIZE);

	}

	for (uint i = 0; i < scene->m_boxCount; i++)
	{
		Partition<uint>::Box* box = &scene->m_boxes[i];

		//	draw the box
		float margin = 0.01f;
		float scl = scene->m_scale - margin;
		AABox aabox(
			box->pos[0] + margin, box->pos[1] + margin, box->pos[2] + margin,
			box->pos[0] + scl, box->pos[1] + scl, box->pos[2] + scl);
		render->draw_box(aabox, 0xff003333 + box->count * 0x00660000);

		//	draw objects in the box
		for (uint j = 0; j < box->count; j++)
		{
			Partition<uint>::Node* node = box->nodes[j];
			if (node)
				render->draw_sphere(Sphere(node->pos[0], node->pos[1], node->pos[2], 0.1f), 0, node->data);
		}
	}

	AABox aabox(
		0.1f, 3.5f, 0.1f, 
		2.5f, 5.9f, 1.5f
		);
	render->draw_box(aabox, 0xffffff00);
	uint objCount = scene->get_count_in_aabox(aabox.x1, aabox.y1, aabox.z1, aabox.x2, aabox.y2, aabox.z2);
	
	if (0)
	{
		wchar tmp[64] = {0};
		uint64 t = g_timer->GetCurrTime();
		_ui64tow_s( t, tmp, 64, 10 );
		window->set_title(tmp);
	}
	else window->set_title(sx_uint_to_str(objCount));



	render->render(elpstime, 0);

	render->draw_compass();
	render->end_draw();


	sx_os_sleep(25);
}


//////////////////////////////////////////////////////////////////////////
//	START POINT OF PROGRAM
//////////////////////////////////////////////////////////////////////////
sint APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, sint nCmdShow)
{

	sx_detect_crash();

	window = sx_create_window(null, &windowcallback, false, false);
	//window->set_border( WBT_NONE );
	window->set_visible(true);


	render = sx_create_renderer(0);
	render->initialize(0);
	render->set_size(window->m_rect.width, window->m_rect.height, SX_D3D_VSYNC, window->get_handle());

	gui = sx_new(uiManager);
	uiPanel* panel = sx_new(uiPanel);
	panel->set_size(2, 1);
	gui->add(panel);

	scene = sx_new(Partition<uint>);
	scene->set_size(2, SSIZE, 3, SSIZE);
	for (int i = 0; i < CNODES; i++)
	{
		node[i] = scene->insert(0xff000000 + sx_random_i_limit(0x00555555, 0x00ffffff), sx_random_f(SSIZE), 0.5f, sx_random_f(SSIZE));
	}

	//////////////////////////////////////////////////////////////////////////
	//	going to main loop in window

	g_timer = sx_new Timer;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, null, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			msg.hwnd = null;
			msg.message = 0;
		}

		g_timer->Update();
		mainloop(g_timer->m_elpsTime);
	}

	sx_delete_and_null(scene);

	detect_crash();
	return 0;
}