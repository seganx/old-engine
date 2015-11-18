#include <windows.h>

#include "../Net.h"
#include "../Socket.h"
#include "../Connection.h"
#include "../Server.h"
#include "database.h"
#include "Thread_Manager.h"


#define NET_ID 32
#define BUF_SZ 1024	

Map<uint, DatabaseThread*> s_threads;

bool OnConnectionAdd( Connection* con )
{
	return true;
}

bool OnConnectionRemove(Connection* con)
{
	return true;
}

int wmain(int argc, wchar* argv[])
{
	sx_callstack();
	sx_net_initialize();

	NetConfig config;
	config.recv_port = 31000;
	config.send_port = 32000;
	config.retry_timeout = 30;
	config.packs_per_sec = 20;

	Server* server = sx_new Server;
	server->Initialize( &config, &OnConnectionAdd, &OnConnectionRemove );
	
	DatabaseConfig dbconfig;
	ZeroMemory(&dbconfig, sizeof(dbconfig));
	dbconfig.host = "127.0.0.1";
	dbconfig.name = "hashtkhan";
	dbconfig.user = "sajad_test";
	dbconfig.pass = "sajad_test_123456";
	dbconfig.port = 3306;

	ThreadManager* threadMan = sx_new ThreadManager(15);

	double lastTime = sx_net_get_timer();
	while (true)
	{
		double tickCount = sx_net_get_timer();
		double elpsTime = (tickCount - lastTime);
		lastTime = tickCount;
		if ( elpsTime < 0 ) continue;

		server->Update(elpsTime);

		for ( int i=0; i < server->m_connections.m_count; ++i )
		{
			char buffer[SX_NET_BUFF_SIZE] = { 0 };
			Connection* con = server->m_connections[i];

			uint msize = con->Received( buffer, SX_NET_BUFF_SIZE );
			if ( msize > 0 )
				threadMan->AddTask( con->m_destination.ip, &dbconfig, buffer, msize );
		}

		for (int i = 0; i < server->m_connections.m_count; ++i)
		{
			Connection* con = server->m_connections[i];
			threadMan->Update( con->m_destination.ip, con );
		}

		threadMan->CheckThreadsTime( elpsTime );

		Sleep(1);
	}


	sx_net_finalize();

	getchar();
	sx_detect_crash();
	return 0;
}