#include "GameIn.h"
#include "crypto.h"
#include "database.h"
#include <malloc.h>


#define authen_md5_salt     "seganx"
#define authen_uri_code     "/authen/code"
#define authen_uri_device   "/authen/device"
#define authen_uri_usr_reg  "/authen/userpass/register"
#define authen_uri_usr_log  "/authen/userpass/login"

typedef struct authen_data
{
    const char*     mode;
    uint            ver;
    char            gamekey[33];
    char            device[33];
    union data
    {
        struct
        {
            char    google[33];
            char    facebook[33];
        };
        struct
        {
            char    user[33];
            char    pass[33];
        };
    };
}
authen_data;


typedef struct authen_task
{
    bool                        done;
    struct gamein_header        header;
    struct gamein_access_token  token;
    struct authen_data          data;
}
authen_task;


static int authen_authcode(struct mg_connection *nc, struct http_message *hm)
{
    sx_trace();

    gamein_header header = init;
    gamein_header_read(&header, hm);

    if (hm->body.len == 32)
    {
        char reckey[33] = init;
        sx_mem_copy(reckey, hm->body.p, 32);

        gamein_access_token authcode = { 1, 0 };

        char publckey[33] = init;
        if (crypto_compute_keys(authcode.local_key, publckey, reckey, gamein_key_len))
        {
            crypto_token_generate(header.access_code, 96, &authcode, sizeof(authcode));
            sx_return(gamein_send_and_close(nc, &header, publckey, 32));
        }
    }

    header.error = GIE_INVALID_DATA;
    sx_return(gamein_send_and_close(nc, &header, "", 1));
}

static void authen_accesscode_send_result(struct authen_task* task)
{
    if (task->header.error == NO_ERROR && task->token.game_id > 0 && task->token.profile_id > 0)
    {
        task->token.version = 1;
        crypto_token_generate(task->header.access_code, 95, &task->token, sizeof(task->token));
    }
    task->done = true;
}

static void authen_accesscode_device(struct authen_task* task)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        char tmp[64] = init;

        //  verify game secret key
        if (sx_database_query(&dbase, tmp, 64, "SELECT game_id FROM gamekey WHERE (game_key='%s');", task->data.gamekey))
            task->token.game_id = sx_str_to_uint(tmp, 0);


        //  verify that there is nothing here
        if (sx_database_query(&dbase, tmp, 64, "SELECT profile_id FROM authen WHERE (device='%s');", task->data.device) < 1)
            sx_database_query(&dbase, tmp, 64, "INSERT INTO authen (device) VALUES('%s'); SELECT profile_id FROM authen WHERE (device='%s');", task->data.device, task->data.device);
        
        task->token.profile_id = sx_str_to_uint(tmp, 0);

        sx_database_finalize(&dbase);
    }
    authen_accesscode_send_result(task);

    sx_return();
}

static void authen_accesscode_user_register(struct authen_task* task)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        char tmp[64] = init;

        //  verify username is not exist in database
        if (sx_database_query(&dbase, tmp, 64, "SELECT profile_id FROM authen WHERE (username='%s');", task->data.user))
        {
            //  since there is someone registered before we can not register it again
            task->header.error = GIE_INVALID_USERPASS;
        }
        else
        {
            //  verify game secret key
            if (sx_database_query(&dbase, tmp, 64, "SELECT game_id FROM gamekey WHERE (game_key='%s');", task->data.gamekey))
                task->token.game_id = sx_str_to_uint(tmp, 0);

            // verify that player had an account on current device
            if (sx_database_query(&dbase, tmp, 64, "SELECT profile_id FROM authen WHERE (device='%s' || google='%s' || facebook='%s');", task->data.device, task->data.google, task->data.facebook))
            {
                //  just update username and password for this profile id
                sx_database_query_cb_fmt(&dbase, null, null, "UPDATE authen SET username='%s', password='%s', device=NULL WHERE (profile_id='%s');", task->data.user, task->data.pass, tmp);
                task->token.profile_id = sx_str_to_uint(tmp, 0);
            }
            else  // there is no account with this device
            {
                //  create a new account
                sx_database_query(&dbase, tmp, 64,
                    "INSERT INTO authen (username, password) VALUES ('%s' ,'%s');"
                    "SELECT profile_id FROM authen WHERE (username='%s');", task->data.user, task->data.pass, task->data.user);
                task->token.profile_id = sx_str_to_uint(tmp, 0);
            }
        }

        sx_database_finalize(&dbase);
    }
    authen_accesscode_send_result(task);

    sx_return();
}

static void authen_accesscode_user_login(struct authen_task* task)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        char tmp[34] = init;

        //  verify game secret key
        if (sx_database_query(&dbase, tmp, 34, "SELECT game_id FROM gamekey WHERE (game_key='%s');", task->data.gamekey))
            task->token.game_id = sx_str_to_uint(tmp, 0);

        //  verify that user has valid account
        if (sx_database_query(&dbase, tmp, 34, "SELECT profile_id FROM authen WHERE (username='%s' AND password='%s');", task->data.user, task->data.pass))
            task->token.profile_id = sx_str_to_uint(tmp, 0);

        sx_database_finalize(&dbase);
    }
    authen_accesscode_send_result(task);

    sx_return();
}

