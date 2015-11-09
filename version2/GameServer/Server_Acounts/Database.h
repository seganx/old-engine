/********************************************************************
	created:	2013/06/15
	filename: 	Database.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple database structure
*********************************************************************/
#ifndef GUARD_Database_HEADER_FILE
#define GUARD_Database_HEADER_FILE

#include "../Net.h"
#include "Database_Result.h"

//////////////////////////////////////////////////////////////////////////
//	database configuration
//////////////////////////////////////////////////////////////////////////
struct DatabaseConfig
{
	uint		port;
	char*		host;				//	host of SQL server
	char*		name;				//	name of database
	char*		user;				//	user name of database
	char*		pass;				//	user pass of database
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

	//	initialize the database
	bool initalize( const DatabaseConfig* config );

	//  send command to mySql server and fill out destination buffer with retrieved data. return number of rows
	uint Command( DatabaseResult& dest, const char* command, ... );

public:
	struct st_mysql* m_mysql;
};


#endif	//	GUARD_Database_HEADER_FILE
