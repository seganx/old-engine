#include "net_test_1.h"

int wmain(int argc, wchar* argv[])
{
	sx_callstack();

	sx_net_initialize();

	//socket_test(argc, argv);
	connection_test(argc, argv);

	sx_net_finalize();

	getchar();
	sx_detect_crash();
	return 0;
}

