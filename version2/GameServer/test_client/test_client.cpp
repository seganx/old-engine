#include "../Net.h"
#include "../Socket.h"
#include "../Connection.h"
#include "../Server.h"

#include <windows.h>

#define NET_ID 32
#define CON_SP 10
#define BUF_SZ 1024	

wchar* persianName[] = {L"Sajad", L"Mamad", L"Amin"};

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
			if (sx_net_verify_packet(buffer, receivedBytes, con->m_RecvNumber))
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

	word currPort = 33000;
	word serverPort = 80;

	if (argc > 1)
		currPort = sx_str_to_uint(argv[1]);
	if (argc > 2)
		serverPort = sx_str_to_uint(argv[2]);

	NetAddress serverAddress(217, 218, 62, 206, serverPort);

	Socket* s = sx_new Socket;
	s->Open(currPort);

	while (true)
	{
		s->Send(serverAddress, "HELLO", 6);
	}

	Connection* con = sx_new Connection;
	con->Open(serverAddress);
	con->SetSpeed( CON_SP );

	int nameIndex = 0;
	double sendTime = 0;
	double sendTimeMax = 1.0 / CON_SP;
	double lastTime = sx_net_get_timer();
	while (true)
	{
		double tickCount = sx_net_get_timer();
		double elpsTime = (tickCount - lastTime);
		lastTime = tickCount;
		if ( elpsTime < 0 ) continue;

		sendTime += elpsTime;
		if ( sendTime > sendTimeMax )
		{
			sendTime = 0;
			if (nameIndex > 2) nameIndex = 0;
			//wchar buffer[BUF_SZ] = { 0 };
			//uint len = sx_time_to_str(buffer, BUFSIZ, L"%y/%m/%d %H:%M:%S");
			//con->Send(buffer, len * 2, SX_NET_OPTN_SAFESEND);
			
			wchar* buff = persianName[nameIndex++];
			con->Send(buff, sx_str_len(buff) * 2 + 2, SX_NET_OPTN_SAFESEND);
		}

		peek_received_messages(s, con);
		con->Update(s, elpsTime);

		{
			char buffer[SX_NET_BUFF_SIZE] = {0};
			if ( con->Received( buffer, SX_NET_BUFF_SIZE) > 0 )
				sx_print( L"Received: %s\n", buffer );
		}

		Sleep(1);
	}


	sx_net_finalize();

	getchar();
	sx_detect_crash();
	return 0;
}

