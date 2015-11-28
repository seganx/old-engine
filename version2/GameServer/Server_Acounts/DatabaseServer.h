/********************************************************************
	created:	2015/11/22
	filename: 	DatabaseServer.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple server to handle database queries
*********************************************************************/
#ifndef DATABASE_SERVER_DEFINED
#define DATABASE_SERVER_DEFINED

#include "../Net.h"

class DatabaseServer
{
public:
	DatabaseServer( void );
	~DatabaseServer( void );

	//! initialize the server
	void Initialize( void );

	//! finalize the server
	void Finalize( void );

	//! return true if can load the configuration file
	bool LoadConfig( const wchar* configFile );

	//! update the server. this function should be in main loop
	void Update( void );

public:
	class Timer*			m_timer;
	class Server*			m_server;
	class ThreadManager*	m_threadMan;
	struct NetConfig*		m_netConfig;
	struct DatabaseConfig*	m_dbConfig;
};


#endif // DATABASE_SERVER_DEFINED