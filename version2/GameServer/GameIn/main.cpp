#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "mongoose.h"
#include "plugin.h"
#include "plugin_manager.h"
#include "gamein.h"
#include "request.h"


int callback_request_send(void* connection, const void* buffer, int size)
{
	struct mg_connection* conn = (struct mg_connection*)connection;

	//////////////////////////////////////////////////////////////////////////
	//	this block can't be here because each plugin will send header separately
	//////////////////////////////////////////////////////////////////////////
	if (false)
	{
		//	prepare HTTP header
		char* header =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/x-zip-compressed\r\n"
			"Accept-Ranges: bytes\r\n"
			"Content-Length: %u\r\n\r\n";

		//	prepare message and send
		mg_printf(conn, header, size);
	}

	return mg_write(conn, buffer, size);
}

void callback_upload(struct mg_connection * conn, const char *file_name) {  }
int callback_websocket(const struct mg_connection * conn) { return 1; }
const char* callback_open_file(const struct mg_connection * conn, const char *path, size_t *data_len) { return 0; }

int callback_log(const struct mg_connection * conn, const char *message)
{
	sx_callstack();
	if (conn)
	{
		struct _ip{ byte p1; byte p2; byte p3; byte p4; } ip = *((_ip*)&mg_get_request_info((mg_connection*)conn)->remote_ip);
		sx_print_a("connection '%u.%u.%u.%u' : %s\n", ip.p4, ip.p3, ip.p2, ip.p1, message);
	}
	else sx_print_a(message);
	return 0;
}


int callback_begin_request(struct mg_connection * conn)
{
	sx_callstack();

	mg_request_info * header = mg_get_request_info(conn);
	if (!header->uri) return 1;
	if (sx_str_len(header->uri) > 256) return 1;
	if (!header->user_data) return 1;

	//	get server object as user data to access all parts
	GameIn* gamein = (GameIn*)header->user_data;

	//	collect activated plugins from plugin manager
	if (gamein->m_plugins->count())
	{
		Plugin* activePlugins[32] = { 0 };
		gamein->m_plugins->get_plugins(activePlugins, 32);

		//	prepare request message to throw in all plugins
		Request reqobj;
		reqobj.uri = header->uri;
		reqobj.size = mg_read(conn, reqobj.data, sizeof(reqobj.data));
		reqobj.handled = false;
		reqobj.player = null;
		reqobj.connection = conn;
		reqobj.send = callback_request_send;

		//	count on all plugins to handle the request 
		for (int i = 0; i < 32 && activePlugins[i]; ++i)
			if (activePlugins[i]->process_msg(GAMEIN_PLUGIN_REQUEST, &reqobj))
				break;
	}
	return 1;
}

void callback_end_request(const struct mg_connection *, int reply_status_code)
{

}

#define dfhlm_buflen	32
#define dfhlm_g			7
#define dfhlm_p			23

struct diffiehellman
{
	char secret_key[dfhlm_buflen + 1];
	char public_key[dfhlm_buflen + 1];
	char final_key[dfhlm_buflen + 1];
};


int main(void)
{
	sx_callstack();
	sx_randomize((uint)sx_time_counter());

	if (false)
	{
		diffiehellman bob;	sx_mem_set(&bob, 0, sizeof(bob));
		diffiehellman alc;	sx_mem_set(&alc, 0, sizeof(alc));

		sx_dh_secret_Key(bob.secret_key, dfhlm_buflen);
		sx_dh_secret_Key(alc.secret_key, dfhlm_buflen);
		printf("generated secret key:\nbob: %s\nalc: %s\n\n", bob.secret_key, alc.secret_key);

		sx_dh_public_key(bob.public_key, bob.secret_key, dfhlm_buflen, dfhlm_g, dfhlm_p);
		sx_dh_public_key(alc.public_key, alc.secret_key, dfhlm_buflen, dfhlm_g, dfhlm_p);
		printf("public key for share:\nbob: %s\nalc: %s\n\n", bob.public_key, alc.public_key);

		sx_dh_final_key(bob.final_key, bob.secret_key, alc.public_key, dfhlm_buflen, dfhlm_p);
		sx_dh_final_key(alc.final_key, alc.secret_key, bob.public_key, dfhlm_buflen, dfhlm_p);
		printf("final key:\nbob: %s\nalc: %s\n", bob.final_key, alc.final_key);

		printf("\ncompare final keys: %d\n", sx_mem_cmp(bob.final_key, alc.final_key, sizeof(bob.final_key)));

		uint ch = sx_checksum(bob.final_key, 64);
		printf("\nencryption key: %u\n", ch);
		getchar();
	}

	if (false)
	{
		byte data[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
		byte dest[10] = { 0 };
		byte finl[10] = { 0 };

		printf("ch: %u\n", sx_checksum(data, 10, 1363));

		sx_encrypt(dest, data, 10, 1363);
		for (int i = 0; i < 10; ++i) printf("%4d ", dest[i]); printf("\n");
		sx_decrypt(finl, dest, 10, 1363);
		for (int i = 0; i < 10; ++i) printf("%4d ", finl[i]); printf("\n");

		sx_encrypt(dest, data, 10, 123456789);
		for (int i = 0; i < 10; ++i) printf("%4d ", dest[i]); printf("\n");
		sx_decrypt(finl, dest, 10, 123456789);
		for (int i = 0; i < 10; ++i) printf("%4d ", finl[i]); printf("\n");

		sx_encrypt(dest, data, 10, 131234563);
		for (int i = 0; i < 10; ++i)
			printf("%4d ", dest[i]);
		printf("\n");
	}

	for (int i = 0; i < 0; ++i)
	{
		char c[25] = { 0 };
		int r = sx_random_i_limit(0, 999999999);
		printf("%9u ", r);
		sx_hash_write_index(c, 24, r);
		printf("%s ", c);
		int indx = sx_hash_read_index(c, 0);
		printf("%u\n", indx);
		Sleep(10);
	}

	if (false)
	{
		const char *json_string_test = "{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000, \"groups\": [{\"n\":11, \"m\":12}, {\"n\":21, \"m\":22}]}";
		Json json;
		json.parse(json_string_test);
		json.print();
		printf("%d\n", json.find("groups")->childs);
	}

	printf("GameIn web-api server version 0.1\n\n");

	// prepare callbacks structure for mongoose
	struct mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = &callback_begin_request;
	callbacks.end_request = &callback_end_request;
	callbacks.open_file = &callback_open_file;
	callbacks.log_message = &callback_log;
	callbacks.upload = &callback_upload;
	callbacks.websocket_connect = &callback_websocket;

	GameIn gin;
	gin.start(L"gamein.config", &callbacks);

	return 0;
			}
