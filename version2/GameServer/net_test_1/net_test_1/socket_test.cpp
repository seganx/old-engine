#include "net_test_1.h"

#define BUF_SZ 1024	

int socket_test(int argc, wchar* argv[])
{
	sx_callstack();

	bool isServer = false;
	word currPort = 31000;
	word serverPort = 32000;

	if (argc > 1 && sx_str_cmp(argv[1], "server") == 0)
		isServer = true;
	if (argc > 2)
		currPort = sx_str_to_uint(argv[2]);
	if (argc > 3)
		serverPort = sx_str_to_uint(argv[3]);

	Socket* s = sx_new(Socket);
	s->Open(currPort);

	if (!isServer)
		sx_print(L"Client starts to send data to port: %d\n", serverPort);

	dword elpsTime = GetTickCount();
	word msgCount = 0;
	while (true)
	{
		if (GetTickCount() - elpsTime > 1000)
		{
			if (isServer)
				sx_print(L"Number of messages received: %d\n", msgCount);
			else
				sx_print(L"Number of messages sent: %d\n", msgCount);

			elpsTime = GetTickCount();
			msgCount = 0;
		}

		wchar buffer[BUF_SZ] = { 0 };
		if (isServer)
		{
			NetAddress ra(0, 0, 0, 0, 0);
			if (s->Receive(buffer, 1024, &ra))
			{
				msgCount++;
				//sx_print(L"Received: %s\n", buffer);
			}
		}
		else
		{
			NetAddress na(192, 168, 90, 102, serverPort);
			uint len = sx_time_to_str(buffer, BUFSIZ, L"%y/%m/%d %H:%M:%S");
			if (s->Send(na, buffer, 128))
			{
				msgCount++;
				//sx_print(L"Sent: %s\n", buffer);
			}
		}

		//Sleep(0);
	}

	return 0;
}