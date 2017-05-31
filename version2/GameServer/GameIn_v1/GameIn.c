#include "GameIn.h"
#include "CryptoService.h"
#include <malloc.h>


static const char *s_http_port = "8000";
static uint s_request_count = 0;
static struct sx_threadpool * s_threadpool = null;

static void send_to_client(struct mg_connection *nc, const void* data, const int lenght)
{
    mg_send_head(nc, 200, lenght, "Content-Type: application/octet-stream");
    mg_send(nc, data, lenght);
}

static void api_authen_authcode(struct mg_connection *nc, struct http_message *hm)
{
    if (hm->body.len < 32) return;

    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, hm->body.p, hm->body.len);
    if (json.nodescount == 5)
    {
        sx_json_node nodes[5] = init;
        json.nodes = nodes;
        sx_json_parse(&json, hm->body.p, hm->body.len);

        char userdata[32] = init;
        sx_json_read_string(&json, userdata, 32, "user_data");

        char reckey[33] = init;
        sx_json_read_string(&json, reckey, 33, "public_key");

        crypto_authen_code authcode = {1, 0};

        char publckey[33] = init;
        if (crypto_compute_keys(authcode.local_key, publckey, reckey)) return;
        
        //printf("%.*s\n", 32, authcode.local_key);

        char token[64] = init;
        crypto_token_generate(token, 64, &authcode, sizeof(authcode));

        char tmp[256] = init;
        int len = sx_sprintf(tmp, 256, "{\"user_data\":\"%s\",\"public_key\":\"%s\",\"auth_code\":\"%s\"}", userdata, publckey, token);

        send_to_client(nc, tmp, len);
    }
}

typedef struct therad_task_authen
{
    bool                done;
    char                key[crypto_key_len];
    struct sx_string    request;
    struct sx_string    result;
}
therad_task_authen;

static void authen_accesscode_thread(void* p)
{
    struct therad_task_authen* data = (struct therad_task_authen*)p;

    //  read data
    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, data->request.text, data->request.len);
    if (json.nodescount == 9)
    {
        sx_json_node nodes[9] = init;
        json.nodes = nodes;

        sx_json_parse(&json, data->request.text, data->request.len);

        char user[32] = init;
        sx_json_read_string(&json, user, 32, "user");

        char pass[33] = init;
        sx_json_read_string(&json, pass, 33, "pass");

        char openid[64] = init;
        sx_json_read_string(&json, openid, 64, "openid");

        char deviceid[64] = init;
        sx_json_read_string(&json, deviceid, 64, "deviceid");

        //  request profile id from database


        sx_string_set(&data->result, user);
        sx_encrypt(data->result.text, data->result.text, data->result.len, data->key, crypto_key_len);
        data->done = true;
    }
}

static void authen_accesscode(struct mg_connection *nc, struct http_message *hm)
{
    if (hm->body.len < crypto_authen_code_len) return;
    if ( crypto_token_validate(hm->body.p, crypto_authen_code_len) < 1 ) return;

    //  extract authentication code
    crypto_authen_code authcode = init;
    crypto_token_decode(&authcode, sizeof(authcode), hm->body.p, crypto_authen_code_len);
    if (authcode.version != 1) return;

    //  read encrypted data
    char data[256] = init;
    uint size = hm->body.len - crypto_authen_code_len;
    sx_mem_copy(data, hm->body.p + crypto_authen_code_len, size);
    
    //  decrypt data from client
    crypto_decrypt(data, data, size, authcode.local_key, crypto_key_len);

    //  read data
    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, data, size);
    if (json.nodescount == 9)
    {
        //  allocate thread object to read data from database
        therad_task_authen* thobject = (therad_task_authen*)sx_mem_calloc(sizeof(therad_task_authen));
        sx_mem_copy(thobject->key, authcode.local_key, crypto_key_len);
        sx_string_set(&thobject->request, data);

        //  assign thread object to connection
        nc->user_data = thobject;

        // TODO: send these data to a thread to request profileID from database
        sx_threadpool_add_job( s_threadpool, authen_accesscode_thread, thobject );
    }
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    switch (ev)
    {
        case MG_EV_HTTP_REQUEST:
        {
            struct http_message *hm = (struct http_message *) ev_data;
            if (mg_vcmp(&hm->uri, "/authen/authencode") == 0)
            {
                s_request_count++;
                api_authen_authcode(nc, hm);
                nc->flags |= MG_F_SEND_AND_CLOSE;
            }
            if (mg_vcmp(&hm->uri, "/authen/accesscode") == 0)
            {
                s_request_count++;
                authen_accesscode(nc, hm);
            }
            else send_to_client(nc, "{seganx@gmail.com}", 18);
        } break;

        case MG_EV_POLL:
            if (nc->user_data)
            {
                therad_task_authen* thobject = (therad_task_authen*)nc->user_data;
                if (thobject->done)
                {
                    send_to_client(nc, thobject->result.text, thobject->result.len);
                    sx_mem_free_and_null(nc->user_data);
                    nc->flags |= MG_F_SEND_AND_CLOSE;
                }
            }
            break;
    }
}


int main(int argc, char* argv[])
{
    sx_trace_attach(10, "gamein crash report.txt");
    sx_trace();

    struct mg_mgr mgr;
    struct mg_connection *nc;
    struct mg_bind_opts bind_opts;
    const char *err_str;

    mg_mgr_init(&mgr, NULL);

    /* Set HTTP server options */
    memset(&bind_opts, 0, sizeof(bind_opts));
    bind_opts.error_string = &err_str;
    nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
    if (nc == NULL)
    {
        fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port, *bind_opts.error_string);
        getchar();
        exit(1);
    }
    mg_set_protocol_http_websocket(nc);

    printf("Starting server on port %s\n", s_http_port);

    s_threadpool = sx_threadpool_create(100);

    sx_time t = sx_time_now();
    for (;;) {
        if (sx_time_diff(sx_time_now(), t) > 1)
        {
            t = sx_time_now();
            printf("\rRPS: %4u  TJC: %4u", s_request_count, sx_threadpool_num_jobs(s_threadpool));
            s_request_count = 0;
        }
        mg_mgr_poll(&mgr, 100);
    }

    sx_threadpool_destroy( s_threadpool );

    mg_mgr_free(&mgr);

    getchar();
    sx_return(0);
}

