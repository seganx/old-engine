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
    char            userdata[33];
    char            device[33];
    char            user[33];
    char            pass[33];
    char            google[33];
    char            facebook[33];
}
authen_data;

typedef struct authen_task
{
    bool                done;
    char                key[crypto_key_len];
    struct authen_data  data;
    struct sx_string    result;
}
authen_task;

static void authen_authcode(struct mg_connection *nc, struct http_message *hm)
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

        send_and_close(nc, tmp, len);
    }

    sx_return();
}

static void authen_accesscode_send_result(struct authen_task* data, const char* userdata, const char* profile_id, const gamein_error error)
{
    sx_trace();

    if (error == NO_ERROR && *profile_id != '0')
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

static void authen_accesscode_device(struct authen_task* task, struct authen_data * data)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        char tmp[64] = init;

        //  verify that there is nothing here
        if (sx_database_query(&dbase, tmp, 64, "SELECT profile_id FROM authen WHERE (device='%s');", data->device) < 1)
        {
            sx_database_query(&dbase, tmp, 64, "INSERT INTO authen (device) VALUES('%s'); SELECT profile_id FROM authen WHERE (device='%s');", data->device, data->device);
            authen_accesscode_send_result(task, data->userdata, tmp, GIE_NO_ERROR);
        }
        else authen_accesscode_send_result(task, data->userdata, tmp, GIE_NO_ERROR);

        sx_database_finalize(&dbase);
    }
    else authen_accesscode_send_result(task, "", "0", GIE_INVALID_DATA);

    sx_return();
}

static void authen_accesscode_user_register(struct authen_task* task, struct authen_data* data)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        char tmp[64] = init;

        //  verify username is not exist in database
        if (sx_database_query(&dbase, tmp, 64, "SELECT profile_id FROM authen WHERE (username='%s');", data->user))
        {
            //  since there is someone registered before we can not register it again
            authen_accesscode_send_result(task, data->userdata, "0", GIE_INVALID_USERPASS);
        }
        else
        {
            // verify that player had an account on current device
            if (sx_database_query(&dbase, tmp, 64, "SELECT profile_id FROM authen WHERE (device='%s' || google='%s' || facebook='%s');", data->device, data->google, data->facebook))
            {
                //  just update username and password for this profile id
                sx_database_query_cb_fmt(&dbase, null, null, "UPDATE authen SET username='%s', password='%s', device=NULL WHERE (profile_id='%s');", data->user, data->pass, tmp);
                authen_accesscode_send_result(task, data->userdata, tmp, GIE_NO_ERROR);
            }
            else  // there is no account with this device
            {
                //  create a new account
                sx_database_query(&dbase, tmp, 64,
                    "INSERT INTO authen (username, password) VALUES ('%s' ,'%s');"
                    "SELECT profile_id FROM authen WHERE (username='%s');", data->user, data->pass, data->user);
                authen_accesscode_send_result(task, data->userdata, tmp, GIE_NO_ERROR);
            }
        }

        sx_database_finalize(&dbase);
    }

    sx_return();
}

static void authen_accesscode_user_login(struct authen_task* task, struct authen_data* data)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        char tmp[34] = init;

        //  verify that user has valid account
        if (sx_database_query(&dbase, tmp, 34, "SELECT profile_id FROM authen WHERE (username='%s' AND password='%s');", data->user, data->pass))
            authen_accesscode_send_result(task, data->userdata, tmp, GIE_NO_ERROR);
        else
            authen_accesscode_send_result(task, data->userdata, "0", GIE_INVALID_USERPASS);

        sx_database_finalize(&dbase);
    }

    sx_return();
}

static void authen_accesscode_thread(void* p)
{
    sx_trace();

    struct authen_task* task = (struct authen_task*)p;
    struct authen_data* data = &task->data;

    if (data->mode == authen_uri_device)
        authen_accesscode_device(task, data);
    else if (data->mode == authen_uri_usr_reg)
        authen_accesscode_user_register(task, data);
    else if (data->mode == authen_uri_usr_log)
        authen_accesscode_user_login(task, data);

    sx_return();
}

static int authen_send_error(struct mg_connection *nc, const char* userdata, gamein_error error)
{
    char tmp[128] = init;
    int len = sx_sprintf(tmp, 128, "{\"user_data\":\"%s\",\"error\":\"%u\",\"access_code\":\"\"}", userdata, error);
    return send_and_close(nc, tmp, len);
}

static int authen_accesscode(struct mg_connection *nc, struct http_message *hm, const char* mode)
{
    sx_trace();

    const uint crypto_authen_code_len = 52;
    if (hm->body.len < crypto_authen_code_len) sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));
    if (crypto_token_validate(hm->body.p, crypto_authen_code_len) < 1) sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));

    //  extract authentication code
    crypto_authen_code authcode = init;
    crypto_token_decode(&authcode, sizeof(authcode), hm->body.p, crypto_authen_code_len);
    if (authcode.version != 1) sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));

    //  read encrypted data
    char data[256] = init;
    uint size = hm->body.len - crypto_authen_code_len;
    sx_mem_copy(data, hm->body.p + crypto_authen_code_len, size);

    //  decrypt data from client
    crypto_decrypt(data, data, size, authcode.local_key, crypto_key_len);

    //  read data
    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, data, size);
    if (json.nodescount < 2) sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));


    json.nodes = alloca(json.nodescount * sizeof(sx_json_node));
    sx_mem_set(json.nodes, 0, json.nodescount * sizeof(sx_json_node));
    sx_json_node* jsroot = sx_json_parse(&json, data, size);

    //  read fields from json
    authen_data audata = init;
    audata.ver = sx_json_read_int(jsroot, "ver", 0);
    if (audata.ver != 1) sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));

    audata.mode = mode;
    sx_json_read_string(jsroot, "user_data", audata.userdata, 33);
    if (sx_json_read_string(jsroot, "device", audata.device, 33) < 10 || sx_database_invalid_data(audata.device))
        sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));

    if (mode == authen_uri_usr_log || mode == authen_uri_usr_reg)
    {
        if (sx_json_read_string(jsroot, "user", audata.user, 33) < 3 || sx_database_invalid_data(audata.user))
            sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));

        if (sx_json_read_string(jsroot, "pass", audata.pass, 33) < 6)
            sx_return(authen_send_error(nc, "", GIE_INVALID_DATA));

        sx_md5(audata.pass, audata.pass, authen_md5_salt, null);
    }

    //  allocate thread object to read data from database
    authen_task* task = (authen_task*)sx_mem_calloc(sizeof(authen_task));
    sx_mem_copy(task->key, authcode.local_key, crypto_key_len);
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
        authen_accesscode(nc, hm, authen_uri_device);
    }
    else if (mg_vcmp(&hm->uri, authen_uri_usr_reg) == 0)
    {
        g_gamein.request_count++;
        authen_accesscode(nc, hm, authen_uri_usr_reg);
    }
    else if (mg_vcmp(&hm->uri, authen_uri_usr_log) == 0)
    {
        g_gamein.request_count++;
        authen_accesscode(nc, hm, authen_uri_usr_log);
    }
    else send_and_close(nc, "{seganx@gmail.com}", 18);
}

void gamein_handle_pool(struct mg_connection *nc)
{
    if (nc->user_data)
    {
        authen_task* task = (authen_task*)nc->user_data;
        if (task->done)
        {
            send_and_close(nc, task->result.text, task->result.len);
            sx_string_clear(&task->result);
            sx_mem_free_and_null(nc->user_data);
        }
    }
}