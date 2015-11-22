#include <windows.h>

#include "DatabaseServer.h"


#define NET_ID 32
#define BUF_SZ 1024	

int wmain(int argc, wchar* argv[])
{
	sx_callstack();
	sx_net_initialize();

	DatabaseServer dbServer;
	dbServer.Initialize();
	dbServer.LoadConfig(  L"config.txt" );
	while (true)
	{
		dbServer.Update();
		Sleep(1);
	}
	dbServer.Finalize();

	sx_net_finalize();

	getchar();
	sx_detect_crash();
	return 0;
}