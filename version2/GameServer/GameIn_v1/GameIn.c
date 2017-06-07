#include "GameIn.h"
#include "CryptoService.h"
#include "database.h"
#include <malloc.h>


static char                         s_trace_file[32] = init;
static char                         s_http_port[8] = init;
static struct sx_threadpool *       s_threadpool = null;
static uint                         s_threads_count = 100;
static struct sx_database_config    s_database_config = init;
static uint                         s_request_count = 0;


static void send_to_client(struct mg_connection *nc, const void* data, const int lenght)
{
    sx_trace();
    mg_send_head(nc, 200, lenght, "Content-Type: application/octet-stream");
    mg_send(nc, data, lenght);
    sx_return();
}

static void api_authen_authcode(struct mg_connection *nc, struct http_message *hm)
{
    sx_trace();

    if (hm->body.len < 32) sx_return();

    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, hm->body.p, hm->body.len);
    if (json.nodescount == 5)
    {
        sx_json_node nodes[5] = init;
        json.nodes = nodes;
        sx_json_node* root = sx_json_parse(&json, hm->body.p, hm->body.len);

        char userdata[32] = init;
        sx_json_read_string(root, "user_data", userdata, 32);

        char reckey[33] = init;
        sx_json_read_string(root, "public_key", reckey, 33);

        crypto_authen_code authcode = { 1, 0 };

        char publckey[33] = init;
        if (crypto_compute_keys(authcode.local_key, publckey, reckey)) sx_return();

        char token[64] = init;
        crypto_token_generate(token, 64, &authcode, sizeof(authcode));

        char tmp[256] = init;
        int len = sx_sprintf(tmp, 256, "{\"user_data\":\"%s\",\"public_key\":\"%s\",\"auth_code\":\"%s\"}", userdata, publckey, token);

        send_to_client(nc, tmp, len);
    }

    sx_return();
}

typedef struct therad_task
{
    bool                done;
    char                key[crypto_key_len];
    struct sx_string    request;
    struct sx_string    result;
}
therad_task;

static void authen_accesscode_send_result(struct therad_task* data, const char* userdata, const char* profile_id, const gamein_error error)
{
    sx_trace();

    if (error == NO_ERROR)
    {
        //  create access code version 1
        crypto_access_code access_code = { 1, 0, 0 };
        access_code.profile_id = sx_str_to_uint(profile_id, 0);
        sx_mem_copy(access_code.local_key, data->key, crypto_key_len);

        //  generate token
        char token[128] = init;
        crypto_token_generate(token, 128, &access_code, sizeof(access_code));

        //  prepare result
        sx_string_format(&data->result, "{\"user_data\":\"%s\",\"error\":\"%u\",\"access_code\":\"%s\"}", userdata, error, token);
    }
    else sx_string_format(&data->result, "{\"user_data\":\"%s\",\"error\":\"%u\",\"access_code\":\"\"}", userdata, error);

    data->done = true;

    sx_return();
}

static void authen_accesscode_userpass(struct therad_task* data, const char* userdata, const char* device, const char* type, const char* user, const char* pass)
{
    sx_trace();

    //  request profile id from database
    sx_string dbquery = init, dbresult = init, profileid = init;

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, &s_database_config))
    {
        if (sx_str_cmp(type, "login") == 0)
        {
            sx_string_format(&dbquery, "SELECT profile_id FROM authen WHERE (username='%s' AND password='%s');", user, pass);
            if (sx_database_query(&dbase, &dbquery, &profileid))
            {
                sx_string_format(&dbquery, "UPDATE authen SET device='%s' WHERE (profile_id='%s');", device, profileid.text);
                sx_database_query(&dbase, &dbquery, &dbresult);
                authen_accesscode_send_result(data, userdata, profileid.text, GIE_NO_ERROR);
            }
            else authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_USERPASS);
        }
        else if (sx_str_cmp(type, "register") == 0)
        {
            sx_string_format(&dbquery, "SELECT profile_id FROM authen WHERE (username='%s');", user);
            if (sx_database_query(&dbase, &dbquery, &dbresult) < 1)
            {
                sx_string_format(&dbquery, "UPDATE authen SET ('username'='%s' , 'password'='%s') WHERE (device='%s');", user, pass, device);
                sx_database_query(&dbase, &dbquery, &dbresult);
                authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_USERPASS);
            }
            else authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_USERPASS);
        }
        else authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_DATA);

        sx_database_finalize(&dbase);
    }

    sx_string_clear(&dbquery);
    sx_string_clear(&dbresult);
    sx_string_clear(&profileid);

    sx_return();
}

static void authen_accesscode_openid(struct therad_task* data, const char* userdata, const char* device, const char* provider, const char* openid)
{
    sx_trace();

    sx_return();
}

static void authen_accesscode_device(struct therad_task* data, const char* userdata, const char* device)
{
    sx_trace();

    //  request profile id from database
    sx_string dbquery = init, dbresult = init;

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, &s_database_config))
    {
        sx_string_format(&dbquery, "SELECT profile_id FROM authen WHERE (device='%s');", device);
        if (sx_database_query(&dbase, &dbquery, &dbresult) < 1)
        {
            sx_string_format(&dbquery, "INSERT INTO authen (device) VALUES('%s');", device);
            sx_database_query(&dbase, &dbquery, &dbresult);
            sx_string_format(&dbquery, "SELECT profile_id FROM authen WHERE (device='%s');", device);
            sx_database_query(&dbase, &dbquery, &dbresult);
        }
        
        authen_accesscode_send_result(data, userdata, dbresult.text, GIE_NO_ERROR);

        sx_database_finalize(&dbase);
    }
    else authen_accesscode_send_result(data, "", "0", GIE_INVALID_DATA);

    sx_string_clear(&dbquery);
    sx_string_clear(&dbresult);

    sx_return();
}


