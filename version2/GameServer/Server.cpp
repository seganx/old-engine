#include "Socket.h"
#include "Connection.h"
#include "Server.h"



Server::Server(void) { }

Server::~Server(void) { }

void Server::Initialize( const word port )
{
	sx_callstack();
	sx_print( L"Info: Initializing server on port %d\n", port );

	m_socket = sx_new Socket;
	m_socket->Open( port );

	sx_print( L"Info: Server initialized\n\n", port );
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
}

void Server::Update(const double elpsTime)
{
	// peek packages on the port
	PeekReceivedMessages();

	// update connections
	for ( int i = 0; i < m_connections.m_count; ++i )
		m_connections[i]->Update( m_socket, elpsTime );

	// remove lost connections from the list
	for (int i = 0; i < m_connections.m_count; ++i)
	{
		Connection* con = m_connections[i];
		if ( con->m_connected == false )
		{
			sx_print(L"Info: Connection closed from %d.%d.%d.%d:%d\n", con->m_destination.ip_bytes[0], con->m_destination.ip_bytes[1], con->m_destination.ip_bytes[2], con->m_destination.ip_bytes[3], con->m_destination.port );
			RemoveConnection( i-- );
			sx_print(L"Info: Total connections: %d\n", m_connections.m_count);
		}
	}
		
}

Connection* Server::AddConnection(const NetAddress& address)
{
	//	create a new connection
	Connection* res = sx_new Connection;

	//	set properties
	res->SetSpeed( 10 );
	res->SetTimeOut( 5 );
	res->Open( address );

	//	add connection to the connection list
	m_connections.push_back( res );
	m_connectionMap.insert( address.ip, res );

	return res;
}

void Server::RemoveConnection( const int index )
{
	Connection* con = m_connections[index];

	//	remove connection from the connection list
	m_connections.remove_index( index );
	m_connectionMap.remove( con->m_destination.ip );
	
	sx_delete con;
}

void Server::PeekReceivedMessages(void)
{
	sx_callstack();
	NetAddress address;
	char buffer[SX_NET_BUFF_SIZE];
	
	int queuePack = 0;
	int receivedBytes = 0;
	do 
	{
		//	peek the package
		receivedBytes = m_socket->Receive( buffer, SX_NET_BUFF_SIZE, &address );
		if ( receivedBytes < 1 ) continue;
		queuePack++;

		//	check if we have a connection for the received address
		Connection* con = null;
		if ( m_connectionMap.find( address.ip, con ) )
		{
			//	verify that the package is valid
			if ( sx_net_verify_package( buffer, receivedBytes, con->m_RecvNumber ) )
			{
				//	append the package to the connection buffer
				con->AppendReceivedMessage( m_socket, buffer, receivedBytes );
			}
		}
		else // it a new connection
		{
			//	verify that the package is valid
			if ( sx_net_verify_package( buffer, receivedBytes, 0 ) )
			{
				sx_print( L"Info: New connection from %d.%d.%d.%d:%d\n", address.ip_bytes[0], address.ip_bytes[1], address.ip_bytes[2], address.ip_bytes[3], address.port );
				
				//	create new connection
				con = AddConnection( address );

				//	append the package to the connection buffer
				con->AppendReceivedMessage( m_socket, buffer, receivedBytes );

				sx_print(L"Info: Total connections: %d\n", m_connections.m_count);
			}
		}		
	} 
	while ( receivedBytes > 0 );

	if ( queuePack > 1 )
		sx_print( L"messages queued : %d\n", queuePack );
}

