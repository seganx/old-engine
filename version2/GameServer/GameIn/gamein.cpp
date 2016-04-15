#include "mongoose.h"
#include "gamein.h"
#include "plugin.h"
#include "plugin_manager.h"


//////////////////////////////////////////////////////////////////////////
//	HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////
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

PluginMan* plugin_manager_start(const wchar* config)
{
	PluginMan* pm = sx_new PluginMan();

	sx_print_a("Loading plugins :");
	while (true)
	{
		config = sx_str_get_value(config, L"pm_plugin");
		if (config)
		{
			wchar pname[128] = { 0 };
			for (int i = 0; i < 128 && *config != '\n' && *config != '\0'; ++i)
				pname[i] = *config++;
			pm->add(pname);
		}
		else break;
	}

	if (pm->count() > 0)
	{
		sx_print_a("\nTotal plugin(s) loaded: %d", pm->count());
		for (int i = 0; i < pm->count(); ++i)
			sx_print_a("\nPriority: %d\nName: %s\nDesc: %s", pm->get(i)->m_priority, pm->get(i)->m_name, pm->get(i)->m_desc);
	}
	else sx_print_a("No plugin loaded to the system!");

	return pm;
}

mg_context* mongoose_start(struct mg_callbacks* mgcallbacks, void* userdata, const wchar* config)
{
	// define basic options
	char lport[8] = "8000";
	char numtr[8] = "10";
	char timeo[8] = "3000";

	//	load mongoose options
	sx_str_get_value(lport, 8, config, L"mg_listening_port");
	sx_str_get_value(numtr, 8, config, L"mg_num_threads");
	sx_str_get_value(timeo, 8, config, L"mg_request_timeout_ms");

	//	construct mongoose options
	const char* options[] = {
		"listening_ports", lport,
		"num_threads", numtr,
		"request_timeout_ms", timeo,
		"enable_directory_listing", "no",
		"authentication_domain", "",
		"document_root", "www_root",
		NULL
	};

	//	start mongoose server
	mg_context* res = mg_start(mgcallbacks, userdata, options);
	if (res)
	{
		sx_print_a("\nMongoose server started on port %s", lport);
	}
	else
	{
		sx_print_a("\nMongoose server failed to started on port %s", lport);
	}
	return res;
}



//////////////////////////////////////////////////////////////////////////
//	GAMEIN SERVER
//////////////////////////////////////////////////////////////////////////
GameIn::GameIn() : m_mongoose(null), m_plugins(null)
{

}

GameIn::~GameIn()
{
	// Stop the server.
	mg_stop(m_mongoose);
	m_mongoose = null;

	sx_delete_and_null(m_plugins);
}

void GameIn::start(const wchar* configFile, struct mg_callbacks* mgcallbacks)
{
	//	try to load configuration file
	{
		String config;
		if (sx_load_string(config, configFile))
		{
			//	load plugin manager
			m_plugins = plugin_manager_start(config);

			//	start the mongoose server
			m_mongoose = mongoose_start(mgcallbacks, this, config);
		}
		else
		{
			sx_print_a("Failed to load configuration file %s", configFile);
			getchar();
			return;
		}
	}

	while (true)
	{
		char command[300] = { 0 };
		int len = gi_get_command(command);

		if (sx_str_cmp(command, "exit") == 0)
		{
			printf("are you sure (yes,no)? ");
			len = gi_get_command(command);
			if (sx_str_cmp(command, "yes") == 0)
			{
				printf("shutting down ...\n");
				break;
			}
		}
		else if (*command)
		{
			//	send command via plugins
			for (int i = 0; i < m_plugins->count(); ++i)
				m_plugins->get(i)->process_msg(GAMEIN_PLUGIN_COMMAND, command);
		}
	}
}


