#include "../Net.h"
#include "../Socket.h"
#include "../Connection.h"
#include "../Server.h"

#include <windows.h>

#define NET_ID 32
#define CON_SP 100
#define BUF_SZ 1024	



void peek_received_messages(Socket* socket, Connection* con)
{
	sx_callstack();
	NetAddress address;
	char buffer[SX_NET_BUFF_SIZE];

	int receivedBytes = 0;
	do
	{
		//	peek the package
		receivedBytes = socket->Receive(buffer, SX_NET_BUFF_SIZE, &address);
		if (receivedBytes < 1) continue;

		//	check if we have a connection for the received address
		if ( con->m_destination.ip == address.ip )
		{
			//	verify that the package is valid
			if (sx_net_verify_package(buffer, receivedBytes, con->m_RecvNumber))
			{
				//	append the package to the connection buffer
				con->AppendReceivedMessage(socket, buffer, receivedBytes);
			}
		}
		
	} 
	while (receivedBytes > 0);
}


int wmain(int argc, wchar* argv[])
{
	sx_callstack();

	sx_net_initialize();

	word currPort = 31000;
	word serverPort = 32000;

	if (argc > 1)
		currPort = sx_str_to_uint(argv[1]);
	if (argc > 2)
		serverPort = sx_str_to_uint(argv[2]);

	NetAddress serverAddress(192, 168, 90, 102, serverPort);

	Socket* s = sx_new Socket;
	s->Open(currPort);

	Connection* con = sx_new Connection;
	con->Open(serverAddress);
	con->SetSpeed( CON_SP );

	double sendTime = 0;
	double sendTimeMax = 1.0 / CON_SP;
	double lastTime = sx_net_get_timer();
	while (true)
	{
		double tickCount = sx_net_get_timer();
		double elpsTime = (tickCount - lastTime);
		lastTime = tickCount;

		sendTime += elpsTime;
		if ( sendTime > sendTimeMax )
		{
			sendTime = 0;

			wchar buffer[BUF_SZ] = { 0 };
			uint len = sx_time_to_str(buffer, BUFSIZ, L"%y/%m/%d %H:%M:%S");
			con->Send(buffer, len * 2, SX_NET_OPTN_SAFESEND);
		}

		peek_received_messages(s, con);
		con->Update(s, elpsTime);

		Sleep(1);
	}


	sx_net_finalize();

	getchar();
	sx_detect_crash();
	return 0;
}

