#include "Socket.h"
#include "Connection.h"

Connection::Connection( void ): m_sendingPacks(4), m_receivedPacks(8), m_delayTime(0), m_delayTimeMax(0.1f), m_sendingId(1), m_receivedId(0), m_confirmId(0) {}
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

	sx_callstack();

	//	clear received list
	for ( int i = 0; i < m_receivedPacks.m_count; ++i )
		sx_mem_free( m_receivedPacks[i] );

	//	clear address
	m_destination.ip = 0;
	m_destination.port = 0;
}

void Connection::SetSpeed( const int packPerSecond )
{
	m_delayTimeMax = 1.0f / packPerSecond;
}

bool Connection::Send( const void* buffer, const int size, const byte option )
{
	sx_callstack();

	if ( (size + sizeof(ConHead)) > SX_NET_CON_BUFF_SIZE )
	{
		sx_print( L"Error: Buffer size for sending with connection is out of range!\n" );
		return false;
	}

	ConPack* cp = (ConPack*)sx_mem_alloc( sizeof(ConPack) );
	cp->size = size + sizeof(ConHead);
	cp->header = (ConHead*)cp->data;
	sx_mem_copy(cp->data + sizeof(ConHead), buffer, size);

	cp->header->netId = SX_NET_ID;
	cp->header->option = option;
	cp->header->number = m_sendingId > 0 ? m_sendingId++ : (++m_sendingId)++;
	cp->header->checksum = sx_net_compute_checksum(buffer, size);

	m_sendingPacks.push_back( cp );

	return true;
}

uint Connection::Received( void* destBuffer, const int destSize )
{
	return 0;
}

void Connection::Update( class Socket* socket, const float elpsTime )
{
	m_delayTime += elpsTime;
	if ( m_delayTime < m_delayTimeMax ) return;
	m_delayTime = 0;

	sx_callstack();

	if ( m_confirmId )	//	we are waiting for received confirmation from the other side
	{
		//	search throw received list to find confirmation message
		for ( int i = m_receivedPacks.m_count - 1; i >= 0; --i )
		{
			ConPack* cp = m_receivedPacks[i];
			if ( cp->header->option & SX_NET_OPTN_CONFIRMED && cp->header->number == m_confirmId && cp->header->checksum == m_confirmChecksum)
			{
				sx_print( L"Info: Message number %d has been confirmed from other side!\n", m_confirmId );

				m_confirmId = 0;  // confirmed

				// remove the message from the list
				m_receivedPacks.remove_index(i);

				break; // we are done. so just exit the loop
			}
		}

		//	verify if the confirmation is still exist
		if ( m_confirmId )
		{
			//	check sending time out and resend the message

			//	check connection time out to close the connection
		}
	}
	else // just continue sending data
	{
		if ( m_sendingPacks.m_count )	// is there any message to send ?
		{
			ConPack* cp = m_sendingPacks[0];

			//	send message throw socket
			socket->Send(m_destination, cp->data, cp->size);

			//	verify id the message has safe send option
			if (cp->header->option & SX_NET_OPTN_SAFESEND)
			{
				m_confirmId = cp->header->number;
				m_confirmChecksum = cp->header->checksum;
			}
			else
				m_sendingPacks.remove_index(0);
		}
	}
}

void Connection::AppendReceivedMessage( class Socket* socket, const void* buffer, const int size )
{
	sx_callstack();

	//	validate message size
	if ( sx_between_i( size, sizeof(ConHead), SX_NET_CON_BUFF_SIZE ) == false )
		return;

	//	verify the package is a valid
	ConHead* ch = (ConHead*)buffer;
	{
		//	validate net id
		if ( ch->netId != SX_NET_ID )
			return;

		//	validate if message is duplicated
		if ( ch->number == m_receivedId && sx_set_hasnt( ch->option, SX_NET_OPTN_SAFESEND ) )
			return;

		//	validate data checksum
		if ( size > sizeof(ConHead) )
		{
			const byte* buf = (const byte*)buffer + sizeof(ConHead);
			if (ch->checksum != sx_net_compute_checksum(buf, size - sizeof(ConHead)))
				return;
		}
	}

	//	handle safe messages
	if ( ch->option & SX_NET_OPTN_SAFESEND )
	{
		ConHead tmp = *ch;
		tmp.option = SX_NET_OPTN_CONFIRMED;
		socket->Send( m_destination, &tmp, sizeof(ConHead) );
	}

	//	create message container and append it to the list
	if ( ch->number != m_receivedId )
	{
		ConPack* cp = (ConPack*)sx_mem_alloc( sizeof(ConPack) );
		cp->size = size;
		cp->header = (ConHead*)cp->data;
		sx_mem_copy( cp->data, buffer, size );

		//	add it to the received list
		m_receivedPacks.push_back(cp);
	}
}

