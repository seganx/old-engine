#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "mongoose.h"
#include "mongoose_config.h"
#include "plugin.h"

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


int begin_request(struct mg_connection * conn)
{
	sx_callstack();

	mg_request_info * header = mg_get_request_info(conn);
	if (!header->uri) return 1;
	if (sx_str_len(header->uri) > 256) return 1;

	//	place codes here


	return 0;
}
void callback_end_request(const struct mg_connection *, int reply_status_code){}
void callback_upload(struct mg_connection * conn, const char *file_name) {  }
int callback_websocket(const struct mg_connection * conn) { return 1; }
const char* callback_open_file(const struct mg_connection * conn, const char *path, size_t *data_len) { return 0; }

int get_command(char* command)
{
	int i = 0;
	while (true)
	{
		if ((command[i++] = getchar()) == 10)
		{
			command[--i] = 0;
			return i;
		}
	};
}

int main(void)
{
	{
		Plugin* p = sx_new Plugin();
		p->Load(L"Test.dll");
		if (p->m_module)
			sx_print(L"Plugin %s has priority %d", p->m_name, p->m_priority);
		sx_delete_and_null(p);
	}

	sx_callstack();
	printf("GameIn web api server version 0.1\n\n");

	// load options
	char* options[MG_MAX_OPTIONS] = { 0 };
	if (!mg_load_command_line_arguments(options))
	{
		printf("fatal error : can't find gamein.config\n");
		if (mg_create_config_file(MG_CONFIG_FILE))
			printf("info : gameup.config has been created.\n\nplease modify that file and start the server again\n");
		else
			printf("fatal error : can't create gameup.config\n");
		getchar();
		return 0;
	}

	// prepare callbacks structure for mongoose
	struct mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = &begin_request;
	callbacks.end_request = &callback_end_request;
	callbacks.open_file = &callback_open_file;
	callbacks.log_message = &callback_log;
	callbacks.upload = &callback_upload;
	callbacks.websocket_connect = &callback_websocket;

	// start the web server.
	mg_context* con = mg_start(&callbacks, NULL, (const char**)options);

	while (true)
	{
		char command[300] = { 0 };
		int len = get_command(command);

		if (strcmp(command, "exit") == 0)
		{
			printf("are you sure (yes,no)? ");
			len = get_command(command);
			if (strcmp(command, "yes") == 0)
			{
				printf("shutting down ...\n");
				break;
			}
		}
	}

	// Stop the server.
	mg_stop(con);

	return 0;
}
