#include <memory>
#include <my_global.h>
#include <mysql.h>
#include "database.h"

#pragma comment( lib, "libmysql.lib" )



//////////////////////////////////////////////////////////////////////////
//	DATA BASE
//////////////////////////////////////////////////////////////////////////

Database::Database( void ): m_mysql(null)
{
	
}

Database::~Database( void )
{
	if ( m_mysql )
		mysql_close( m_mysql );
}

bool Database::initalize( const DatabaseConfig* config )
{
	m_mysql = mysql_init(NULL);

	if ( mysql_real_connect( m_mysql, config->host, config->user, config->pass,	config->name, config->port, NULL, 0 ) == null )
	{
		sx_print( L"%s\n", mysql_error( m_mysql ) );
		mysql_close( m_mysql );
		return false;
	}
	return true;
}

uint Database::Command( DatabaseResult& dest, const char* command, ... )
{
	sx_callstack();

	// send query to SQL
	{
		va_list argList;
		va_start(argList, command);

		sint strLen = _vscprintf( command, argList );
		if ( strLen > SX_NET_BUFF_SIZE - 2 ) return false;

		char sqlcommand[SX_NET_BUFF_SIZE] = {0};
		strLen = vsprintf_s( sqlcommand, strLen + 1, command, argList );
		va_end(argList);
		
		if ( mysql_query( m_mysql, sqlcommand ) )
		{
			sx_print( L"%s\n", sx_utf8_to_str( mysql_error( m_mysql ) ) );
			return false;
		}
	}

	MYSQL_RES *result = mysql_store_result( m_mysql );
	if ( !result ) return 0;

	uint c = mysql_field_count( m_mysql );

	uint i = 0;
	for ( ; true; ++i )
	{ 
		MYSQL_ROW row = mysql_fetch_row( result );
		if ( row )
		{
			for ( uint j=0; j<c; ++j )
			{
				dest.add( *row );
				row++;
			}
		}
		else break;
	}

	mysql_free_result(result);
	return i;
}
