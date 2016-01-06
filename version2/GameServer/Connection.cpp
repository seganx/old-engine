#include "Socket.h"
#include "Connection.h"




//////////////////////////////////////////////////////////////////////////
//	HELPER FUNCTION
//////////////////////////////////////////////////////////////////////////
bool connection_socket_send(Socket* socket, const NetPacket* np, const NetAddress& address)
{
	sx_print(L"Info: Send [NO: %d, OP: %d, IP:%d.%d.%d.%d:%d]", np->header->number, np->header->option, address.ip_bytes[0], address.ip_bytes[1], address.ip_bytes[2], address.ip_bytes[3], address.port );
	return socket->Send( address, np->data, np->size );
}

bool connection_socket_send(Socket* socket, const NetHeader* nh, const NetAddress& address)
{
	sx_print( L"Info: Send header [NO: %d OP: %d CH: %d, IP:%d.%d.%d.%d:%d]", nh->number, nh->option, nh->checksum, address.ip_bytes[0], address.ip_bytes[1], address.ip_bytes[2], address.ip_bytes[3], address.port );
	return socket->Send( address, nh, sizeof(NetHeader) );
}

Connection::Connection( void )
	: m_connected(true)
	, m_sendingPacks(4)
	, m_receivedPacks(8)
	, m_sentNumber(1)
	, m_RecvNumber(0)
	, m_confirmNumber(0)
	, m_confirmChecksum(0)
	, m_delayTime(0)
	, m_delayTimeMax(0.1f)
	, m_retryTime(0)
	, m_retryTimeMax(1)
	, m_outTime(0)
	, m_outTimeMax(60)
{

}

Connection::~Connection( void )
{
	Close();	//	just for sure
}

void Connection::Open( const NetAddress& destination )
{
	m_destination = destination;
}

void Connection::Close( void )
{
	sx_callstack();

	//	clear sending list
	for ( int i = 0; i < m_sendingPacks.m_count; ++i )
		sx_mem_free( m_sendingPacks[i] );
	m_sendingPacks.clear();

	sx_callstack();

	//	clear received list
	for ( int i = 0; i < m_receivedPacks.m_count; ++i )
		sx_mem_free( m_receivedPacks[i] );
	m_receivedPacks.clear();

	//	clear address
	m_destination.ip = 0;
	m_destination.port = 0;
}

void Connection::SetSpeed( const int packPerSecond )
{
	if ( packPerSecond )
		m_delayTimeMax = 1.0f / packPerSecond;
	else
		m_delayTimeMax = -1;
}

void Connection::SetTimeOut( const double timeOut )
{
	m_outTimeMax = timeOut;
}

void Connection::SetRetryTime( const double retryTime )
{
	m_retryTimeMax = retryTime;
}

bool Connection::Send( const void* buffer, const int size, const word option )
{
	sx_callstack();

	if ( (size + sizeof(NetHeader)) > SX_NET_BUFF_SIZE )
	{
		sx_print( L"Error: Buffer size for sending with connection is out of range!" );
		return false;
	}

	NetPacket* cp = (NetPacket*)sx_mem_alloc( sizeof(NetPacket) );
#if _DEBUG
	sx_mem_set( cp, 0, sizeof(NetPacket) );
#endif
	cp->size = size + sizeof(NetHeader);
	cp->header = (NetHeader*)cp->data;
	sx_mem_copy(cp->data + sizeof(NetHeader), buffer, size);

	cp->header->netId = SX_NET_ID;
	cp->header->option = option;
	cp->header->number = m_sentNumber > 0 ? m_sentNumber++ : (++m_sentNumber)++;
	cp->header->checksum = sx_net_compute_checksum(buffer, size);

	m_sendingPacks.push_back( cp );

	return true;
}

uint Connection::Received( void* destBuffer, const int destSize )
{
	sx_callstack();

	if ( m_receivedPacks.m_count )
	{
		NetPacket* np = m_receivedPacks[0];

		//	copy packet data to the destination buffer
		int size = sx_min_i( np->size - sizeof(NetHeader), destSize );
		sx_mem_copy( destBuffer, np->data + sizeof(NetHeader), size );

		//	remove from the list
		m_receivedPacks.remove_index(0);

		// delete packet
		sx_mem_free(np);

		return size;
	}
	else return 0;
}

