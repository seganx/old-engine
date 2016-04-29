#include <process.h>
#include <windows.h>
#include <stdio.h>

#include "../plugin.h"
#include "../../request.h"
#include "../../imports.h"

#include "authenticator.h"

void worker_authen(void* user_data)
{
	while (g_authen)
	{
		g_authen->Update();
		Sleep(1000);
	}	
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
				if (strcmp( cmd, "Authentication" ) == 0)
				{
					printf("enter the command for plugin Authentication: ");
					char command[256] = {0};
					int len = gi_get_command(command);
					if ( sx_str_cmp(command, "list all") == 0 )
					{
						g_authen->m_mutex_keys.lock();
						for (uint i = 0; i < g_authen->m_keys.m_size; ++i )
							if ( g_authen->m_keys.m_slots[i] )
							{
								AuthenKeys* ak = g_authen->m_keys.m_slots[i];
								printf( "id:%u key:%.*s time:%u\n", ak->id, 16, ak->gnrt_key, ak->time_out );
							}
						g_authen->m_mutex_keys.unlock();
					}
				}
				
			}
			return 1;

		case GAMEIN_PLUGIN_REQUEST:
			if (data)
			{
				Request* req = (Request*)data;
				if ( sx_str_cmp(req->uri, "/authen") == 0 )
				{
					if ( req->size != 37 ) return 1;
					req->data[37] = 0; // form it as a null terminated string

					uint gameid = sx_str_get_value_uint(req->data, "game_id", 0);
					if ( gameid == 0 ) return 1;
					
					const char* secretkey = g_authen->get_key_of_game( gameid );
					if ( secretkey == null ) return 1;
					
					const char* reqkey = sx_str_get_value( req->data, "req_key" );
					if ( reqkey == null ) return 1;
					if ( sx_str_len( reqkey ) != 17 ) return 1;
					srand( (uint)sx_time_counter() );

					// create a authentication session key for the request
					AuthenKeys* ak = (AuthenKeys*)sx_mem_alloc( sizeof(AuthenKeys) );
					ak->time_out = g_authen->m_authen_timeout;
					sx_mem_copy( ak->game_key, secretkey, 16 );
					sx_mem_copy( ak->recv_key, reqkey, 16 );
					sx_hash_write_index( ak->gnrt_key, 16, sx_random_i_limit(0, 9) );

					//	add the authentication key to the table
					g_authen->m_mutex_keys.lock();
					ak->id = g_authen->m_keys.add( ak );
					g_authen->m_mutex_keys.unlock();

					//  send session id and generated key to the client
					char sid[16];
					sx_hash_write_index( sid, 16, ak->id );
					
					char msg[128] = {0};
					int len = sprintf_s( msg, 128, "{sid:%.*s,key:%.*s}", 16, sid, 16, ak->gnrt_key );
					req->send( req->connection, msg, len );

					return 1;
				}
				else
				{
				}
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
			if ( g_authen == null )
			{
				g_authen = sx_new Authenticator();
				_beginthread( worker_authen, 0, null );
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
