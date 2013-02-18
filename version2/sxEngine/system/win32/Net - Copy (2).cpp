#if defined(_WIN32)

#include "../Net.h"
#include "../Log.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")



#define NET_MAX_NUM_BUFFER				512		//	number of connection buffer in the stack
#define NET_MAX_QUEUE_BUFFER			512		//	maximum number of buffer in the queue
#define NET_MESSAGE_SIZE				512		//	size of connection buffer
#define NET_SERVER_FLAG_TIME			1200	//	time period that server notify all client to see the flag by sending broadcast message
#define NET_CONNECTION_ALIVE_TIME		1000	//	keep connection age
#define NET_CONNECT_REQUEST_TIME		500		//	time period to send request message to connect


//	we use first byte after 4 bytes of protocol ID to identify the message type
enum NetMessageType
{
	NMT_CONNECT				= 1,		//	a connection message
	NMT_WELCOME,						//	welcome message
	NMT_DISCONNECT,						//	disconnect message
	NMT_SYNC,							//	sync connection with the others
	NMT_ALIVE,							//	keep connection age
	NMT_ACK,							//	request lost ack
	NMT_USER,							//	contain user data
	NMT_SERVER_OPEN,					//	broad cast message sent by server to notify that the server is open to accept clients
	NMT_SERVER_CLOSED					//	broad cast message sent by server to notify that the server is closed and no more clients will accepted
};

//	header of network pocket
struct NetPacketHeader
{
	word		id;						//	id of network system to avoid conflict with other network systems
	byte		type;					//	type of message
	byte		subtype;				//	sub type of message
	uint		ack;					//	message number
};

//	structure of network pocket
struct NetPacket
{
	NetPacketHeader	header;					//	header of message
	char			data[1024];				//	message data
};

//  use this buffer in buffer pool and queue of messages
struct NetMessage
{
	NetPacket	packet;

	//  additional info
	NetAddress	address;				//	address of sender
	uint		size;					//	size of message
	uint		bytsRecved;				//  hold number of bytes received

	NetMessage( dword id, dword type, uint ack ): size(0), bytsRecved(0)
	{
		packet.header.id		= id;
		packet.header.type		= type;
		packet.header.subtype	= 0;
		packet.header.ack		= ack;
	}
	NetMessage( void ) { ZeroMemory( this, sizeof(NetMessage) ); }
};

//////////////////////////////////////////////////////////////////////////
// use as initialized network structure
class NetInternal 
{
public:
	wchar					name[32];
	word					id;
	NetAddress				address;
	Stack<NetMessage*>		msgPool;
};
static NetInternal* s_netInternal = NULL;



//////////////////////////////////////////////////////////////////////////
//	additional functions
wchar* net_error_string( const sint code );
void net_connection_clear_queue( Connection* con );
bool net_check_address( const NetAddress* ad1, const NetAddress* ad2 );
int net_compare_ack( const NetMessage*& cb1, const NetMessage*& cb2 );
int net_copy_name( NetMessage* msg, const wchar* name );
void callback_connection_server(Connection* connection, const char* buffer, const uint size);
void callback_connection_client(Connection* connection, const char* buffer, const uint size);



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
#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: Can't initialize socket. error code : %d", WSAGetLastError() );
#endif
		return false;
	}

	// make it broadcast capable
	int i = 1;
	if( setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i) ) == SOCKET_ERROR )
	{
#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: Unable to make socket broadcast! error code : %d", WSAGetLastError() );
#endif
		Close();
		return false;
	}

	// bind to port
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( port );
	if ( bind( m_socket, (const sockaddr*) &address, sizeof(sockaddr_in) ) == SOCKET_ERROR )
	{
#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: Unable to bind socket! error code : %d", WSAGetLastError() );
#endif
		Close();
		return false;
	}

	// make non-blocking socket
	DWORD nonBlocking = 1;
	if ( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
	{
#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: Unable to make non-blocking socket! error code : %d", WSAGetLastError() );
#endif
		Close();
		return false;
	}

	return true;
}

void Socket::Close( void )
{
	if ( !m_socket ) return;
	closesocket( m_socket );
	m_socket = 0;
}

