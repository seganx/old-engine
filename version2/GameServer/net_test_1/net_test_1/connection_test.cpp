#include "net_test_1.h"

#define BUF_SZ 1024	

int connection_test(int argc, wchar* argv[])
{
	sx_callstack();

	word currPort = 31000;
	word serverPort = 32000;

	if (argc > 2)
		currPort = sx_str_to_uint(argv[2]);
	if (argc > 3)
		serverPort = sx_str_to_uint(argv[3]);

	NetAddress serverAddress( 192, 168, 90, 102, serverPort );

	Socket* s = sx_new Socket;
	s->Open( currPort );
		
	Connection* con = sx_new Connection;
	con->Open( serverAddress );
	con->SetSpeed( 5 );


	float sendTime = 0;
	dword lastTime = GetTickCount();
	while (true)
	{
		dword tickCount = GetTickCount();
		float elpsTime = (tickCount - lastTime) * 0.001f;
		lastTime = tickCount;

		sendTime += elpsTime;
		if (sendTime > 0.2f)
		{
			wchar buffer[BUF_SZ] = { 0 };
			uint len = sx_time_to_str(buffer, BUFSIZ, L"%y/%m/%d %H:%M:%S");
			if (con->Send(buffer, len, SX_NET_OPTN_SAFESEND))
				sx_print(L"Sent: %s\n", buffer);
			sendTime = 0;
		}

		con->Update( s, elpsTime );
	}

	return 0;
}