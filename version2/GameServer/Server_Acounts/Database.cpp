#include <memory>
#include <my_global.h>
#include <mysql.h>
#include "database.h"

#pragma comment( lib, "libmysql.lib" )



//////////////////////////////////////////////////////////////////////////
//	DATA BASE
//////////////////////////////////////////////////////////////////////////

Database::Database(void) : m_mysql(null)
{

}

Database::~Database(void)
{
	Finalize();
}

bool Database::initalize(const DatabaseConfig* config)
{
	sx_callstack();

	m_mysql = mysql_init(NULL);

	if (mysql_real_connect(m_mysql, config->host, config->user, config->pass, config->name, config->port, NULL, 0) == null)
	{
		sx_print_a("Error: %s", mysql_error(m_mysql));
		mysql_close(m_mysql);
		m_mysql = null;
		return false;
	}
	return true;
}

void Database::Finalize(void)
{
	if (m_mysql)
		mysql_close(m_mysql);
	m_mysql = null;
}

uint Database::Command(char* dest, const uint destsize, const char* command)
{
	sx_callstack();
	if (m_mysql == null) return 0;

	if (mysql_query(m_mysql, command))
	{
		sx_print_a("Error: %s", mysql_error(m_mysql));
		return 0;
	}

	MYSQL_RES *result = mysql_store_result(m_mysql);
	if (!result) return 0;

	uint fieldc = mysql_num_fields(result);

	uint res = 0;
	while (true)
	{
		MYSQL_ROW row = mysql_fetch_row(result);
		if (row)
		{
			for ( uint i = 0; i < fieldc; ++i )
			{
				uint size = sx_raw_write_text( dest, destsize, row[i] );
				res += size;
			}
		}
		else break;
	}
	mysql_free_result(result);

	return res;
}

uint Database::FormatCommand(char* dest, const uint destsize, const char* command, ...)
{
	sx_callstack();
	if (m_mysql == null) return 0;

	va_list argList;
	va_start(argList, command);

	sint strLen = _vscprintf(command, argList);
	if (strLen > 2047) return false;

	char sqlcommand[2048] = { 0 };
	vsprintf_s(sqlcommand, strLen + 1, command, argList);

	return Command(dest, destsize, sqlcommand);
}