SEGAN_INLINE bool Socket::Send( const NetAddress& destination, const char* buffer, const int size )
{
	if ( !m_socket || !buffer || size<1 ) return false;

	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	if ( !destination.ip[0] && !destination.ip[1] && !destination.ip[2] && !destination.ip[3] )
	{
		address.sin_addr.s_addr = INADDR_BROADCAST;
	}
	else
	{
		address.sin_addr.S_un.S_un_b.s_b1 = destination.ip[0];
		address.sin_addr.S_un.S_un_b.s_b2 = destination.ip[1];
		address.sin_addr.S_un.S_un_b.s_b3 = destination.ip[2];
		address.sin_addr.S_un.S_un_b.s_b4 = destination.ip[3];
	}
	address.sin_port = htons( destination.port );

	int sentBytes = sendto( m_socket, buffer, (int)size, 0, (sockaddr*)&address, sizeof(address) );

	return sentBytes == size;
}

SEGAN_INLINE sint Socket::Receive( const char* buffer, const int size, NetAddress* OUT sender )
{
	if ( !m_socket || !buffer || size<1 ) return 0;

	if ( sender )
	{
		sockaddr_in from;
		int fromLength = sizeof( from );
		ZeroMemory( &from, fromLength );

		int receivedBytes = recvfrom( m_socket, (char*)buffer, size, 0, (sockaddr*)&from, &fromLength );

		if ( receivedBytes <= 0 )
			return 0;

		sender->ip[0] = from.sin_addr.S_un.S_un_b.s_b1;
		sender->ip[1] = from.sin_addr.S_un.S_un_b.s_b2;
		sender->ip[2] = from.sin_addr.S_un.S_un_b.s_b3;
		sender->ip[3] = from.sin_addr.S_un.S_un_b.s_b4;
		sender->port = ntohs( from.sin_port );

		return receivedBytes;
	}
	else
	{
		int receivedBytes = recvfrom( m_socket, (char*)buffer, size, 0, NULL, NULL );
		if ( receivedBytes <= 0 )
			return 0;
		return receivedBytes;
	}
}

//////////////////////////////////////////////////////////////////////////
//	connection implementation
//////////////////////////////////////////////////////////////////////////
Connection::Connection( void )
: m_state(STOPPED)
, m_socket(0)
, m_sntAck(0)
, m_recAck(0)
, m_timeout(0)
, m_sendTime(0)
, m_msgList(32)
, m_sentList(32)
, m_callBack(0)
, m_userData(0)
{
	ZeroMemory( &m_destination, sizeof(m_destination) );
}

Connection::~Connection( void )
{
	Stop();
}

bool Connection::Start( Socket* psocket, const NetAddress& destination )
{
	if ( !psocket || !psocket->m_socket || m_state != STOPPED ) return false;

	m_socket = psocket;
	m_sntAck = 0;
	m_recAck = 0;
	m_sendTime = 999999;
	m_timeout = 0;
	m_state = STARTED;
	memcpy( &m_destination, &destination, sizeof(destination) );

	return true;	
}

void Connection::Stop( void )
{
	if ( m_state == CONNECTED )
	{
		NetMessage netmsg( s_netInternal->id, NMT_DISCONNECT, 0 );
		m_socket->Send( m_destination, (char*)&netmsg, sizeof(NetPacketHeader) );
	}
	m_state = STOPPED;

	net_connection_clear_queue( this );
}

