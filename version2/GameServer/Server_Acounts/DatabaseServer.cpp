#include "DatabaseServer.h"

#include "../Connection.h"
#include "../Server.h"
#include "database.h"
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

	return true;
}


//////////////////////////////////////////////////////////////////////////
//	SERVER DATABASE
//////////////////////////////////////////////////////////////////////////
DatabaseServer::DatabaseServer(void)
	: m_timer(0)
	, m_server(0)
	, m_threadMan(0)
	, m_netConfig(0)
	, m_dbConfig(0)
{

}

void DatabaseServer::Initialize(void)
{
	// create configuration file
	m_netConfig = sx_new NetConfig;
	m_dbConfig = sx_new DatabaseConfig;

	//	create objects
	m_timer = sx_new Timer;
	m_server = sx_new Server;
	m_threadMan = sx_new ThreadManager;

	//	initialize objects
	m_server->Initialize(m_netConfig);
}

void DatabaseServer::Finalize(void)
{
	sx_safe_delete_and_null(m_threadMan);
	sx_safe_delete_and_null(m_server);
	sx_safe_delete_and_null(m_timer);
	sx_safe_delete_and_null(m_dbConfig);
	sx_safe_delete_and_null(m_netConfig);
}

bool DatabaseServer::LoadConfig(const wchar* configFile)
{
	//	try to load configuration file
	if (LoadConfigs(m_netConfig, m_dbConfig, configFile))
	{
		m_threadMan->m_timeout = m_netConfig->retry_timeout * 0.0005;
		return true;
	}
	else
	{
		sx_print(L"ERROR: Can't read the configuration file %s", configFile);
		return false;
	}
}

void DatabaseServer::Update()
{
	m_timer->Update();

	m_server->Update(m_timer->m_elpsTime);

	for (int i = 0; i < m_server->m_connections.m_count; ++i)
	{
		char buffer[SX_NET_BUFF_SIZE] = { 0 };
		Connection* con = m_server->m_connections[i];

		uint msize = con->Received(buffer, SX_NET_BUFF_SIZE);
		if (msize > 0)
			m_threadMan->AddTask(con->m_destination.ip, m_dbConfig, buffer, msize);
	}

	for (int i = 0; i < m_server->m_connections.m_count; ++i)
	{
		Connection* con = m_server->m_connections[i];
		m_threadMan->Update(con->m_destination.ip, con);
	}

	m_threadMan->CheckThreadsTime(m_timer->m_elpsTime);
}
