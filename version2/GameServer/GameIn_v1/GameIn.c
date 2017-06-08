#include "GameIn.h"
#include "authenticator.h"
#include "crypto.h"
#include "database.h"
#include <malloc.h>

struct gamein g_gamein = init;

void send_and_close(struct mg_connection *nc, const void* data, const int lenght)
{
    sx_trace();
    mg_send_head(nc, 200, lenght, "Content-Type: application/octet-stream");
    mg_send(nc, data, lenght);
    nc->flags |= MG_F_SEND_AND_CLOSE;
    sx_return();
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    switch (ev)
    {
        case MG_EV_HTTP_REQUEST:
            gamein_handle_request(nc, (struct http_message *)ev_data);
            break;

        case MG_EV_POLL:
            gamein_handle_pool(nc);
            break;
    }
}

int main(int argc, char* argv[])
{
    struct sx_database_config database_config = init;
    g_gamein.database_config = &database_config;

    //  read config from file
    {
        FILE* file = NULL;
        if (fopen_s(&file, "config.json", "r") == 0)
        {
            char tmp[2048] = init;
            int len = fread_s(tmp, 2048, 1, 2048, file);
            sx_json config = init;

            config.nodescount = sx_json_node_count(&config, tmp, len);
            config.nodes = (sx_json_node*)alloca(config.nodescount * sizeof(sx_json_node));
            sx_mem_set(config.nodes, 0, config.nodescount * sizeof(sx_json_node));

            sx_json_node* root = sx_json_parse(&config, tmp, len);
            sx_json_read_string(root, "trace", g_gamein.trace_file, 32);
            sx_json_read_string(root, "port", g_gamein.http_port, 8);
            g_gamein.threads_count = sx_json_read_int(root, "threads", 0);

            sx_json_node* dbconfig = sx_json_find(&config, "database");
            sx_json_read_string(dbconfig, "host", g_gamein.database_config->host, 128);
            sx_json_read_string(dbconfig, "name", g_gamein.database_config->name, 64);
            sx_json_read_string(dbconfig, "user", g_gamein.database_config->user, 64);
            sx_json_read_string(dbconfig, "pass", g_gamein.database_config->pass, 64);
            g_gamein.database_config->port = sx_json_read_int(dbconfig, "port", 3306);
        }
    }

    sx_trace_attach(10, g_gamein.trace_file);
    sx_trace();

    struct mg_mgr mgr;
    mg_mgr_init(&mgr, NULL);

    struct mg_bind_opts bind_opts;
    const char *err_str;
    memset(&bind_opts, 0, sizeof(bind_opts));
    bind_opts.error_string = &err_str;

    struct mg_connection *nc;
    nc = mg_bind_opt(&mgr, g_gamein.http_port, ev_handler, bind_opts);
    if (nc == NULL)
    {
        fprintf(stderr, "Error starting server on port %s: %s\n", g_gamein.http_port, *bind_opts.error_string);
        getchar();
        sx_return(0);
    }
    mg_set_protocol_http_websocket(nc);

    printf("Starting server on port %s\n", g_gamein.http_port);

    if (g_gamein.threads_count)
        g_gamein.threadpool = sx_threadpool_create(g_gamein.threads_count, g_gamein.trace_file);

    uint rpc = 0;
    sx_time t = sx_time_now();
    for (; sx_getch() != 27;) {
        if (sx_time_diff(sx_time_now(), t) > 1)
        {
            t = sx_time_now();
            rpc = g_gamein.request_count;
            g_gamein.request_count = 0;
        }

        if (g_gamein.threads_count)
            printf("\rRPS: %03u BT: %03u TJC: %03u", rpc, sx_threadpool_num_busy_threads(g_gamein.threadpool), sx_threadpool_num_jobs(g_gamein.threadpool));
        else
            printf("\rRPS: %03u", rpc);

        mg_mgr_poll(&mgr, 10);
    }

    sx_threadpool_destroy(g_gamein.threadpool);

    mg_mgr_free(&mgr);

    sx_trace_detach();

    printf("\nPress enter to close...");
    getchar();
    return 0;
}

