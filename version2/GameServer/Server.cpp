#include "Socket.h"
#include "Connection.h"
#include "Server.h"



Server::Server(void) 
{
	sx_mem_set( &m_stats, 0, sizeof(m_stats) );
	m_stats.min_cycle_per_sec = 9999999;
}

Server::~Server(void) { }

void Server::Initialize( const NetConfig* config /*= null*/, CBServer add /*= null*/, CBServer remove /*= null*/ )
{
	sx_callstack();
	sx_print( L"Info: Initializing server:\n\n");

	if ( config )
		m_config = *config;

	sx_print( L"Info: Opening receiver port:\n" );
	m_recvSocket = sx_new Socket;
	m_recvSocket->Open( m_config.recv_port );

	sx_print( L"\nInfo: Opening sender port:\n" );
	m_sendSocket = sx_new Socket;
	m_sendSocket->Open( m_config.send_port );

	m_stats.start_time = sx_net_get_time();

	sx_print( L"\nInfo: Server initialized.\n\n" );

	m_connectionAdd = add;
	m_connectionRem = remove;
}

void Server::Finalize(void)
{
	sx_callstack();

	for ( int i = 0; i < m_connections.m_count; ++i )
	{
		Connection* con = m_connections[i];
		con->Close();
		sx_delete con;
	}
	m_connections.clear();
	m_connectionMap.clear();

	m_recvSocket->Close();
	m_sendSocket->Close();
}

void Server::Update( const double elpsTime )
{
	// peek packets on the port
	PeekReceivedMessages();

	// update connections
	for ( int i = 0; i < m_connections.m_count; ++i )
		m_connections[i]->Update( m_sendSocket, elpsTime );

	// remove lost connections from the list
	for (int i = 0; i < m_connections.m_count; ++i)
	{
		Connection* con = m_connections[i];
		if ( con->m_connected == false )
		{
			if (m_connectionRem)
				if (m_connectionRem(con) == false)
					continue;

			sx_print( L"Info: Connection closed from %d.%d.%d.%d:%d\n", con->m_destination.ip_bytes[0], con->m_destination.ip_bytes[1], con->m_destination.ip_bytes[2], con->m_destination.ip_bytes[3], con->m_destination.port );
			RemoveConnection( i-- );
			sx_print( L"Info: Total connections: %d\n", m_connections.m_count );
		}
	}

	UpdateStatistics();
}

Connection* Server::AddConnection( const NetAddress& address )
{
	//	create a new connection
	Connection* res = sx_new Connection;

	//	set properties
	res->SetSpeed( m_config.packs_per_sec );
	res->SetRetryTime( m_config.retry_time );
	res->SetTimeOut( m_config.retry_timeout );
	res->Open( address );

	//	add connection to the connection list
	m_connections.push_back( res );
	m_connectionMap.insert( address.ip, res );

	// update stats
	m_stats.total_connections++;
	m_stats.curr_connections = m_connections.m_count;
	if ( m_stats.curr_connections > m_stats.max_connections )
		m_stats.max_connections = m_stats.curr_connections;

	return res;
}

void Server::RemoveConnection( const int index )
{
	Connection* con = m_connections[index];

	//	remove connection from the connection list
	m_connections.remove_index( index, false );
	m_connectionMap.remove( con->m_destination.ip );
	
	m_stats.curr_connections = m_connections.m_count;

	sx_delete con;
}

void Server::PeekReceivedMessages(void)
{
	sx_callstack();
	NetAddress address;
	char buffer[SX_NET_BUFF_SIZE] = {0};
	
	m_stats.socket_queued_packs = 0;
	while ( true )
	{
		//	peek the packet
		int receivedBytes = m_recvSocket->Receive( buffer, SX_NET_BUFF_SIZE, &address );
		if ( receivedBytes < 1 )
			break;
		else
			sx_print_a("\nReceived: %s", buffer);

		//	check if we have a connection for the received address
		Connection* con = null;
		if ( m_connectionMap.find( address.ip, con ) )
		{
			//	verify that the packet is valid
			if ( sx_net_verify_packet( buffer, receivedBytes, con->m_RecvNumber ) )
			{
				//	update stats
				m_stats.total_recv_packs++;
				m_stats.socket_queued_packs++;
				m_stats.total_recv_bytes += receivedBytes;

				//	append the packet to the connection buffer
				con->AppendReceivedMessage( m_sendSocket, buffer, receivedBytes );
			}
			else m_stats.total_invalid_packs++;
		}
		else // it a new connection
		{
			//	verify that the packet is valid
			if ( sx_net_verify_packet( buffer, receivedBytes, 0 ) )
			{
				//	update stats
				m_stats.total_recv_packs++;
				m_stats.socket_queued_packs++;
				m_stats.total_recv_bytes += receivedBytes;

				sx_print( L"Info: New connection from %d.%d.%d.%d:%d\n", address.ip_bytes[0], address.ip_bytes[1], address.ip_bytes[2], address.ip_bytes[3], address.port );
				
				//	create new connection
				con = AddConnection( address );

				//	append the packet to the connection buffer
				con->AppendReceivedMessage( m_sendSocket, buffer, receivedBytes );

				//	call callback
				if (m_connectionAdd)
					m_connectionAdd(con);

				sx_print(L"Info: Total connections: %d\n", m_connections.m_count);
			}
			else m_stats.total_invalid_packs++;
		}
	}
}

void Server::UpdateStatistics( void )
{
	m_stats.helper_cps++;
	m_stats.curr_time = sx_net_get_time();

	//	verify that one second has been passed
	if (m_stats.curr_time - m_stats.helper_timer < 1000) return;

	//	update cycle stats
	m_stats.cycle_per_sec = m_stats.helper_cps;
	if (m_stats.curr_time - m_stats.start_time > 5000)
	{
		if (m_stats.cycle_per_sec > m_stats.max_cycle_per_sec)
			m_stats.max_cycle_per_sec = m_stats.cycle_per_sec;

		if (m_stats.cycle_per_sec < m_stats.min_cycle_per_sec)
			m_stats.min_cycle_per_sec = m_stats.cycle_per_sec;
	}

	// update packet stats
	m_stats.recv_packs_per_sec = m_stats.total_recv_packs - m_stats.helper_rpps;
	m_stats.helper_rpps = m_stats.total_recv_packs;

	// update bytes stats
	m_stats.recv_bytes_per_sec = m_stats.total_recv_bytes - m_stats.helper_rbps;
	m_stats.helper_rbps = m_stats.total_recv_bytes;

	// update invalid packets stats
	m_stats.invalid_packs_per_sec = m_stats.total_invalid_packs - m_stats.helper_ipps;
	m_stats.helper_ipps = m_stats.total_invalid_packs;

	// prepare for next loop
	m_stats.helper_cps = 0;
	m_stats.helper_timer = m_stats.curr_time;
}

