#include <my_global.h>
#include <mysql.h>
#pragma comment (lib, "libmysql.lib")

#include "database.h"
#include <malloc.h>

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

uint sx_database_query_cb(struct sx_database * database, sx_database_func callback, void* userdata, const char* query)
{
    sx_trace();

    if (database->mysql == null) return 0;

    if (mysql_query(database->mysql, query))
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
                if (callback)
                    for (uint i = 0; i < fieldc; ++i)
                        callback(userdata, res++, row[i]);
                else res++;
            }
            else break;
        }
        mysql_free_result(myres);

    } while (mysql_next_result(database->mysql) == 0);

    sx_return(res);
}

uint sx_database_query_cb_fmt(struct sx_database * database, sx_database_func callback, void* userdata, const char* strformat, ...)
{
    if (!strformat) return 0;
    sx_trace();

    //  init query string
    va_list args;
    va_start(args, strformat);
    int len = sx_vsprintf_len(strformat, args);
    char* query = (char*)alloca(len);
    sx_vsprintf(query, len, strformat, args);
    va_end(args);

    int res = sx_database_query_cb(database, callback, userdata, query);

    sx_return(res);
}

static void sx_db_cb_str(void * userdata, const int index, const char* data)
{
    struct sx_string * str = userdata;
    if (index)
        sx_string_append(str, sx_database_sepchar);
    sx_string_append(str, data);
}

uint sx_database_query_str(struct sx_database * database, struct sx_string * result, const char* strformat, ...)
{
    if (!strformat) return 0;
    sx_trace();

    //  init query string
    va_list args;
    va_start(args, strformat);
    int len = sx_vsprintf_len(strformat, args);
    char* query = (char*)alloca(len);
    sx_vsprintf(query, len, strformat, args);
    va_end(args);

    int res = sx_database_query_cb(database, sx_db_cb_str, result, query);

    sx_return(res);
}

typedef struct sx_db_ud
{
    int     ptr;
    uint    len;
    char*   des;
}
sx_db_ud;

static void sx_db_cb_ud(void * userdata, const int index, const char* data)
{
    struct sx_db_ud * ud = userdata;
    uint datalen = sx_str_len(data);
    uint rmndlen = ud->len - ud->ptr;
    if (datalen + 1 > rmndlen) return;

    const char* spchar = sx_database_sepchar;
    if (index)
        ud->des[ud->ptr++] = *spchar;

    sx_mem_copy(ud->des + ud->ptr, data, datalen);
    ud->ptr += datalen;
}

uint sx_database_query(struct sx_database * database, char* dest, const uint destsize, const char* strformat, ...)
{
    if (!strformat) return 0;
    sx_trace();

    //  init query string
    va_list args;
    va_start(args, strformat);
    int len = sx_vsprintf_len(strformat, args);
    char* query = (char*)alloca(len);
    sx_vsprintf(query, len, strformat, args);
    va_end(args);

    sx_db_ud ud = {0, destsize, dest};
    int res = sx_database_query_cb(database, sx_db_cb_ud, &ud, query);

    sx_return(res);
}

bool sx_database_invalid_data(const char* data)
{
    return sx_str_str(data, "<") || sx_str_str(data, ">") || sx_str_str(data, ";");
}
