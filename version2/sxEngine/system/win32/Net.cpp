#if defined(_WIN32)

#include "../Net.h"
#include "../Log.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


#define NET_MAX_NUM_BUFFER				512		//	number of connection buffer in the stack
#define NET_MAX_QUEUE_BUFFER			512		//	maximum number of buffer in the queue
#define NET_PACKET_SIZE					512		//	size of connection buffer
#define NET_SERVER_FLAG_TIME			1200	//	time period that server notify all client to see the flag by sending broadcast message


//	we use first byte after 4 bytes of protocol ID to identify the message type
enum NetPacketType
{
	NPT_CONNECT				= 1,		//	a connection message
	NPT_WELCOME,						//	welcome message
	NPT_DISCONNECT,						//	disconnect message
	NPT_ALIVE,							//	keep connection alive
	NPT_ACK,							//	request lost ack
	NPT_USER,							//	contain user data
	NPT_ZIP,							//	contain array compressed packet
	NPT_SERVER_OPEN,					//	broad cast message sent by server to notify that the server is open to accept clients
	NPT_SERVER_CLOSED					//	broad cast message sent by server to notify that the server is closed and no more clients will accepted
};

//	header of network pocket
struct NetPacketHeader
{
	word		id;						//	id of network system to avoid conflict with other network systems
	byte		type;					//	type of message
	byte		crit;					//	message is critical
	uint		ack;					//	message number

	NetPacketHeader( const word _id, const byte _type, const uint _ack ): id(_id), type(_type), crit(false), ack(_ack){}
	NetPacketHeader( void ){}
};

//	structure of network pocket
struct NetPacket
{
	NetPacketHeader	header;				//	header of message
	byte			data[512];			//	message data

	NetPacket( const word id, const byte type, const uint ack ): header(id, type, ack){}
	NetPacket( void ){}
};

//  structure of received message 
struct NetMessage
{
	NetPacket	packet;

	NetAddress	address;				//	address of sender
	uint		size;					//	size of message
	uint		bytsRecved;				//  hold number of bytes received

	NetMessage( void ) { ZeroMemory( this, sizeof(NetMessage) ); }
};
typedef NetMessage* PNetMessage;

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

SEGAN_INLINE bool net_check_address( const NetAddress* ad1, const NetAddress* ad2 )
{
	return memcmp( ad1, ad2, sizeof(NetAddress) ) == 0;
}

SEGAN_INLINE sint net_compare_ack( const PNetMessage& cb1, const PNetMessage& cb2 )
{
	return ( cb1->packet.header.ack > cb2->packet.header.ack ) ? 1 : ( (cb1->packet.header.ack < cb2->packet.header.ack) ? -1 : 0 );
}

