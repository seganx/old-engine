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

#include "GameIn.h"

//! every fields in result will be separated by this char
#define database_sepchar    ">"


//! database configuration
typedef struct sx_database_config
{
	uint		port;
	char		host[128];				//	host of SQL server
	char		name[64];				//	name of database
	char		user[64];				//	user name of database
	char		pass[64];				//	user pass of database
}
sx_database_config;

//	a simple database object to connect to MySql database
typedef struct sx_database
{
	struct st_mysql* mysql;
}
sx_database;

#if __cplusplus
extern "C" {
#endif // __cplusplus

//	initialize the SQL connection
bool sx_database_initalize( struct sx_database * database, const struct sx_database_config * config );

//	finalize the SQL connection
void sx_database_finalize( struct sx_database * database );

//  send query command to mySql server and return number of received fields
uint sx_database_query( struct sx_database * database, struct sx_string * query, struct sx_string * result );

//  verify that user content is clear from injection strings
bool sx_database_verify_data( const char* data );

#if __cplusplus
}
#endif // __cplusplus

#endif // DEFINED_database
