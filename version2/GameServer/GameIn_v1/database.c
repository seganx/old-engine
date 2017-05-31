#include <my_global.h>
#include <mysql.h>
#pragma comment (lib, "libmysql.lib")

#include "database.h"

bool sx_database_initalize(struct sx_database * database, const struct sx_database_config * config)
{
    sx_trace();

	//	check to see if library is compiled as thread-safe
#if _DEBUG
	if (mysql_thread_safe() == 0)
	{
		sx_print("ERROR: MySQL library is NOT compiled as thread-safe!");
		sx_assert(false);
		return false;
	}
#endif

	database->mysql = mysql_init(NULL);
	if (mysql_real_connect(database->mysql, config->host, config->user, config->pass, config->name, config->port, NULL, CLIENT_MULTI_STATEMENTS) == null)
	{
		sx_print("Error: %s", mysql_error(database->mysql));
		mysql_close(database->mysql);
		mysql_thread_end();
		database->mysql = null;
		return false;
	}
	return true;
}

void sx_database_finalize(struct sx_database * database)
{
    sx_trace();

    if (database->mysql)
	{
		mysql_close(database->mysql);
		mysql_thread_end();
	}
	database->mysql = null;
}

uint sx_database_command(struct sx_database * database, struct sx_string * query, struct sx_string * result)
{
    sx_trace();

	if (database->mysql == null) return 0;

	if (mysql_query(database->mysql, query->text))
	{
		sx_print("Error: %s", mysql_error(database->mysql));
		return 0;
	}

	//	process each statement result
	do
	{
		MYSQL_RES *myres = mysql_store_result(database->mysql);
		if (myres == null) continue;

		uint fieldc = mysql_num_fields(myres);
		while (true)
		{
			MYSQL_ROW row = mysql_fetch_row(myres);
			if (row)
			{
				for (uint i = 0; i < fieldc; ++i)
                {
                    sx_string_append(result, row[i]);
                    sx_string_append(result, database_sepchar);
				}
			}
			else break;
		}
		mysql_free_result(myres);

	} while (mysql_next_result(database->mysql) == 0);

	return result->len;
}