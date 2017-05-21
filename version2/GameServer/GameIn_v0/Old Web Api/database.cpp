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

	//	check to see if library is compiled as thread-safe
#if _DEBUG
	if (mysql_thread_safe() == 0)
	{
		sx_print_a("ERROR: MySQL library is NOT compiled as thread-safe!");
		sx_assert(false);
		return false;
	}
#endif

#if 0
	//  let MySQL to initialize it's parameters to use in thread
	if (mysql_thread_init() > 0)
	{
		sx_print_a("ERROR: MySQL library couldn't initialize thread!");
		return false;
	}
#endif

	m_mysql = mysql_init(NULL);
	if (mysql_real_connect(m_mysql, config->host, config->user, config->pass, config->name, config->port, NULL, CLIENT_MULTI_STATEMENTS) == null)
	{
		sx_print_a("Error: %s", mysql_error(m_mysql));
		mysql_close(m_mysql);
		mysql_thread_end();
		m_mysql = null;
		return false;
	}
	return true;
}

void Database::Finalize(void)
{
	if (m_mysql)
	{
		mysql_close(m_mysql);
		mysql_thread_end();
	}
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

	uint res = 0;
	//	process each statement result
	do
	{
		MYSQL_RES *result = mysql_store_result(m_mysql);
		if (result == null) continue;

		uint fieldc = mysql_num_fields(result);
		while (true)
		{
			MYSQL_ROW row = mysql_fetch_row(result);
			if (row)
			{
				for (uint i = 0; i < fieldc; ++i)
				{
					uint size = sx_raw_write_text(dest, destsize, row[i]);
					res += size;
				}
			}
			else break;
		}
		mysql_free_result(result);

	} while (mysql_next_result(m_mysql) == 0);

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

