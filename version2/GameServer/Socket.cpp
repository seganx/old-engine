#if defined(_WIN32)

#include "Socket.h"
#include "Net.h"
#include <stdio.h>
#include <winsock2.h>


//////////////////////////////////////////////////////////////////////////
//	socket implementation
//////////////////////////////////////////////////////////////////////////
Socket::Socket( void ): m_socket(0)
{

}

Socket::~Socket( void )
{
	if ( m_socket ) Close();
}

bool Socket::Open( const word port )
{
	if ( m_socket ) Close();

	// create socket
	m_socket = (uint)socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( m_socket == INVALID_SOCKET )
	{
		sx_print( L"Error: Can't initialize socket. error code : %s !" , sx_net_error_string( WSAGetLastError() ) );
		return false;
	}

#if SX_NET_BROADCAST
	// make it broadcast capable
	int i = 1;
	if( setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i) ) == SOCKET_ERROR )
	{
		sx_print(L"Error: Unable to make socket broadcast! error code : %s !", sx_net_error_string(WSAGetLastError()));
		Close();
		return false;
	}
#endif

	// bind to port
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( port );
	if ( bind( m_socket, (const sockaddr*) &address, sizeof(sockaddr_in) ) == SOCKET_ERROR )
	{
		sx_print(L"Error: Unable to bind socket! error code : %s !", sx_net_error_string(WSAGetLastError()));
		Close();
		return false;
	}

	// make non-blocking socket
	DWORD nonBlocking = 1;
	if ( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
	{
		sx_print(L"Error: Unable to make non-blocking socket! error code : %s !", sx_net_error_string(WSAGetLastError()));
		Close();
		return false;
	}

	sx_print(L"Info: Socket has been opened on port : %d", port);
	return true;
}

void Socket::Close( void )
{
	if ( !m_socket ) return;
	closesocket( m_socket );
	m_socket = 0;
}

bool Socket::Send( const NetAddress& destination, const void* buffer, const int size )
{
	sx_assert( m_socket || buffer || size>0 );
	sx_callstack();

	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = htons( destination.port );

#if SX_NET_BROADCAST
	if ( !destination.ip_bytes[0] && !destination.ip_bytes[1] && !destination.ip_bytes[2] && !destination.ip_bytes[3] )
	{
		address.sin_addr.s_addr = INADDR_BROADCAST;
	}
	else
	{
		address.sin_addr.S_un.S_un_b.s_b1 = destination.ip_bytes[0];
		address.sin_addr.S_un.S_un_b.s_b2 = destination.ip_bytes[1];
		address.sin_addr.S_un.S_un_b.s_b3 = destination.ip_bytes[2];
		address.sin_addr.S_un.S_un_b.s_b4 = destination.ip_bytes[3];
	}
#else
	address.sin_addr.s_addr = destination.ip;
#endif
	int sentBytes = sendto( m_socket, (char*)buffer, size, 0, (sockaddr*)&address, sizeof(address) );
	return sentBytes == size;
}

sint Socket::Receive( void* buffer, const int size, NetAddress* OUT sender )
{
	sx_assert( m_socket || buffer || size>0 );
	sx_callstack();

	DWORD receivedBytes = 0;
	if ( sender )
	{
#if 0
		struct sockaddr_storage from;
		int fromLength = sizeof( sockaddr_storage );
		//receivedBytes = recvfrom( m_socket, (char*)buffer, size, 0, (sockaddr*)&from, &fromLength );

		DWORD flag = 0;
		WSABUF winBuf;
		winBuf.len = size;
		winBuf.buf = (char*)buffer;
		int res = ::WSARecvFrom( m_socket, &winBuf, 1, &receivedBytes, &flag, (sockaddr*)&from, &fromLength, null, null );
		if ( res == 0 && receivedBytes > 0 )
		{
			switch ( from.ss_family )
			{
				case AF_INET:
				{
					sockaddr_in* sa = (sockaddr_in*)&from;
					sender->ip = sa->sin_addr.S_un.S_addr;
					sender->port = ntohs(sa->sin_port);


					sx_print_a("\ngetting host name: ...");
					struct hostent* h = gethostbyaddr((const char*)&sender->ip, 4, AF_INET);
					sx_print_a("\ndone: ");
					//byte* hp = (byte*)h->h_addr_list[0];
					sx_print_a("\n\nName: %s", h->h_name);
					//sx_print_a("\n\nName: %s, adrs: %u.%u.%u.%u", h->h_name, hp[0], hp[1], hp[2], hp[3]);
					Sleep(10000);
				}
					break;
				case AF_INET6:
					break;
			}
		}
#else
		struct sockaddr_in from;
		int fromlen = sizeof(from);
		sx_mem_set(&from, 0, sizeof(from));
		receivedBytes = recvfrom(m_socket, (char*)buffer, size, 0, (sockaddr*)&from, &fromlen);
		if ( receivedBytes > 0)
		{
			sender->ip = from.sin_addr.S_un.S_addr;
			sender->port = ntohs(from.sin_port);
		}
#endif

	}
	else
	{
		receivedBytes = recvfrom( m_socket, (char*)buffer, size, 0, NULL, NULL );
	}

	return ( receivedBytes <= 0 ? 0 : receivedBytes );
}


#endif
