#include <windows.h>

#include "../Server.h"
#include "DatabaseServer.h"
#include "Thread_Manager.h"
#include "sxConsole.h"

#define NET_ID 32
#define BUF_SZ 1024	

extern Library* g_lib = null;

int wmain(int argc, wchar* argv[])
{
	g_lib = Library::GetSingelton();

	sx_net_initialize();

	sx_callstack();

	DatabaseServer dbServer;
	dbServer.LoadConfig(argc > 1 ? argv[1] : L"config.txt");
	dbServer.Initialize();

	sx_create_window(L"Stats");

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

		g_lib->m_timer->Update();
		dbServer.Update();

		{
			wchar stats[1024] = {0};
			swprintf_s(stats, 1024, 
				L"Total Connections: %d\r\n"
				L"Current Connections: %d\r\n"
				L"Max Concurrent Connections: %d\r\n"
				L"Current Threads: %d\r\n"
				L"Cycle Per Second: %d\r\n"
				L"Max Cycle Per Second: %d\r\n"
				L"Min Cycle Per Second: %d\r\n"
				L"Socket Queued Packets: %d\r\n"
				L"Total Invalid Packets: %d\r\n"
				L"Invalid Packets Per Second: %d\r\n"
				L"Total Retry Packets Send: %d\r\n", 
				dbServer.m_server->m_stats.total_connections,
				dbServer.m_server->m_stats.curr_connections,
				dbServer.m_server->m_stats.max_connections,
				dbServer.m_threadMan->m_threads.m_count,
				dbServer.m_server->m_stats.cycle_per_sec,
				dbServer.m_server->m_stats.max_cycle_per_sec,
				dbServer.m_server->m_stats.min_cycle_per_sec,
				dbServer.m_server->m_stats.socket_queued_packs,
				dbServer.m_server->m_stats.total_invalid_packs,
				dbServer.m_server->m_stats.invalid_packs_per_sec,
				dbServer.m_server->m_stats.retry_count
				);
			sx_window_set_text( stats );
		}

		Sleep(1);
	}

	dbServer.Finalize();

	sx_net_finalize();

	return 0;
}