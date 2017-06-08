#include "authenticator.h"
#include "crypto.h"
#include "database.h"
#include <malloc.h>


typedef struct authen_task
{
    bool                done;
    char                key[crypto_key_len];
    struct sx_string    request;
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

static void authen_accesscode_userpass(struct authen_task* data, const char* userdata, const char* device, const char* type, const char* user, const char* pass)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        if (sx_str_cmp(type, "login") == 0)
        {
            char profileid[16] = init;
            if (sx_database_query(&dbase, profileid, 16, "SELECT profile_id FROM authen WHERE (username='%s' AND password='%s');", user, pass))
            {
                sx_database_query_cb_fmt(&dbase, null, null, "UPDATE authen SET device='%s' WHERE (profile_id='%s');", device, profileid);
                authen_accesscode_send_result(data, userdata, profileid, GIE_NO_ERROR);
            }
            else authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_USERPASS);
        }
        else if (sx_str_cmp(type, "register") == 0)
        {
            // verify that player had an account on current device
            char profileid[16] = init;
            if (sx_database_query(&dbase, profileid, 16, "SELECT profile_id FROM authen WHERE (device='%s');", device))
            {

            }

            if (sx_database_query_cb_fmt(&dbase, null, null, "SELECT profile_id FROM authen WHERE (username='%s');", user) < 1)
            {
                sx_database_query(&dbase, "REPLACE INTO authen (username,password,device) VALUES('%s','%s','%s');", user, pass, device);
                char profileid[16] = init;
                if (sx_database_query(&dbase, profileid, 16, "SELECT profile_id FROM authen WHERE (username='%s' AND password='%s');", user, pass))
                    authen_accesscode_send_result(data, userdata, profileid, GIE_NO_ERROR);
                else 
                    authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_USERPASS);
            }
            else authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_USERPASS);
        }
        else authen_accesscode_send_result(data, userdata, "0", GIE_INVALID_DATA);

        sx_database_finalize(&dbase);
    }

    sx_return();
}

static void authen_accesscode_openid(struct authen_task* data, const char* userdata, const char* device, const char* provider, const char* openid)
{
    sx_trace();

    sx_return();
}

static void authen_accesscode_device(struct authen_task* data, const char* userdata, const char* device)
{
    sx_trace();

    sx_database dbase = init;
    if (sx_database_initalize(&dbase, g_gamein.database_config))
    {
        char profileid[16] = init;
        if (sx_database_query(&dbase, profileid, 16, "SELECT profile_id FROM authen WHERE (device='%s');", device) < 1)
        {
            sx_database_query_cb_fmt(&dbase, null, null, "INSERT INTO authen (device) VALUES('%s');", device);
            sx_database_query(&dbase, profileid, 16, "SELECT profile_id FROM authen WHERE (device='%s');", device);
        }        
        authen_accesscode_send_result(data, userdata, profileid, GIE_NO_ERROR);

        sx_database_finalize(&dbase);
    }
    else authen_accesscode_send_result(data, "", "0", GIE_INVALID_DATA);

    sx_return();
}

static void authen_accesscode_thread(void* p)
{
    sx_trace();

    struct authen_task* data = (struct authen_task*)p;

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
    if (json.nodescount == 15)
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