SEGAN_INLINE sint net_copy_name( NetPacket* packet, const wchar* name )
{
	return sx_str_copy( (wchar*)packet->data, 32, name ? name : s_netInternal->name ) * 2;
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

SEGAN_INLINE void net_connection_flush_list( Connection* con )
{
	if ( con->m_msgList.Count() )
	{
		con->m_msgList.Sort( net_compare_ack );

		for ( int i=0; i<con->m_msgList.Count(); i++ )
		{
			NetMessage* pbuf = con->m_msgList[i];
			if ( con->m_recAck == pbuf->packet.header.ack )
			{
				con->m_callBack( con, pbuf->packet.data, pbuf->bytsRecved );
				con->m_recAck++;

				s_netInternal->msgPool.Push( pbuf );
			}
		}
		con->m_msgList.Clear();
	}
}

SEGAN_INLINE void net_connection_send_from_queue( Connection* con )
{
	while ( con->m_sendQueue.Count() )
	{
		NetMessage* msg;

		//	pop the message from list
		if ( con->m_sendQueue.Pop( msg ) )
		{
			//	send to destination
			msg->packet.header.ack = con->m_sntAck;
			con->m_socket->Send( con->m_destination, &msg->packet, msg->size );

			//	push the message to the sent items
			if ( msg->packet.header.crit )
			{
				con->m_sntAck++;
				con->m_sentList.PushBack( msg );

				//	keep sent items limit
				if ( con->m_sentList.Count() == NET_MAX_QUEUE_BUFFER )
				{
					s_netInternal->msgPool.Push( con->m_sentList[0] );
					con->m_sentList.RemoveByIndex( 0 );
				}
			}
			else
			{
				s_netInternal->msgPool.Push( msg );
			}
		}
	}
}

SEGAN_INLINE bool net_connection_push_to_queue( NetMessage* buffer, Connection* con )
{
	bool res;

	//  verify that if message queue is going too long may be a message is lost
	if ( con->m_msgList.Count() < NET_MAX_QUEUE_BUFFER )
	{
		//	push the message to the queue
		NetMessage* netmsg;
		s_netInternal->msgPool.Pop( netmsg );
		memcpy( netmsg, buffer, sizeof(NetMessage) );
		con->m_msgList.PushBack( netmsg );
		res = true;
	}
	else
	{
		con->Disconnect();
		res = false;
	}

	return res;
}

SEGAN_INLINE void callback_connection_server( Connection* connection, const byte* buffer, const uint size )
{
	Server* server = (Server*)connection->m_userData;
	if ( server->m_callback )
		server->m_callback( server, connection, buffer, size );
}

SEGAN_INLINE void callback_connection_client( Connection* connection, const byte* buffer, const uint size )
{
	Client* client = (Client*)connection->m_userData;
	if ( client->m_callback )
		client->m_callback( client, buffer, size );
}

//	merge a packet into data and return new size of data. return 0 if the packets can't be merged
SEGAN_INLINE uint net_merge_packet( NetPacket* currpacket, const uint currsize, const NetPacket* packet, const uint packetsize )
{
	if ( currsize + packetsize >= NET_PACKET_SIZE ) return 0;

	sx_callstack();
	MemoryStream data;
	
	//	verify that current packet has merged packet
	if ( currpacket->header.type != NPT_ZIP )
	{
		//	set number of packet in the data
		data.WriteByte(2);

		//	copy the first packet to the data
		data.WriteUInt32( currsize );
		data.Write( currpacket, currsize );

		//	copy the second packet to the data
		data.WriteUInt32( packetsize );
		data.Write( packet, packetsize );

		//	compress data
		//zlib_compress_stream()

		//	store data to packet
		data.SetPos(0);
		data.CopyTo( currpacket->data );

		//	change the header of packet
		currpacket->header.type = NPT_ZIP;
	}
	else
	{
		//	decompress the data !!! in the next code ;)

		//	update number of packets
		currpacket->data[0] += 1;

		//	copy packet data to the data container
		data.Write( currpacket->data, currsize - sizeof(NetPacketHeader) );

		//	copy the new packet to the data
		data.WriteUInt32( packetsize );
		data.Write( packet, packetsize );

		//	store data to packet
		data.SetPos(0);
		data.CopyTo( currpacket->data );
	}

	return data.Size() + sizeof(NetPacketHeader);
}

SEGAN_INLINE void net_unmerge_packet( byte* data, Connection* con )
{
	sint n = data[0], p = 1;
	for ( int i=0; i<n; i++ )
	{
		uint* size = (uint*)&data[p]; p+=4;
		NetPacket* packet = (NetPacket*)&data[p];

		con->m_callBack( con, packet->data, *size );
	}
}

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
		g_logger->Log( L"Error: Can't initialize socket. error code : %s !" , net_error_string( WSAGetLastError() ) );
		return false;
	}

	// make it broadcast capable
	int i = 1;
	if( setsockopt( m_socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i) ) == SOCKET_ERROR )
	{
		g_logger->Log( L"Error: Unable to make socket broadcast! error code : %s !" , net_error_string( WSAGetLastError() ) );
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
		g_logger->Log( L"Error: Unable to bind socket! error code : %s !" , net_error_string( WSAGetLastError() ) );
		Close();
		return false;
	}

	// make non-blocking socket
	DWORD nonBlocking = 1;
	if ( ioctlsocket( m_socket, FIONBIO, &nonBlocking ) == SOCKET_ERROR )
	{
		g_logger->Log( L"Error: Unable to make non-blocking socket! error code : %s !" , net_error_string( WSAGetLastError() ) );
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

SEGAN_INLINE bool Socket::Send( const NetAddress& destination, const void* buffer, const int size )
{
	sx_callstack();
	sx_assert( m_socket || buffer || size>0 );

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

	int sentBytes = sendto( m_socket, (char*)buffer, (int)size, 0, (sockaddr*)&address, sizeof(address) );
	return sentBytes == size;
}

SEGAN_INLINE sint Socket::Receive( const void* buffer, const int size, NetAddress* OUT sender )
{
	sx_callstack();
	sx_assert( m_socket || buffer || size>0 );

	int receivedBytes = 0;
	if ( sender )
	{
		SOCKADDR_IN from;
		int fromLength = sizeof( from );
		receivedBytes = recvfrom( m_socket, (char*)buffer, size, 0, (sockaddr*)&from, &fromLength );
		if ( receivedBytes > 0 )
		{
			sender->ip[0]	= from.sin_addr.S_un.S_un_b.s_b1;
			sender->ip[1]	= from.sin_addr.S_un.S_un_b.s_b2;
			sender->ip[2]	= from.sin_addr.S_un.S_un_b.s_b3;
			sender->ip[3]	= from.sin_addr.S_un.S_un_b.s_b4;
			sender->port	= ntohs( from.sin_port );
		}
	}
	else
	{
		receivedBytes = recvfrom( m_socket, (char*)buffer, size, 0, NULL, NULL );
	}

	return ( receivedBytes <= 0 ? 0 : receivedBytes );
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

bool Connection::Start( Socket* psocket )
{
	if ( !psocket || !psocket->m_socket || m_state != STOPPED ) return false;

	m_socket = psocket;
	m_sntAck = 0;
	m_recAck = 0;
	m_sendTime = 0;
	m_timeout = 0;
	m_state = STARTED;

	return true;	
}

void Connection::Stop( void )
{
	if ( m_state == CONNECTED )
		Disconnect();
	m_state = STOPPED;

	net_connection_clear_queue( this );
}

bool Connection::Listen( void )
{
	switch ( m_state )
	{
	case STOPPED:
		return false;
	case CONNECTED:
		Disconnect();
	}

	m_sntAck = 0;
	m_recAck = 0;
	m_sendTime = 0;
	m_timeout = 0;
	m_state = LISTENING;

	net_connection_clear_queue( this );

	return true;
}

void Connection::Connect( const NetAddress& destination )
{
	switch ( m_state )
	{
	case STOPPED:
		return;

	case CONNECTED:
		if ( net_check_address( &m_destination, &destination ) )
			return;
		else
			Disconnect();
	}

	m_destination = destination;
	m_sntAck = 0;
	m_recAck = 0;
	m_sendTime = 999999;
	m_timeout = 0;
	m_state = CONNECTING;

	net_connection_clear_queue( this );

}

void Connection::Disconnect( void )
{
	switch ( m_state )
	{
	case STOPPED:
		return;

	case CONNECTED:
		{
			NetPacketHeader packet( s_netInternal->id, NPT_DISCONNECT, 0 );
			m_socket->Send( m_destination, &packet, sizeof(NetPacketHeader) );
		}
		break;
	}

	m_state = DISCONNECTED;

	net_connection_clear_queue( this );

}

SEGAN_INLINE void Connection::Update( struct NetMessage* buffer, const float elpsTime, const float delayTime, const float timeOut )
{

	switch ( m_state )
	{
	case STARTED:
	case STOPPED:
	case DISCONNECTED: return;

	case LISTENING:

		sx_assert( buffer );
		
		//	verify that received message is connection request
		if ( buffer->bytsRecved && buffer->packet.header.id == s_netInternal->id && buffer->packet.header.type == NPT_CONNECT )
		{
			buffer->bytsRecved = 0;	//	avoid processing message by other connections

			//  store client address
			m_destination = buffer->address;

			//  respond to client with welcome message
			NetPacket packet( s_netInternal->id, NPT_WELCOME, m_sntAck );
			m_socket->Send( m_destination, &packet, sizeof(NetPacketHeader) + net_copy_name( &packet, m_name ) );

			//  store client name
			m_name = (wchar*)buffer->packet.data;

			//	the connection is connected
			m_state = CONNECTED;
		}
		
		break;

	case CONNECTING:

		sx_assert( buffer );

		//	wait for receiving welcome message
		if ( buffer->bytsRecved && buffer->packet.header.id == s_netInternal->id && net_check_address( &buffer->address, &m_destination ) && buffer->packet.header.type == NPT_WELCOME )
		{
			buffer->bytsRecved = 0;	//	avoid processing message by other connections

			//  copy host name
			m_name = (wchar*)buffer->packet.data;

			//  client is connected to the destination
			m_recAck = buffer->packet.header.ack;
			m_sntAck = 0;
			m_timeout = 0;
			m_sendTime = 0;
			
			m_state = CONNECTED;

			NetPacketHeader packet( s_netInternal->id, NPT_ALIVE, m_sntAck );
			m_socket->Send( m_destination, &packet, sizeof(NetPacketHeader) );

			break;
		}

		//  check connection timeout
		m_timeout += elpsTime;
		if ( m_timeout > timeOut )
		{
			Disconnect();
			break;
		}

		//  send request data to connect
		m_sendTime += elpsTime;
		if ( m_sendTime > delayTime )
		{
			m_sendTime = 0;

			NetPacket packet( s_netInternal->id, NPT_CONNECT, 0 );
			m_socket->Send( m_destination, &packet, sizeof(NetPacketHeader) + net_copy_name( &packet, m_name ) );
		}
		break;

	case CONNECTED:

		sx_assert( buffer );

		//	message received
		if ( buffer->bytsRecved && buffer->packet.header.id == s_netInternal->id && net_check_address( &m_destination, &buffer->address ) )
		{
			m_timeout = 0;

			switch ( buffer->packet.header.type )
			{
			case NPT_DISCONNECT:
				{
					//  avoid process buffer by other connections
					buffer->bytsRecved = 0;
					Disconnect();
					break;
				}

			case NPT_ACK:	//	search the sent list for requested ack
				{
					int n = m_sentList.Count();
					for ( int i=0; i<n; i++ )
					{
						NetMessage* msg = m_sentList[i];
						if ( msg->packet.header.ack == buffer->packet.header.ack )
						{
							m_socket->Send( m_destination, &msg->packet, msg->size );
							break;
						}
					}

					buffer->bytsRecved = 0;		// notify that buffer handled
				}
				break;

			case NPT_USER:
				{
					if ( !buffer->packet.header.crit ) 
					{
						m_callBack( this, buffer->packet.data, buffer->bytsRecved );
					}
					else if ( m_recAck == buffer->packet.header.ack )
					{
						//	user callback function
						m_callBack( this, buffer->packet.data, buffer->bytsRecved );
						m_recAck++;

						//	flush stored messages in the list
						net_connection_flush_list( this );
					}
					else if ( m_recAck < buffer->packet.header.ack )
					{
						if ( ! net_connection_push_to_queue( buffer, this ) )
							return;
					}
				}
				//	break;	continue to process the messages

			case NPT_ALIVE:
				{
					if ( m_recAck == buffer->packet.header.ack )
					{
						net_connection_send_from_queue( this );
					}
					else if ( m_recAck < buffer->packet.header.ack )
					{
						//	request to send lost message
						NetPacketHeader packet( s_netInternal->id, NPT_ACK, m_recAck );
						m_socket->Send( m_destination, &packet, sizeof(NetPacketHeader) );
					}

					buffer->bytsRecved = 0;		// notify that buffer handled
				}
				break;

			case NPT_ZIP:
				{
					if ( !buffer->packet.header.crit ) 
					{
						net_unmerge_packet( buffer->packet.data, this );
					}
					else if ( m_recAck == buffer->packet.header.ack )
					{
						net_unmerge_packet( buffer->packet.data, this );
						m_recAck++;
					}
					else if ( m_recAck < buffer->packet.header.ack )
					{
						if ( ! net_connection_push_to_queue( buffer, this ) )
							return;
					}

					buffer->bytsRecved = 0;		// notify that buffer handled
				}
				break;

			}	//	switch ( buffer->packet.header.type )
		}
		else	//	if ( buffer->bytsRecved && buffer->id == s_netInternal->id && net_check_address( &m_destination, &buffer->address ) )
		{
			m_timeout += elpsTime;
			if ( m_timeout > timeOut )
			{
				Disconnect();
				break;
			}
		}

		//	keep connection alive
		m_sendTime += elpsTime;
		if ( m_sendTime > delayTime )
		{
			m_sendTime = 0;
			NetPacketHeader packet( s_netInternal->id, NPT_ALIVE, m_sntAck );
			m_socket->Send( m_destination, &packet, sizeof(NetPacketHeader) );
		}

		break;
	}
}

SEGAN_INLINE bool Connection::Send( const void* buffer, const int sizeinbyte, const bool critical /*= true */ )
{
	sx_callstack();
	sx_assert( buffer && sizeinbyte>0 && sizeinbyte<=NET_PACKET_SIZE );

	bool res = false;
	
	switch ( m_state )
	{
	case STOPPED:
	case LISTENING:
	case DISCONNECTED: res = false; break;
	default:
		{
			NetMessage* msg;
			uint newsize = 0;

			//	verify that messages are in the queue
			if ( m_sendQueue.Count() )
			{
				msg = m_sendQueue.Top();

				//	merge only messages with same critical state
				if ( msg->packet.header.crit == (byte)critical )
				{
					NetPacket packet( s_netInternal->id, NPT_USER, 0 );
					packet.header.crit = critical;
					memcpy( packet.data, buffer, sizeinbyte );
					newsize = net_merge_packet( &msg->packet, msg->size, &packet, sizeinbyte + sizeof(NetPacketHeader) );
					if ( newsize )
					{
						msg->size = newsize;
						res = true;
					}
				}
			}
			
			//	if message can't be merged just send it normally
			if ( !newsize && s_netInternal->msgPool.Pop( msg ) )
			{
				msg->packet.header.id = s_netInternal->id;
				msg->packet.header.type = NPT_USER;
				msg->packet.header.crit = critical;
				msg->size = sizeinbyte + sizeof(NetPacketHeader);
				memcpy( msg->packet.data, buffer, sizeinbyte );

				m_sendQueue.Push( msg );
				res = true;
			}
		}
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////
//	server implementation
//////////////////////////////////////////////////////////////////////////
Server::Server( void )
: m_state(STOPPED)
, m_callback(0)
, m_numSend(0)
, m_flagTime(0)
, m_clientPort(0)
{
}

Server::~Server( void )
{
	switch ( m_state )
	{
	case STOPPED:
		return;
	}

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
			con->Start( &m_socket );
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
		NetPacket packet( s_netInternal->id, NPT_SERVER_CLOSED, 0 );
		NetAddress broadcast; ZeroMemory( &broadcast.ip, sizeof(broadcast.ip) ); broadcast.port = m_clientPort;
		m_socket.Send( broadcast, &packet, sizeof(NetPacketHeader) + net_copy_name( &packet, m_name ) );
	}

	for ( int i=0; i<m_clients.Count(); i++ )
	{
		m_clients[i]->Disconnect();
		m_clients[i]->Stop();
		sx_delete( m_clients[i] );
	}
	m_clients.Clear();

	m_socket.Close();
}

void Server::Listen( void )
{
	if ( m_state == STOPPED ) return;

	for ( int i=0; i<m_clients.Count(); i++ )
		m_clients[i]->Listen();

	m_state = LISTENING;
}

void Server::Run( void )
{
	if ( m_state == STOPPED ) return;


	//	notify that this server has stopped client acceptance
	NetPacket packet( s_netInternal->id, NPT_SERVER_CLOSED, 0 );
	NetAddress broadcast; ZeroMemory( &broadcast.ip, sizeof(broadcast.ip) ); broadcast.port = m_clientPort;
	m_socket.Send( broadcast, &packet, sizeof(NetPacketHeader) + net_copy_name( &packet, m_name ) );

	for ( int i=0; i<m_clients.Count(); i++ )
	{
		if ( m_clients[i]->m_state == LISTENING )
			m_clients[i]->Stop();
	}

	m_state = RUNNING;
}

void Server::Update( const float elpsTime, const float delayTime, const float timeOut )
{
	switch ( m_state )
	{
	case STOPPED: return;

	case LISTENING:

		//	broad cast flag message to network to rise the flag up
		m_flagTime += elpsTime;
		if ( m_flagTime > NET_SERVER_FLAG_TIME )
		{
			m_flagTime = 0;

			NetPacket packet( s_netInternal->id, NPT_SERVER_OPEN, 0 );
			NetAddress broadcast; ZeroMemory( &broadcast.ip, sizeof(broadcast.ip) ); broadcast.port = m_clientPort;
			m_socket.Send( broadcast, &packet, sizeof(NetPacketHeader) + net_copy_name( &packet, m_name ) );

			for ( int i=0; i<m_clients.Count(); i++ )
			{
				if ( m_clients[i]->m_state != CONNECTED )
				{
					m_clients[i]->Listen();
					m_clients[i]->m_sntAck = m_numSend;
				}
			}
		}
		// break; connections should be update :)

	case RUNNING:
		{
			NetMessage buffer;
			buffer.bytsRecved = m_socket.Receive( &buffer, NET_PACKET_SIZE, &buffer.address );

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

bool Server::Send( const char* buffer, const int sizeinbyte, const bool critical /*= false */ )
{
	sx_assert( buffer );
	sx_assert( sizeinbyte > 0 );
	sx_assert( sizeinbyte <= NET_PACKET_SIZE );

	bool res = true;
	if ( m_state != STOPPED )
	{
		for ( int i=0; res && i<m_clients.Count(); i++ )
		{
			res = m_clients[i]->Send( buffer, sizeinbyte, critical );
		}
	}

	return res;

}



//////////////////////////////////////////////////////////////////////////
//	client implementation
//////////////////////////////////////////////////////////////////////////
Client::Client( void )
: m_callback(0)
{

}

Client::~Client( void )
{
	Stop();
}

bool Client::Start( const word port, CB_Client callback )
{
	if ( m_socket.m_socket ) return false;

	if ( m_socket.Open( port ) )
	{
		m_callback = callback;
		m_connection.m_userData = this;
		m_connection.m_callBack = callback_connection_client;
		return m_connection.Start( &m_socket );
	}
	
	return false;
}

void Client::Stop( void )
{
	m_connection.Stop();
	m_socket.Close();
}

void Client::Listen( void )
{
	m_connection.Listen();
}

void Client::Connect( NetAddress& destination )
{
	m_connection.m_name = m_name;
	m_connection.Connect( destination );
}

void Client::Disconnect( void )
{
	m_connection.Disconnect();
}

void Client::Update( const float elpsTime, const float delayTime, const float timeOut )
{
	NetMessage buffer;

	switch ( m_connection.m_state )
	{
	case STOPPED:	return;

	case LISTENING:

		//	listen to the port to find any server
		buffer.bytsRecved = m_socket.Receive( (char*)&buffer, NET_PACKET_SIZE, &buffer.address );

		if ( buffer.bytsRecved && buffer.packet.header.id == s_netInternal->id && ( buffer.packet.header.type == NPT_SERVER_OPEN || buffer.packet.header.type == NPT_SERVER_CLOSED ) ) 
		{
			ServerInfo serverInfo;
			memcpy( serverInfo.address.ip, buffer.address.ip, 4 );
			serverInfo.address.port = buffer.address.port;
			serverInfo.age = 3000;

			if ( buffer.packet.header.type == NPT_SERVER_OPEN )
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
			else	//	if ( buffer.type == NPT_SERVER_CLOSED )
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
				{
					m_servers.RemoveByIndex( i );
				}
			}
		}

		break;

	default:
		{
			buffer.bytsRecved = m_socket.Receive( (char*)&buffer, NET_PACKET_SIZE, &buffer.address );
			m_connection.Update( &buffer, elpsTime, delayTime, timeOut );
		}
		break;
	}
}

bool Client::Send( const char* buffer, const int sizeinbyte, const bool critical /*= false */ )
{
	return m_connection.Send( buffer, sizeinbyte, critical );
}


//////////////////////////////////////////////////////////////////////////
//	network functions
//////////////////////////////////////////////////////////////////////////
SEGAN_ENG_API bool sx_net_initialize( const dword netID )
{
	sx_callstack();

	if ( s_netInternal  )
	{
		g_logger->Log( L"Warning: calling sx_net_initialize() failed du to network system was initialized!" );
		return 0;
	}
	bool netInitialized = true;

	//  initialize windows socket
	WSADATA wsaData;
	if( ::WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) )
	{
		g_logger->Log( L"Error: Network initialization on Windows failed! error code : %s !" , net_error_string( WSAGetLastError() ) );
		netInitialized = false;
	}

	//  check initialized version
	bool incorrectVersion = LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2;
	if ( netInitialized && incorrectVersion )
	{
		g_logger->Log( L"Error: Network initialization on Windows failed! Invalid version detected!" );
		netInitialized = false;
	}

	// Get local host name
	char hostName[128] = {0};
	if( netInitialized && ::gethostname( hostName, sizeof(hostName) ) )
	{
		g_logger->Log( L"Error: function ::gethostname() failed with error code : %s !" , net_error_string( WSAGetLastError() ) );
		netInitialized = false;
	}

	// Get local IP address
	hostent* pHost = ::gethostbyname( hostName );
	if( netInitialized && !pHost )
	{
		g_logger->Log( L"Error: function ::gethostbyname() failed with error code : %s !" , net_error_string( WSAGetLastError() ) );
		netInitialized = false;
	}
	

	if ( netInitialized )
	{
		//  initialize internal net object
		s_netInternal = sx_new( NetInternal );
		sx_str_copy( s_netInternal->name, 32, hostName );
		s_netInternal->id = netID;
		s_netInternal->address.ip[0] = pbyte(pHost->h_addr_list[0])[0];
		s_netInternal->address.ip[1] = pbyte(pHost->h_addr_list[0])[1];
		s_netInternal->address.ip[2] = pbyte(pHost->h_addr_list[0])[2];
		s_netInternal->address.ip[3] = pbyte(pHost->h_addr_list[0])[3];
		s_netInternal->address.port  = 0;

		for (int i=0; i<NET_MAX_NUM_BUFFER; i++)
			s_netInternal->msgPool.Push( (NetMessage*)mem_alloc( sizeof(NetMessage) ) );

		g_logger->Log(  L"Network system initialized successfully on Windows." );
		g_logger->Log_( L"    Name:		%s \r\n" , s_netInternal->name );
		g_logger->Log_( L"    IP:			%d.%d.%d.%d \r\n\r\n" , 
			s_netInternal->address.ip[0], s_netInternal->address.ip[1], s_netInternal->address.ip[2], s_netInternal->address.ip[3] );
	}
	else
	{
		g_logger->Log( L"The network system is now disabled." );
		WSACleanup();
	}

	return netInitialized;
}

void sx_net_finalize( void )
{
	sx_callstack();

	if ( s_netInternal )
	{
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
	else
	{
		g_logger->Log( L"Warning: calling sx_net_finalize() failed du to network system was finalized or was not initialized!" );
	}
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


#endif