SEGAN_INLINE void Connection::Update( struct NetMessage* buffer, const float elpsTime, const float delayTime, const float timeOut )
{

	switch ( m_state )
	{
	case STOPPED:
	case DISCONNECTED: return;

	case STARTED:
		{
			//	try to sync the connection with destination
			m_sendTime += elpsTime;
			if ( m_sendTime > delayTime )
			{
				m_sendTime = 0;
				NetMessage msg( s_netInternal->id, NMT_SYNC, m_sntAck );
				m_socket->Send( m_destination, (char*)&msg, sizeof(NetPacketHeader) );
			}

			//	received sync message
			if (
				buffer->bytsRecved									&& 
				buffer->packet.header.id == s_netInternal->id		&& 
				buffer->packet.header.type == NMT_SYNC				&&
				net_check_address( &m_destination, &buffer->address )
				)
			{
				m_recAck = buffer->packet.header.ack;
				m_state = CONNECTED;
			}
		}
		break;

	case CONNECTED:

		//	message received
		if ( buffer->bytsRecved && buffer->packet.header.id == s_netInternal->id && net_check_address( &m_destination, &buffer->address ) )
		{
			m_timeout = 0;

			switch ( buffer->packet.header.type )
			{
			case NMT_DISCONNECT:
				buffer->bytsRecved = 0;			//  avoid process buffer by other connections
				Stop();
				return;

			case NMT_SYNC:
				{
					//	try to sync connection
					NetMessage msg( s_netInternal->id, NMT_SYNC, m_sntAck );
					m_socket->Send( m_destination, (char*)&msg, sizeof(NetPacketHeader) );
				}
				break;

			}
		}
		else	//	if ( buffer->bytsRecved && buffer->id == s_netInternal->id && net_check_address( &m_destination, &buffer->address ) )
		{
			m_timeout += elpsTime;
			if ( m_timeout > timeOut )
			{
				Stop();
				break;
			}
		}

		//	keep connection age
		m_sendTime += elpsTime;
		if ( m_sendTime > delayTime )
		{
			m_sendTime = 0;
			NetMessage msg( s_netInternal->id, NMT_ALIVE, m_sntAck );
			m_socket->Send( m_destination, (char*)&msg, sizeof(NetPacketHeader) );
		}

		break;
	}
}

bool Connection::Send( const char* buffer, const int size, const bool important /*= false */ )
{
	sx_assert( buffer );
	sx_assert( size > 0 );
	sx_assert( size < NET_MESSAGE_SIZE );

	if ( !buffer || size<1 || size>NET_MESSAGE_SIZE ) return false;

	switch ( m_state )
	{
	case STOPPED:
	case LISTENING:
	//case CONNECTING:
	case DISCONNECTED: return false;
	}

	if ( s_netInternal->msgPool.Count() )
	{
		NetMessage* msg;
		s_netInternal->msgPool.Pop( msg );
		msg->packet.header.id = s_netInternal->id;
		msg->packet.header.type = NMT_ALIVE;
		msg->packet.header.subtype = NMT_USER;
		msg->size = sizeof(NetPacketHeader) + size;
		memcpy( msg->packet.data, buffer, size );
		m_sendQueue.Push( msg );

		return true;
	}
	else return false;
}


//////////////////////////////////////////////////////////////////////////
//	server implementation
//////////////////////////////////////////////////////////////////////////
Server::Server( void )
: m_state(STOPPED)
, m_clientPort(0)
, m_numSend(0)
, m_flagTime(0)
, m_callback(null)
{
}

Server::~Server( void )
{
	Stop();
}

bool Server::Start( const word port, const word clientPort, const uint maxClients, CB_Server callback )
{
	if ( m_state != STOPPED ) return false;

	m_clientPort = clientPort;
	if ( m_socket.Open( port ) )
	{
		int numClients = maxClients > 0 ? maxClients : 1;
		for ( int i=0; i<numClients; i++ )
		{
			Connection* con = sx_new( Connection );
			con->m_userData = this;
			con->m_callBack = callback_connection_server;
			m_clients.PushBack( con );
		}

		m_callback = callback;
		m_state = STARTED;
		return true;
	}
	else return false;
}


void Server::Stop( void )
{
	if ( m_state == STOPPED ) return;

	//	notify that this server has stopped
	if ( s_netInternal )
	{
		NetMessage buffer( s_netInternal->id, NMT_SERVER_CLOSED, 0 );
		NetAddress broadcast; ZeroMemory( &broadcast.ip, sizeof(broadcast.ip) ); broadcast.port = m_clientPort;
		m_socket.Send( broadcast, (char*)&buffer, sizeof(NetPacketHeader) + net_copy_name( &buffer, m_name ) );
	}

	for ( int i=0; i<m_clients.Count(); i++ )
	{
		m_clients[i]->Stop();
		sx_delete( m_clients[i] );
	}
	m_clients.Clear();

	m_socket.Close();
}

