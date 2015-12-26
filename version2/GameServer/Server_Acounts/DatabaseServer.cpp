#include "DatabaseServer.h"

#include "../Connection.h"
#include "../Server.h"
#include "database.h"
#include "Database_Thread.h"
#include "Thread_Manager.h"



bool LoadConfigs(NetConfig* netConfig, DatabaseConfig* dbConfig, const wchar* filename)
{
	String strFile;
	if (sx_load_string(strFile, filename) == false) return false;

	netConfig->id = sx_str_get_value_uint(strFile, L"network.id", netConfig->id);
	netConfig->packs_per_sec = sx_str_get_value_uint(strFile, L"network.packs_per_sec", netConfig->packs_per_sec);
	netConfig->recv_port = sx_str_get_value_uint(strFile, L"network.recv_port", netConfig->recv_port);
	netConfig->retry_time = sx_str_get_value_uint(strFile, L"network.retry_time", netConfig->retry_time);
	netConfig->retry_timeout = sx_str_get_value_uint(strFile, L"network.retry_timeout", netConfig->retry_timeout);
	netConfig->send_port = sx_str_get_value_uint(strFile, L"network.send_port", netConfig->send_port);

	dbConfig->port = sx_str_get_value_uint(strFile, L"database.port", 0);
	if (!sx_str_get_value(dbConfig->host, 128, strFile, L"database.host")) return false;
	if (!sx_str_get_value(dbConfig->name, 128, strFile, L"database.name")) return false;
	if (!sx_str_get_value(dbConfig->user, 128, strFile, L"database.user")) return false;
	if (!sx_str_get_value(dbConfig->pass, 128, strFile, L"database.pass")) return false;

	dbConfig->threadTimeout = sx_str_get_value_uint(strFile, L"threads.timeout", 5000);

	return true;
}


//////////////////////////////////////////////////////////////////////////
//	SERVER DATABASE
//////////////////////////////////////////////////////////////////////////
DatabaseServer::DatabaseServer(void)
	: m_server(0)
	, m_threadMan(0)
	, m_netConfig(0)
	, m_dbConfig(0)
{
	if (g_timer == null)
		g_timer = sx_new Timer;

	// create configuration file
	m_netConfig = sx_new NetConfig;
	m_dbConfig = sx_new DatabaseConfig;
}

DatabaseServer::~DatabaseServer(void)
{
	sx_safe_delete_and_null( g_timer );
	sx_safe_delete_and_null(m_dbConfig);
	sx_safe_delete_and_null(m_netConfig);
}

void DatabaseServer::Initialize(void)
{
	//	create objects
	m_server = sx_new Server;
	m_threadMan = sx_new ThreadManager;

	//	initialize objects
	m_server->Initialize(m_netConfig);
}

void DatabaseServer::Finalize(void)
{
	m_server->Finalize();

	sx_safe_delete_and_null(m_threadMan);
	sx_safe_delete_and_null(m_server);
}

bool DatabaseServer::LoadConfig(const wchar* configFile)
{
	//	try to load configuration file
	if (LoadConfigs(m_netConfig, m_dbConfig, configFile) == false)
	{
		sx_print(L"ERROR: Can't read the configuration file %s", configFile);
		return false;
	}
	else return true;
}

void DatabaseServer::Update()
{
	char buffer[SX_NET_BUFF_SIZE];

	// update server
	m_server->Update();

	// peek received data from clients and send them to the thread manager
	for (int i = 0; i < m_server->m_connections.m_count; ++i)
	{
		Connection* con = m_server->m_connections[i];
		uint recsize = con->Received(buffer, SX_NET_BUFF_SIZE);
		if ( recsize > 0 )
			m_threadMan->AddTask( con->m_destination.ip, m_dbConfig, buffer, recsize );
	}

	// peek received data from database and send them through connection
	for (sint i = 0; i < m_threadMan->m_threads.m_count; ++i)
	{
		DatabaseThread* dbThread = m_threadMan->m_threads[i];
		int ressize = dbThread->PeekResult( buffer, SX_NET_BUFF_SIZE );
		if ( ressize > 0 )
		{
			Connection* con = null;
			if ( m_server->m_connectionMap.find( dbThread->m_id, con ) )
				con->Send( buffer, ressize, SX_NET_OPTN_SAFESEND );
		}
	}

	// update thread manager
	m_threadMan->Update();
}
