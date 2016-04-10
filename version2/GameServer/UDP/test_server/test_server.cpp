#include "../Net.h"
#include "../Socket.h"
#include "../Connection.h"
#include "../Server.h"

#include <windows.h>

#define NET_ID 32
#define BUF_SZ 1024	


int wmain(int argc, wchar* argv[])
{
	sx_callstack();

	sx_net_initialize();


	NetConfig config;
	config.recv_port = 31100;
	config.send_port = 32100;

	Server* server = sx_new Server;
	server->Initialize( /*&config*/ );

	char buffer[BUF_SZ] = {0};
	double lastTime = sx_net_get_timer();
	while (true)
	{
		double tickCount = sx_net_get_timer();
		double elpsTime = (tickCount - lastTime);
		lastTime = tickCount;
		if ( elpsTime < 0 ) continue;

		server->Update(elpsTime);

		for ( int i = 0; i < server->m_connections.m_count; ++i )
		{
			while ( uint len = server->m_connections[i]->Received( buffer, BUF_SZ ) > 0 )
			{
				sx_print( L"Info: Connection %d received %s\n", i, buffer );
				sx_mem_set( buffer, 0, len+2 );
			}
		}

		Sleep(1);
	}


	sx_net_finalize();

	getchar();
	sx_detect_crash();
	return 0;
}