static void authen_accesscode_thread(void* p)
{
    sx_trace();

    struct authen_task* task = (struct authen_task*)p;

    if (task->data.mode == authen_uri_device)
        authen_accesscode_device(task);
    else if (task->data.mode == authen_uri_usr_reg)
        authen_accesscode_user_register(task);
    else if (task->data.mode == authen_uri_usr_log)
        authen_accesscode_user_login(task);

    sx_return();
}

static int authen_access_error(struct mg_connection *nc, gamein_header *header, gamein_error error)
{
    header->error = error;
    return gamein_send_and_close(nc, header, "", 1);
}

static int authen_access(struct mg_connection *nc, struct http_message *hm, const char* mode)
{
    sx_trace();

    gamein_header header = init;
    const uint access_len = gamein_header_read(&header, hm);

    //  extract authentication code
    gamein_access_token authcode = init;
    if (gamein_access_token_read(&authcode, header.access_code, access_len)->version != 1)
        sx_return(authen_access_error(nc, &header, GIE_INVALID_DATA));

    //  read encrypted data
    char data[256] = init;
    sx_mem_copy(data, hm->body.p, hm->body.len);

    //  decrypt data from client
    crypto_decrypt(data, data, hm->body.len, authcode.local_key, gamein_key_len);

    //  read data
    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, data, hm->body.len);
    if (json.nodescount < 2) sx_return(authen_access_error(nc, &header, GIE_INVALID_DATA));

    json.nodes = alloca(json.nodescount * sizeof(sx_json_node));
    sx_mem_set(json.nodes, 0, json.nodescount * sizeof(sx_json_node));
    sx_json_node* jsroot = sx_json_parse(&json, data, hm->body.len);

    //  read fields from json
    authen_data audata = init;
    audata.ver = sx_json_read_int(jsroot, "ver", 0);
    if (audata.ver != 1) sx_return(authen_access_error(nc, &header, GIE_INVALID_DATA));

    audata.mode = mode;
    if (sx_json_read_string(jsroot, "game", audata.gamekey, 33) < 10 || sx_database_invalid_data(audata.gamekey))
        sx_return(authen_access_error(nc, &header, GIE_INVALID_DATA));

    if (sx_json_read_string(jsroot, "device", audata.device, 33) < 10 || sx_database_invalid_data(audata.device))
        sx_return(authen_access_error(nc, &header, GIE_INVALID_DATA));

    if (mode == authen_uri_usr_log || mode == authen_uri_usr_reg)
    {
        if (sx_json_read_string(jsroot, "user", audata.user, 33) < 3 || sx_database_invalid_data(audata.user))
            sx_return(authen_access_error(nc, &header, GIE_INVALID_DATA));

        if (sx_json_read_string(jsroot, "pass", audata.pass, 33) < 6)
            sx_return(authen_access_error(nc, &header, GIE_INVALID_DATA));

        sx_md5(audata.pass, audata.pass, authen_md5_salt, null);
    }

    //  allocate thread object to read data from database
    authen_task* task = (authen_task*)sx_mem_calloc(sizeof(authen_task));
    sx_mem_copy(&task->header, &header, sizeof(header));
    sx_mem_copy(&task->token, &authcode, sizeof(authcode));
    sx_mem_copy(&task->data, &audata, sizeof(audata));

    //  assign thread object to connection
    nc->user_data = task;

    // TODO: send these data to a thread to request profileID from database
    sx_threadpool_add_job(g_gamein.threadpool, authen_accesscode_thread, task);

    sx_return(0);
}

void gamein_handle_request(struct mg_connection *nc, struct http_message *hm)
{
    if (mg_vcmp(&hm->uri, authen_uri_code) == 0)
    {
        g_gamein.request_count++;
        authen_authcode(nc, hm);
    }
    else if (mg_vcmp(&hm->uri, authen_uri_device) == 0)
    {
        g_gamein.request_count++;
        authen_access(nc, hm, authen_uri_device);
    }
    else if (mg_vcmp(&hm->uri, authen_uri_usr_reg) == 0)
    {
        g_gamein.request_count++;
        authen_access(nc, hm, authen_uri_usr_reg);
    }
    else if (mg_vcmp(&hm->uri, authen_uri_usr_log) == 0)
    {
        g_gamein.request_count++;
        authen_access(nc, hm, authen_uri_usr_log);
    }
    else 
    {
        gamein_header header = init;
        gamein_send_and_close(nc, &header, "{seganx@gmail.com}", 18);
    }
}

void gamein_handle_pool(struct mg_connection *nc)
{
    if (nc->user_data)
    {
        authen_task* task = (authen_task*)nc->user_data;
        if (task->done)
        {
            gamein_send_and_close(nc, &task->header, "", 1);
            sx_mem_free_and_null(nc->user_data);
        }
    }
}