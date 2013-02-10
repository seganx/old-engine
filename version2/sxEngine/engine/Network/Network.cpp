#include "Network.h"


static sint s_network_count		= 0;		//  number of network in the system
static bool s_netInitialized	= false;


//////////////////////////////////////////////////////////////////////////
//	Network
//////////////////////////////////////////////////////////////////////////
Network::Network( void ): m_server(0), m_client(0)
{
	ZeroMemory( &m_curInfo, sizeof(m_curInfo) );
}

Network::~Network( void )
{
	SEGAN_DELETE_AND_NULL( m_server );
	SEGAN_DELETE_AND_NULL( m_client );
}

void Network::Initialize( const word defaultPort )
{
	if ( !s_network_count )
	{
		s_netInitialized = ( sx_net_initialize() == 0 );
	}
	s_network_count++;

	//! if network initialized successfully then we can create instance of server/client
	if ( s_netInitialized )
	{
		memcpy( &m_curInfo, &s_currentNetInfo, sizeof(NetInfo) );
		m_curInfo.port = defaultPort;

		m_server = SEGAN_NEW( Server_win32 );
		m_client = SEGAN_NEW( Client_win32 );

	}
}

void Network::Finalize( void )
{
	SEGAN_DELETE_AND_NULL( m_client );
	SEGAN_DELETE_AND_NULL( m_server );

	if ( !s_network_count )
		g_logger->Log( L"Warning: Network::Finalize calling unexpected ! \r\n" );

	s_network_count--;
	if ( !s_network_count )
	{
		sx_net_finalize();
	}
}

void Network::SearchServers( NetInfo* netInfoArray, const sint netInfoCount )
{
	if ( !s_netInitialized )
	{
		g_logger->Log( L"Error: function Network::SearchServers failed du to network system is not initialized !" );
		return;
	}

	// 	//  TODO : find a better solution
	// 	Client_win32 client;
	// 	NetInfo netInfo;
	// 	memcpy( &netInfo, &m_curInfo, sizeof(NetInfo) );
	// 	for (sint i=0; i<256; i++)
	// 	{
	// 		if ( i == m_curInfo.ip[3] ) continue;	//  ignore this machine
	// 		
	// 		netInfo.ip[3] = i;
	// 		
	// 		//  try to connect servers with new IP
	// 		client.Initialize( m_curInfo, netInfo, NULL );
	// 
	// 		//  if connection success then
	// 		//		try to get name
	// 		//		add to the array of clients
	// 
	// 		client.Finalize();
	// 	}

	//	what ? :D
}