void Connection::Update( class Socket* socket, const double elpsTime )
{
	sx_callstack();

	//	check connection time out to close the connection
	m_outTime += elpsTime;
	if (m_outTime > m_outTimeMax)
	{
		// the connection is dropped
		m_outTime = 0;
		m_connected = false;
		sx_print(L"Warning: Connection time out !");
	}


	if ( m_confirmNumber )	//	we are waiting for received confirmation from the other side
	{
		//	check retry time out and resend the message
		m_retryTime += elpsTime;
		if (m_retryTime > m_retryTimeMax)
		{
			m_retryTime = 0;
			if ( m_sendingPacks.m_count )
			{
				connection_socket_send(socket, m_sendingPacks[0], m_destination);
			}
			else
			{
				m_confirmNumber = 0;  // confirmed
				m_retryTime = 0;
				m_outTime = 0;
			}
		}
		return;
	}


	//	handle delay time
	if (m_delayTimeMax > 0 && m_delayTime > 0)
	{
		m_delayTime -= elpsTime;
		if (m_delayTime > 0) return;
	}


	// is there any message to send ?
	if ( m_sendingPacks.m_count && !m_confirmNumber )
	{
		NetPacket* np = m_sendingPacks[0];
		connection_socket_send( socket, np, m_destination );

		//	verify id the message has safe send option
		if (np->header->option & SX_NET_OPTN_SAFESEND)
		{
			m_confirmNumber = np->header->number;
			m_confirmChecksum = np->header->checksum;
		}
		else
		{
			m_sendingPacks.remove_index(0);
			sx_mem_free( np );
		}

		m_delayTime = m_delayTimeMax;
	}
}

void Connection::AppendReceivedMessage( class Socket* socket, const void* buffer, const int size )
{
	sx_callstack();
	
	m_outTime = 0;
	m_connected = true;

	NetHeader* ch = (NetHeader*)buffer;

	//	we are waiting for received confirmation from the other side
	if ( ch->option & SX_NET_OPTN_DELIVERED )
	{
		if ( ch->number == m_confirmNumber && ch->checksum == m_confirmChecksum )
		{
			sx_print(L"Info: Message number %d has been confirmed from other side!", m_confirmNumber);

			m_confirmNumber = 0;  // confirmed
			m_retryTime = 0;

			// remove the message from the sending list
			sx_mem_free( m_sendingPacks[0] );
			m_sendingPacks.remove_index( 0 );
		}
		else
		{
			// we should handle this situation.
		}
		return;
	}
	

	//	replay to sender that safe message has been received
	if (ch->option & SX_NET_OPTN_SAFESEND)
	{
		NetHeader tmp = *ch;
		tmp.option = SX_NET_OPTN_DELIVERED;
		connection_socket_send(socket, &tmp, m_destination);
	}

	//	create message container and append it to the list
	if (ch->number != m_RecvNumber)
	{
		m_RecvNumber = ch->number;

		NetPacket* np = (NetPacket*)sx_mem_alloc(sizeof(NetPacket));
		np->size = size;
		np->header = (NetHeader*)np->data;
		sx_mem_copy(np->data, buffer, size);

		//	add it to the received list
		m_receivedPacks.push_back(np);
	} 

	{
		char tmp[SX_NET_BUFF_SIZE] = {0};
		int copysize = sx_min_i(size, SX_NET_BUFF_SIZE-1);
		sx_mem_copy(tmp, (byte*)buffer + sizeof(NetHeader), copysize);
		sx_print_a("Info: Received [NO: %d OP: %d CH: %d, IP:%d.%d.%d.%d:%d]: %s", ch->number, ch->option, ch->checksum, m_destination.ip_bytes[0], m_destination.ip_bytes[1], m_destination.ip_bytes[2], m_destination.ip_bytes[3], m_destination.port, tmp);
	}
}

