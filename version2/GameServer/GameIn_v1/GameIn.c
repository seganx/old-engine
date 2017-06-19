#include "GameIn.h"
#include "authenticator.h"
#include "crypto.h"
#include "database.h"
#include <malloc.h>

struct gamein g_gamein = init;

static int gamein_read_header(char* dest, const uint destsize, struct http_message *hm, const char* name)
{
    struct mg_str* tmp = mg_get_http_header(hm, name);
    if (tmp && tmp->len < destsize)
    {
        sx_mem_copy(dest, tmp->p, tmp->len);
        return tmp->len;
    }
    return 0;
}

uint gamein_header_read(struct gamein_header* dest, struct http_message *hm)
{
    gamein_read_header(dest->user_data, 63, hm, "gamein_userdata");
    return gamein_read_header(dest->access_code, 95, hm, "gamein_access");
}

struct gamein_access_token * gamein_access_token_read(struct gamein_access_token *dest, const char* data, const uint size)
{
    if (crypto_token_validate(data, size))
    {
        char tmp[sizeof(gamein_access_token) + 3] = init;
        if (crypto_token_decode(tmp, sizeof(gamein_access_token) + 3, data, size))
            sx_mem_copy(dest, tmp, sizeof(gamein_access_token));
    }
    return dest;    
}

int gamein_send_and_close(struct mg_connection *nc, const gamein_header *header, const void *data, const int lenght)
{
    sx_trace();

    char hdr[256] = init;
    sx_sprintf(hdr, 256, 
        "Content-Type: application/octet-stream\n"
        "gamein_userdata: %s\n"
        "gamein_error: %u\n"
        "gamein_access: %s",
        header->user_data, 
        header->error,
        header->access_code);

    mg_send_head(nc, 200, lenght, hdr);
    mg_send(nc, data, lenght);
    nc->flags |= MG_F_SEND_AND_CLOSE;

    sx_return(0);
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
#if 0
    {
        for (int i = 1; i < 256; ++i)
        {
            int blen = sx_base64_encode_len(i);
            int slen = sx_base64_decode_len(blen);
            if (slen != i)
                printf("%3u", i);
        }
        getchar();
        return 0;
    }
#endif

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

