#include "authenticator.h"
#include "crypto.h"
#include "database.h"
#include <malloc.h>


#define authen_md5_salt     "seganx"

typedef struct authen_task
{
    bool                done;
    char                key[crypto_key_len];
    struct sx_string    request;
    struct sx_string    result;
}
authen_task;

typedef struct authen_user
{
    uint    ver;
    char    userdata[33];
    char    device[33];
    char    type[16];
    char    user[33];
    char    pass[33];
    char    google[33];
    char    facebook[33];
}
authen_user;

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

static void authen_accesscode_userpass(struct authen_task* task, struct authen_user* data)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        if (sx_str_cmp(data->type, "login") == 0)
        {
            char tmp[34] = init;

            //  verify that user has valid account
            if (sx_database_query(&dbase, tmp, 34, "SELECT profile_id FROM authen WHERE (username='%s' AND password='%s');", data->user, data->pass))
                authen_accesscode_send_result(task, data->userdata, tmp, GIE_NO_ERROR);
            else
                authen_accesscode_send_result(task, data->userdata, "0", GIE_INVALID_USERPASS);
        }
        else if (sx_str_cmp(data->type, "register") == 0)
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
        }

        sx_database_finalize(&dbase);
    }

    sx_return();
}

static void authen_accesscode_device(struct authen_task* task, struct authen_user * data)
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

static void authen_accesscode_thread(void* p)
{
    sx_trace();

    struct authen_task* task = (struct authen_task*)p;

    //  read data
    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, task->request.text, task->request.len);
    json.nodes = alloca(json.nodescount * sizeof(sx_json_node));
    sx_mem_set(json.nodes, 0, json.nodescount * sizeof(sx_json_node));
    sx_json_node* jsroot = sx_json_parse(&json, task->request.text, task->request.len);

    authen_user data = init;
    data.ver = sx_json_read_int(jsroot, "ver", 0);
    if (data.ver == 1)
    {
        sx_json_read_string(jsroot, "user_data", data.userdata, 33);
        sx_json_read_string(jsroot, "device", data.device, 33);
        sx_json_read_string(jsroot, "type", data.type, 16);
        sx_json_read_string(jsroot, "user", data.user, 33);
        sx_json_read_string(jsroot, "pass", data.pass, 33);

        if (sx_str_len(data.device) < 10 || sx_database_invalid_data(data.device) || sx_database_invalid_data(data.user))
        {
            authen_accesscode_send_result(task, data.userdata, "0", GIE_INVALID_DATA);
            sx_return();
        }

        if (sx_str_len(data.type) > 2)
        {
            if (sx_str_len(data.user) > 2 && sx_str_len(data.pass) > 5)
            {
                sx_md5(data.pass, data.pass, authen_md5_salt, null);
                authen_accesscode_userpass(task, &data);
                sx_return();
            }

            //  type has value but username or password is invalid
            authen_accesscode_send_result(task, data.userdata, "0", GIE_INVALID_USERPASS);
            sx_return();
        }

        //  just login with device
        authen_accesscode_device(task, &data);
    }

    sx_return();
}

static void authen_accesscode(struct mg_connection *nc, struct http_message *hm)
{
    sx_trace();

    const uint crypto_authen_code_len = 52;
    if (hm->body.len < crypto_authen_code_len) sx_return();
    if (crypto_token_validate(hm->body.p, crypto_authen_code_len) < 1) sx_return();

    //  extract authentication code
    crypto_authen_code authcode = init;
    crypto_token_decode(&authcode, sizeof(authcode), hm->body.p, crypto_authen_code_len);
    if (authcode.version != 1) sx_return();

    //  read encrypted data
    char data[256] = init;
    uint size = hm->body.len - crypto_authen_code_len;
    sx_mem_copy(data, hm->body.p + crypto_authen_code_len, size);

    //  decrypt data from client
    crypto_decrypt(data, data, size, authcode.local_key, crypto_key_len);

    //  read data
    sx_json json = init;
    json.nodescount = sx_json_node_count(&json, data, size);
    if (json.nodescount > 2)
    {
        //  allocate thread object to read data from database
        authen_task* thobject = (authen_task*)sx_mem_calloc(sizeof(authen_task));
        sx_mem_copy(thobject->key, authcode.local_key, crypto_key_len);
        sx_string_set(&thobject->request, data);

        //  assign thread object to connection
        nc->user_data = thobject;

        // TODO: send these data to a thread to request profileID from database
        sx_threadpool_add_job(g_gamein.threadpool, authen_accesscode_thread, thobject);
    }
    else send_and_close(nc, "{\"user_data\":\"\",\"error\":\"1\",\"access_code\":\"\"}", 47);

    sx_return();
}

void gamein_handle_request(struct mg_connection *nc, struct http_message *hm)
{
    if (mg_vcmp(&hm->uri, "/authen/authencode") == 0)
    {
        g_gamein.request_count++;
        authen_authcode(nc, hm);
    }
    if (mg_vcmp(&hm->uri, "/authen/accesscode") == 0)
    {
        g_gamein.request_count++;
        authen_accesscode(nc, hm);
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
            sx_string_clear(&task->request);
            sx_string_clear(&task->result);
            sx_mem_free_and_null(nc->user_data);
        }
    }
}