void Server::Listen( void )
{
	if ( m_state == STOPPED ) return;
	m_state = LISTENING;
}

void Server::Run( void )
{
	if ( m_state == STOPPED ) return;

	//	notify that this server has stopped client acceptance
	NetMessage buffer( s_netInternal->id, NMT_SERVER_CLOSED, 0 );
	NetAddress broadcast; ZeroMemory( &broadcast.ip, sizeof(broadcast.ip) ); broadcast.port = m_clientPort;
	m_socket.Send( broadcast, (char*)&buffer, sizeof(NetPacketHeader) + net_copy_name( &buffer, m_name ) );

	for ( int i=0; i<m_clients.Count(); i++ )
	{
		//m_clients[i]->Start( m_socket, );
	}

	m_state = RUNNING;
}

void Server::Update( const float elpsTime, const float delayTime, const float timeOut )
{
	switch ( m_state )
	{
	case STOPPED: return;

	case LISTENING:

		//	listen to any client who wants to join the server
		{
			NetMessage buffer;
			buffer.bytsRecved = m_socket.Receive( (char*)&buffer, NET_MESSAGE_SIZE, &buffer.address );

			if ( buffer.bytsRecved && buffer.packet.header.id == s_netInternal->id && buffer.packet.header.type == NMT_SYNC )
			{
				buffer.bytsRecved = 0;

				for ( int i=0; i<m_clients.Count(); ++i )
				{
					Connection* pclient = m_clients[i];

					if ( pclient->m_state != CONNECTED )
					{
						pclient->Start( &m_socket, buffer.address );
					}
				}
			}

		}

		//	broad cast flag message to network to rise the flag up
		m_flagTime += elpsTime;
		if ( m_flagTime > NET_SERVER_FLAG_TIME )
		{
			m_flagTime = 0;

			NetMessage buffer( s_netInternal->id, NMT_SERVER_OPEN, 0 );
			NetAddress broadcast; ZeroMemory( &broadcast.ip, sizeof(broadcast.ip) ); broadcast.port = m_clientPort;
			m_socket.Send( broadcast, (char*)&buffer, sizeof(NetPacketHeader) + net_copy_name( &buffer, m_name ) );
		}

		// break; connections should be update :)

	case RUNNING:
		{
			NetMessage buffer;
			buffer.bytsRecved = m_socket.Receive( (char*)&buffer, NET_MESSAGE_SIZE, &buffer.address );

			for ( int i=0; i<m_clients.Count(); i++ )
			{
				Connection* pclient = m_clients[i];

				switch ( pclient->m_state )
				{
				case STARTED:
				case STOPPED:
				case LISTENING:
				case DISCONNECTED:
					pclient->m_name = m_name;
					ZeroMemory ( &pclient->m_destination, sizeof(pclient->m_destination) );
				}

				pclient->Update( &buffer, elpsTime, delayTime, timeOut );

				switch ( pclient->m_state )
				{
				case STARTED:
				case STOPPED:
				case LISTENING:
				case DISCONNECTED:
					pclient->m_name.Clear();
					ZeroMemory ( &pclient->m_destination, sizeof(pclient->m_destination) );
				}
			}
		}
		break;

	}
}

