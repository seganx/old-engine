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
#define sx_database_sepchar     ">"

typedef void(*sx_database_func)(void * userdata, const int index, const char* data);

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
uint sx_database_query_cb( struct sx_database * database, sx_database_func callback, void* userdata, const char* query );

//  send query command to mySql server and return number of received fields
uint sx_database_query_cb_fmt( struct sx_database * database, sx_database_func callback, void* userdata, const char* strformat, ... );

//  send query command to mySql server and return number of received fields
uint sx_database_query_str( struct sx_database * database, struct sx_string * result, const char* strformat, ... );

//  send query command to mySql server and return number of received fields
uint sx_database_query( struct sx_database * database, char* dest, const uint destsize, const char* strformat, ... );

//  verify that user content is clear from injection strings
bool sx_database_invalid_data( const char* data );

//  verify that user content is clear from injection strings
bool sx_database_valid_data( const char* data );

#if __cplusplus
}
#endif // __cplusplus

#endif // DEFINED_database