static void authen_accesscode_thread(void* p)
{
    sx_trace();

    struct therad_task* data = (struct therad_task*)p;

    //  read data
    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, data->request.text, data->request.len);
    if (json.nodescount == 15)
    {
        sx_json_node nodes[15] = init;
        json.nodes = nodes;
        
        sx_json_node* jsroot = sx_json_parse(&json, data->request.text, data->request.len);

        char userdata[33] = init;
        sx_json_read_string(jsroot, "user_data", userdata, 33);

        char device[33] = init;
        if (sx_json_read_string(jsroot, "device", device, 33) < 10 || !sx_database_verify_data(device))
        {
            authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_DATA);
            sx_return();
        }

        char type[16] = init;
        if (sx_json_read_string(jsroot, "type", type, 16) > 2)
        {
            char user[33] = init;
            if (sx_json_read_string(jsroot, "user", user, 33) > 5 && sx_database_verify_data(user))
            {
                char pass[33] = init;
                if (sx_json_read_string(jsroot, "pass", pass, 33) > 5 && sx_database_verify_data(pass))
                {
                    char tmp[33] = init;
                    authen_accesscode_userpass(data, userdata, device, type, user, sx_md5(pass, "seganx", null));
                    sx_return();
                }
            }
        }

        char google[33] = init;
        if (sx_json_read_string(jsroot, "google", google, 33) > 10 && sx_database_verify_data(google))
        {
            authen_accesscode_openid(data, userdata, device, "google", google);
            sx_return();
        }

        char facebook[33] = init;
        if (sx_json_read_string(jsroot, "facebook", facebook, 33) > 10 && sx_database_verify_data(facebook))
        {
            authen_accesscode_openid(data, userdata, device, "facebook", facebook);
            sx_return();
        }

        //  just login with device
        authen_accesscode_device(data, userdata, device);
    }
    else authen_accesscode_send_result(data, "", "0", GIE_INVALID_DATA);

    sx_return();
}

static void authen_accesscode(struct mg_connection *nc, struct http_message *hm)
{
    if (hm->body.len < crypto_authen_code_len) return;
    if (crypto_token_validate(hm->body.p, crypto_authen_code_len) < 1) return;

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
    if (json.nodescount == 15)
    {
        //  allocate thread object to read data from database
        therad_task* thobject = (therad_task*)sx_mem_calloc(sizeof(therad_task));
        sx_mem_copy(thobject->key, authcode.local_key, crypto_key_len);
        sx_string_set(&thobject->request, data);

        //  assign thread object to connection
        nc->user_data = thobject;

        // TODO: send these data to a thread to request profileID from database
        sx_threadpool_add_job(s_threadpool, authen_accesscode_thread, thobject);
    }
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    sx_trace();

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
                therad_task* task = (therad_task*)nc->user_data;
                if (task->done)
                {
                    send_to_client(nc, task->result.text, task->result.len);
                    sx_string_clear(&task->request);
                    sx_string_clear(&task->result);
                    sx_mem_free_and_null(nc->user_data);
                    nc->flags |= MG_F_SEND_AND_CLOSE;
                }
            }
            break;
    }

    sx_return();
}

int main(int argc, char* argv[])
{
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
            sx_json_read_string(root, "trace", s_trace_file, 32);
            sx_json_read_string(root, "port", s_http_port, 8);
            s_threads_count = sx_json_read_int(root, "threads", 50);

            sx_json_node* dbconfig = sx_json_find(&config, "database");
            sx_json_read_string(dbconfig, "host", s_database_config.host, 128);
            sx_json_read_string(dbconfig, "name", s_database_config.name, 64);
            sx_json_read_string(dbconfig, "user", s_database_config.user, 64);
            sx_json_read_string(dbconfig, "pass", s_database_config.pass, 64);
            s_database_config.port = sx_json_read_int(dbconfig, "port", 3306);

            //sx_json_print(dbconfig);
        }
    }

    sx_trace_attach(10, s_trace_file);
    sx_trace();

    struct mg_mgr mgr;
    mg_mgr_init(&mgr, NULL);

    struct mg_bind_opts bind_opts;
    const char *err_str;
    memset(&bind_opts, 0, sizeof(bind_opts));
    bind_opts.error_string = &err_str;

    struct mg_connection *nc;
    nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
    if (nc == NULL)
    {
        fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port, *bind_opts.error_string);
        getchar();
        sx_return(0);
    }
    mg_set_protocol_http_websocket(nc);

    printf("Starting server on port %s\n", s_http_port);

    s_threadpool = sx_threadpool_create(s_threads_count, s_trace_file);

    uint rpc = 0;
    sx_time t = sx_time_now();
    for (;sx_getch() != 27;) {
        if (sx_time_diff(sx_time_now(), t) > 1)
        {
            t = sx_time_now();
            rpc = s_request_count;
            s_request_count = 0;
        }

        printf("\rRPS: %03u BT: %03u TJC: %03u", rpc, sx_threadpool_num_busy_threads(s_threadpool), sx_threadpool_num_jobs(s_threadpool));
        mg_mgr_poll(&mgr, 10);
    }

    sx_threadpool_destroy(s_threadpool);

    mg_mgr_free(&mgr);

    sx_return(0);
}

