#include <process.h>
#include <windows.h>
#include <stdio.h>

#include "../plugin.h"
#include "../../request.h"
#include "../../imports.h"

#include "authenticator.h"


// worker thread used to update time of all sessions
void worker_authen(void* user_data)
{
	while (g_authen)
	{
		g_authen->update();
		Sleep(1000);
	}
}

int handle_authen(Request* req, const uint user_data)
{
	char rcvdkey[diffie_hellman_l] = { 0 };
	if (sx_str_get_value(rcvdkey, diffie_hellman_l, req->data, "public_key") == false)
		return 1;

	// validate received key
	{
		const int hirange = 65 + diffie_hellman_p;
		for (int i = 0; i < diffie_hellman_l; ++i)
			if (sx_between_i(rcvdkey[i], 65, hirange) == false)
				return 1;
	}

	//////////////////////////////////////////////////////////////////////////
	// setup Diffie-Hellman keys to start communication
	//////////////////////////////////////////////////////////////////////////
	sx_randomize((uint)sx_time_counter());

	// generate secret key
	char secret_key[diffie_hellman_l] = { 0 };
	sx_dh_secret_Key(secret_key, diffie_hellman_l);

	// generate public key based on secret key
	char public_key[diffie_hellman_l] = { 0 };
	sx_dh_public_key(public_key, secret_key, diffie_hellman_l, diffie_hellman_g, diffie_hellman_p);

	char final_key[diffie_hellman_l] = { 0 };
	sx_dh_final_key(final_key, secret_key, rcvdkey, diffie_hellman_l, diffie_hellman_p);

	// create a session for the request
	AuthenSession* as = (AuthenSession*)sx_mem_alloc(sizeof(AuthenSession));
	as->time_out = g_authen->m_authen_timeout;
	as->access_key = sx_checksum(final_key, diffie_hellman_l);

	//	add the authentication key to the table
	g_authen->m_mutex.lock();
	as->session_id = g_authen->m_sessions.add(as);
	g_authen->m_mutex.unlock();

	//  send session id and public key to the client
	char sid[16];
	sx_hash_write_index(sid, 16, as->session_id);
	as->session_id_chk = sx_checksum(sid, 16, session_checksum);

	char msg[128] = { 0 };
	int len = sprintf_s(msg, 128, "{\"user_data\":%u,\"id\":\"%.*s\",\"key\":\"%.*s\"}", user_data, 16, sid, diffie_hellman_l, public_key);
	req->send(req->connection, msg, len);

	return 1;
}

int handle_access(Request* req)
{
	uint sid = sx_hash_read_index(&req->uri[1], 0);
	if ( sid < 1 ) return 1;

	//  retrieve session from table
	g_authen->m_mutex.lock();
	AuthenSession* as = g_authen->m_sessions.get(sid);
	g_authen->m_mutex.unlock();
	if (as == null) return 1;

	//	verify that request URI is a real session id
	if (as->session_id_chk != sx_checksum(&req->uri[1], 16, session_checksum))
		return 1;

	//	update session time
	as->time_out = g_authen->m_access_timeout;

	req->send(req->connection, "Hello", 6);

	return 0;
}

int gi_get_command(char* command)
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

int __stdcall process_msg(int msg, void* data)
{
	switch (msg)
	{
		case GAMEIN_PLUGIN_INITIALIZE:
			return 1;

		case GAMEIN_PLUGIN_FINALIZE:
			return 1;

		case GAMEIN_PLUGIN_NAME:
			strcpy_s((char*)data, 64, "Authentication v1");
			return 1;

		case GAMEIN_PLUGIN_DESC:
			strcpy_s((char*)data, 256, "Authenticate any request comes from client.");
			return 1;

		case GAMEIN_PLUGIN_PRIORITY:
			return 0;

		case GAMEIN_PLUGIN_COMMAND:
			if (data)
			{
				char* cmd = (char*)data;
				if (strcmp(cmd, "Authentication") == 0)
				{
					printf("enter the command for plugin Authentication: ");
					char command[256] = { 0 };
					int len = gi_get_command(command);
					if (sx_str_cmp(command, "list all") == 0)
						g_authen->print_keys();
				}

			}
			return 1;

		case GAMEIN_PLUGIN_REQUEST:
			if (data)
			{
				Request* req = (Request*)data;
				if (sx_str_cmp(req->uri, "/authen") == 0)
				{
					uint userdata = sx_str_get_value_uint(req->data, "user_data", 0);
					if ( userdata )
						return handle_authen(req, userdata);
					else
						return 1;
				}
				else if ( sx_str_len(req->uri) == 17 )
					return handle_access(req);
				else
					return 1;
			}
			return 0;

	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	DLL main function
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			if (g_authen == null)
			{
				g_authen = sx_new Authenticator();
				_beginthread(worker_authen, 0, null);
			}
			break;

		case DLL_PROCESS_DETACH:
			sx_delete_and_null(g_authen);
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
