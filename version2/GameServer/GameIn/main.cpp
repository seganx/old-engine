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
