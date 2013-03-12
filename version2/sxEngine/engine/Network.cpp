#include "Network.h"
#include "../system/System.h"


Network::Network( void )
: m_delayTime(60)
, m_timeOut(10000)
{
	m_client = sx_new( Client );
	m_server = sx_new( Server );
}

Network::~Network( void )
{
	sx_delete_and_null( m_client );
	sx_delete_and_null( m_server );
}

void Network::Initialize( void )
{

}

void Network::Finalize( void )
{

}

void Network::Update( float elpsTime )
{
	m_client->Update( elpsTime, m_delayTime, m_timeOut );
}

bool Network::Send( const char* buffer, const int sizeinbyte, const bool critical /*= false */ )
{
	return m_client->Send( buffer, sizeinbyte, critical );
}

