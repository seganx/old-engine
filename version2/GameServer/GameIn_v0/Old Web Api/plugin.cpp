#include <windows.h>
#include "plugin.h"

Plugin::Plugin(): m_priority(0), m_module(null), m_func(null)
{
	sx_mem_set(m_name, 0, sizeof(m_name));
	sx_mem_set(m_desc, 0, sizeof(m_desc));
}

Plugin::~Plugin()
{
	if (m_module)
		FreeLibrary((HMODULE)m_module);
}

int Plugin::load(const wchar* filename)
{
	if (m_module)
	{
		char name[256] = {0};
		process_msg(GAMEIN_PLUGIN_NAME, name);
		sx_print(L"Error: Plugin %S already loaded! Discard loading %s", name, filename);
		return 0;
	}

	//	load library
	m_module = LoadLibrary(filename);
	if (!m_module) 
	{
		sx_print(L"Error: Can't load plugin %s due to code: %u", filename, GetLastError());
		return 0;
	}

	//	load functions
	m_func = (pfunc)GetProcAddress((HMODULE)m_module, "_process_msg@8");
	if (!m_func) 
	{
		sx_print(L"Error: Can't load function 'process_msg' from plugin %s!", filename);

		m_func = null;
		if (m_module)
			FreeLibrary((HMODULE)m_module);
		m_module = null;

		return 0;
	}

	//	get plugin name
	m_func(GAMEIN_PLUGIN_NAME, m_name);
	m_func(GAMEIN_PLUGIN_DESC, m_desc);

	//	get plugin priority
	m_priority = m_func(GAMEIN_PLUGIN_PRIORITY, null);

	return 1;
}

int Plugin::process_msg(GAMEIN_PLUGIN_ int msg, void* data)
{
	if (m_module)
	{
		return m_func(msg, data);
	}
	else
	{
		sx_print_a("Plugin not loaded! Class pointer: 0x%x", (uint)this)
		return 0;
	}
}
