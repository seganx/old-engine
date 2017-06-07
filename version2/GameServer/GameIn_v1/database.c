#include <my_global.h>
#include <mysql.h>
#pragma comment (lib, "libmysql.lib")

#include "database.h"

bool sx_database_initalize(struct sx_database * database, const struct sx_database_config * config)
{
    sx_trace();
    
	//	check to see if library is compiled as thread-safe
#if 0 && _DEBUG
	if (mysql_thread_safe() == 0)
	{
		sx_print("ERROR: MySQL library is NOT compiled as thread-safe!");
		sx_assert(false);
		sx_return(false);
	}
#endif

	database->mysql = mysql_init(NULL);
	if (mysql_real_connect(database->mysql, config->host, config->user, config->pass, config->name, config->port, NULL, CLIENT_MULTI_STATEMENTS) == null)
	{
		sx_print("Error: %s", mysql_error(database->mysql));
		mysql_close(database->mysql);
		mysql_thread_end();
		database->mysql = null;
		sx_return(false);
	}

	sx_return(true);
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

    sx_return();
}

uint sx_database_query(struct sx_database * database, struct sx_string * query, struct sx_string * result)
{
    sx_trace();

	if (database->mysql == null) return 0;

	if (mysql_query(database->mysql, query->text))
	{
		sx_print("Error: %s", mysql_error(database->mysql));
		sx_return(0);
	}

	//	process each statement result
    uint res = 0;
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
                    if (res > 0)
                        sx_string_append(result, database_sepchar);

                    res++;
                    sx_string_append(result, row[i]);
				}
			}
			else break;
		}
		mysql_free_result(myres);

	} while (mysql_next_result(database->mysql) == 0);

	sx_return(res);
}

bool sx_database_verify_data(const char* data)
{
    return sx_str_str(data, "<") == null && sx_str_str(data, ">") == null && sx_str_str(data, ";") == null;
}