bool Server::Send( const char* buffer, const int size, const bool important /*= false */ )
{
	sx_assert( buffer );
	sx_assert( size > 0 );
	sx_assert( size < NET_MESSAGE_SIZE );

	if ( !buffer || size<1 || size>NET_MESSAGE_SIZE ) return false;

	switch ( m_state )
	{
	case STOPPED:
		return false;
	}

	for ( int i=0; i<m_clients.Count(); i++ )
	{
		m_clients[i]->Send( buffer, size );
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
//	client implementation
//////////////////////////////////////////////////////////////////////////
Client::Client( void )
: m_state(STOPPED)
, m_callback(0)
, m_time(0)
{

}

Client::~Client( void )
{
	Stop();
}

bool Client::Start( const word port, const word serverPort, CB_Client callback )
{
	if ( m_state != STOPPED ) return false;
	m_serverPort = serverPort;
	if ( m_socket.Open( port ) )
	{
		m_callback = callback;
		m_connection.m_userData = this;
		m_connection.m_callBack = callback_connection_client;
		m_state = LISTENING;
	}
	return false;
}

void Client::Stop( void )
{
	if ( m_state == STOPPED ) return;
	m_connection.Stop();
	m_socket.Close();
	m_state = STOPPED;
}

void Client::Connect( NetAddress& destination )
{
	if ( m_state == CONNECTED ) return;

	m_connection.m_name = m_name;
	m_connection.m_destination = destination;
	m_state = CONNECTING;
}

void Client::Disconnect( void )
{
	if ( m_state != CONNECTED ) return;
	m_state = DISCONNECTED;
}

void Client::Update( const float elpsTime, const float delayTime, const float timeOut )
{
	NetMessage buffer;

	switch ( m_state )
	{
	case STOPPED:	return;

	case LISTENING:

		//	listen to the port to find any server
		buffer.bytsRecved = m_socket.Receive( (char*)&buffer, NET_MESSAGE_SIZE, &buffer.address );

		if ( buffer.bytsRecved && buffer.packet.header.id == s_netInternal->id ) 
		{
			ServerInfo serverInfo;
			memcpy( serverInfo.address.ip, buffer.address.ip, 4 );
			serverInfo.address.port = buffer.address.port;
			serverInfo.age = 2000;

			if ( buffer.packet.header.type == NMT_SERVER_OPEN )
			{
				bool itisnew = true;
				for ( int i=0; i<m_servers.Count(); i++ )
				{
					if ( memcmp( &m_servers[i].address, &serverInfo.address, sizeof(serverInfo.address) ) == 0 )
					{
						sx_str_copy( m_servers[i].name, 32, (wchar*)buffer.packet.data );
						m_servers[i].age = serverInfo.age;
						itisnew = false;
						break;
					}
				}
				if ( itisnew )
				{
					sx_str_copy( serverInfo.name, 32, (wchar*)buffer.packet.data );
					m_servers.PushBack( serverInfo );
				}
			}
			else if ( buffer.packet.header.type == NMT_SERVER_CLOSED )
			{
				for ( int i=0; i<m_servers.Count(); i++ )
				{
					if ( memcmp( &m_servers[i].address, &serverInfo.address, sizeof(serverInfo.address) ) == 0 )
					{
						m_servers.RemoveByIndex( i );
						break;
					}
				}
			}
			buffer.bytsRecved = 0;
		}
		else
		{
			for ( int i=0; i<m_servers.Count(); i++ )
			{
				m_servers[i].age -= elpsTime;
				if ( m_servers[i].age < 0 )
					m_servers.RemoveByIndex(i);
			}
		}

		break;

	case CONNECTING:
		{
			m_time += elpsTime;
			if ( m_time > delayTime )
			{
				m_time = 0;
				NetMessage msg( s_netInternal->id, NMT_CONNECT, 0 );
				m_socket->Send( m_destination, (char*)&msg, sizeof(NetPacketHeader) );
			}
		}
		break;

	case DISCONNECTED:
		m_connection.Stop();
		m_state = LISTENING;
		break;

	default:
		{
			buffer.bytsRecved = m_socket.Receive( (char*)&buffer, NET_MESSAGE_SIZE, &buffer.address );
			m_connection.Update( &buffer, elpsTime, delayTime, timeOut );
		}
		break;
	}
}


bool Client::Send( const char* buffer, const int size, const bool important /*= false */ )
{
	return m_connection.Send( buffer, size, important );
}


//////////////////////////////////////////////////////////////////////////
//	network functions
//////////////////////////////////////////////////////////////////////////
SEGAN_ENG_API bool sx_net_initialize( const dword netID )
{
	sx_callstack();
	sx_assert( !s_netInternal );
	bool netInitialized = true;

	//  initialize internal net object
	s_netInternal = sx_new( NetInternal );
	s_netInternal->id = netID;
	ZeroMemory( &s_netInternal->address, sizeof(s_netInternal->address) );

 	for (int i=0; i<NET_MAX_NUM_BUFFER; i++)
 		s_netInternal->msgPool.Push( (NetMessage*)mem_alloc( sizeof(NetMessage) ) );

	//  initialize windows socket
	WSADATA wsaData;
	if( ::WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) )
	{

#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: Network initialization on Windows failed! error code : %s " , net_error_string( WSAGetLastError() ) );
#endif
		
		netInitialized = false;
	}

	//  check initialized version
	bool incorrectVersion = LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2;
	if ( netInitialized && incorrectVersion )
	{

#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: Network initialization on Windows failed! Invalid version detected!" );
#endif
		
		netInitialized = false;
	}

	// Get local host name
	char hostName[128] = {0};
	if( netInitialized && ::gethostname( hostName, sizeof(hostName) ) )
	{
#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: function ::gethostname() failed with error code : %d !", WSAGetLastError() );
#endif
		netInitialized = false;
	}

	// Get local IP address
	hostent* pHost = ::gethostbyname( hostName );
	if( netInitialized && !pHost )
	{
#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: function ::gethostbyname() failed with error code : %d !", WSAGetLastError() );
#endif
		netInitialized = false;
	}
	else
	{	//  copy name
		sint i = 0;
		for ( ; i<30 && hostName[i]; i++)
			s_netInternal->name[i] = hostName[i];
		s_netInternal->name[i] = 0;
	}

	if ( pHost->h_addr_list[0] )
	{	//  copy IP address
		s_netInternal->address.ip[0] = pbyte(pHost->h_addr_list[0])[0];
		s_netInternal->address.ip[1] = pbyte(pHost->h_addr_list[0])[1];
		s_netInternal->address.ip[2] = pbyte(pHost->h_addr_list[0])[2];
		s_netInternal->address.ip[3] = pbyte(pHost->h_addr_list[0])[3];
	}
	else
	{

#if LOG_NETWORK_ERROR
		g_logger->Log( L"Error: Can't detect current IP !" );
#endif

		netInitialized = false;
	}

	if ( !netInitialized )
	{

#if LOG_NETWORK_ERROR
		g_logger->Log( L"The network system is now disabled." );
#endif
		
		WSACleanup();
		return false;
	}
	else
	{

#if LOG_NETWORK_INFO
		g_logger->Log(  L"Network system initialized successfully on Windows." );
		g_logger->Log_( L"    Name:		%s \r\n" , s_netInternal->name );
		g_logger->Log_( L"    IP:			%d.%d.%d.%d \r\n\r\n" , 
			s_netInternal->address.ip[0], s_netInternal->address.ip[1], s_netInternal->address.ip[2], s_netInternal->address.ip[3] );
#endif
		
		return 0;
	}
}

void sx_net_finalize( void )
{
	sx_callstack();
	sx_assert( s_netInternal );

	NetMessage* netMsg = null;
	while ( s_netInternal->msgPool.Pop(netMsg) )
	{
		mem_free( netMsg );
	}
	s_netInternal->msgPool.Clear();

	sx_delete_and_null( s_netInternal );

	WSACleanup();
	g_logger->Log( L"Network system Finalized.  \r\n" );
}


//////////////////////////////////////////////////////////////////////////
//	additional functions
//////////////////////////////////////////////////////////////////////////
wchar* net_error_string( const sint code )
{
	switch( code ) {
	case WSAEINTR:				return L"WSAEINTR";
	case WSAEBADF:				return L"WSAEBADF";
	case WSAEACCES: 			return L"WSAEACCES";
	case WSAEDISCON: 			return L"WSAEDISCON";
	case WSAEFAULT: 			return L"WSAEFAULT";
	case WSAEINVAL: 			return L"WSAEINVAL";
	case WSAEMFILE: 			return L"WSAEMFILE";
	case WSAEWOULDBLOCK: 		return L"WSAEWOULDBLOCK";
	case WSAEINPROGRESS: 		return L"WSAEINPROGRESS";
	case WSAEALREADY: 			return L"WSAEALREADY";
	case WSAENOTSOCK: 			return L"WSAENOTSOCK";
	case WSAEDESTADDRREQ: 		return L"WSAEDESTADDRREQ";
	case WSAEMSGSIZE: 			return L"WSAEMSGSIZE";
	case WSAEPROTOTYPE: 		return L"WSAEPROTOTYPE";
	case WSAENOPROTOOPT: 		return L"WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: 	return L"WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: 	return L"WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP: 		return L"WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT: 		return L"WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT: 		return L"WSAEAFNOSUPPORT";
	case WSAEADDRINUSE: 		return L"WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL: 		return L"WSAEADDRNOTAVAIL";
	case WSAENETDOWN: 			return L"WSAENETDOWN";
	case WSAENETUNREACH: 		return L"WSAENETUNREACH";
	case WSAENETRESET: 			return L"WSAENETRESET";
	case WSAECONNABORTED:		return L"WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET: 		return L"WSAECONNRESET";
	case WSAENOBUFS: 			return L"WSAENOBUFS";
	case WSAEISCONN: 			return L"WSAEISCONN";
	case WSAENOTCONN: 			return L"WSAENOTCONN";
	case WSAESHUTDOWN: 			return L"WSAESHUTDOWN";
	case WSAETOOMANYREFS: 		return L"WSAETOOMANYREFS";
	case WSAETIMEDOUT: 			return L"WSAETIMEDOUT";
	case WSAECONNREFUSED: 		return L"WSAECONNREFUSED";
	case WSAELOOP: 				return L"WSAELOOP";
	case WSAENAMETOOLONG: 		return L"WSAENAMETOOLONG";
	case WSAEHOSTDOWN: 			return L"WSAEHOSTDOWN";
	case WSASYSNOTREADY: 		return L"WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: 	return L"WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED: 	return L"WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND: 	return L"WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN: 			return L"WSATRY_AGAIN";
	case WSANO_RECOVERY: 		return L"WSANO_RECOVERY";
	case WSANO_DATA: 			return L"WSANO_DATA";
	default: 					return L"UNKNOWN";
	}
}

SEGAN_INLINE void net_connection_clear_queue( Connection* con )
{
	if ( con->m_msgList.Count() )
	{
		for ( int i=0; i<con->m_msgList.Count(); i++ )
		{
			NetMessage* pbuf = con->m_msgList[i];
			s_netInternal->msgPool.Push( pbuf );
		}

		con->m_msgList.Clear();
	}

	if ( con->m_sentList.Count() )
	{
		for ( int i=0; i<con->m_sentList.Count(); i++ )
		{
			NetMessage* pbuf = con->m_sentList[i];
			s_netInternal->msgPool.Push( pbuf );
		}

		con->m_sentList.Clear();
	}

	if ( con->m_sendQueue.Count() )
	{
		while ( con->m_sendQueue.Count() )
		{
			NetMessage* pbuf;
			con->m_sendQueue.Pop( pbuf );
			s_netInternal->msgPool.Push( pbuf );
		}

		con->m_sendQueue.Clear();
	}
}

SEGAN_INLINE bool net_check_address( const NetAddress* ad1, const NetAddress* ad2 )
{
	return memcmp( ad1, ad2, sizeof(NetAddress) ) == 0;
}

SEGAN_INLINE sint net_compare_ack( const NetMessage*& cb1, const NetMessage*& cb2 )
{
	return ( cb1->packet.header.ack > cb2->packet.header.ack ) ? 1 : ( (cb1->packet.header.ack < cb2->packet.header.ack) ? -1 : 0 );
}

sint net_copy_name( NetMessage* msg, const wchar* name )
{
	return sx_str_copy( (wchar*)msg->packet.data, 32, name ? name : s_netInternal->name ) * 2;
}

SEGAN_INLINE void callback_connection_server( Connection* connection, const char* buffer, const uint size )
{
	Server* server = (Server*)connection->m_userData;
	if ( server->m_callback )
	{
		server->m_callback( server, connection, buffer, size );
	}
}


SEGAN_INLINE void callback_connection_client( Connection* connection, const char* buffer, const uint size )
{
	Client* client = (Client*)connection->m_userData;
	if ( client->m_callback )
	{
		client->m_callback( client, buffer, size );
	}
}


#endif
