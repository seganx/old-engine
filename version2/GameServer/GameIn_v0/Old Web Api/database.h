/********************************************************************
	created:	2016/4/15
	filename: 	database.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple database structure
*********************************************************************/
#ifndef DEFINED_database
#define DEFINED_database

#include "imports.h"

//////////////////////////////////////////////////////////////////////////
//	database configuration
//////////////////////////////////////////////////////////////////////////
struct DatabaseConfig
{
	uint		port;
	char		host[128];				//	host of SQL server
	char		name[128];				//	name of database
	char		user[128];				//	user name of database
	char		pass[128];				//	user pass of database
	uint		threadTimeout;			//	timeout for unused thread in milliseconds
};

//////////////////////////////////////////////////////////////////////////
//	database class
//	a simple database class to connect to MySql database
//////////////////////////////////////////////////////////////////////////
class Database
{
public:
	Database( void );
	~Database( void );

	//	initialize the SQL connection
	bool initalize( const DatabaseConfig* config );

	//	finalize the SQL connection
	void Finalize(void);

	//  send command to mySql server and fill out destination buffer with retrieved data. return data size in byte
	uint Command(char* dest, const uint destsize, const char* command);

	//  send command to mySql server and fill out destination buffer with retrieved data. return data size in byte
	uint FormatCommand( char* dest, const uint destsize, const char* command, ... );

public:
	struct st_mysql* m_mysql;
};


#endif // DEFINED_database
