#include "plugin_manager.h"
#include "plugin.h"

//	we have to define new type to use reference to pointer in the sort function
typedef Plugin* PMPluginPointer;
int CompPluginPriority(const PMPluginPointer& p1, const PMPluginPointer& p2)
{
	return p1->m_priority - p2->m_priority;
}


//////////////////////////////////////////////////////////////////////////
//	Plugin manager
//////////////////////////////////////////////////////////////////////////
PluginMan::PluginMan()
{

}

PluginMan::~PluginMan()
{
	//	remove all plugins from the system
	while (m_plugins.m_count)
	{
		sx_delete_and_null(m_plugins[0]);
		m_plugins.remove_index(0);
	}
}

Plugin* PluginMan::add(const wchar* filename)
{
	//	try to load new plugin
	Plugin* p = sx_new Plugin();
	int res = p->load(filename);
	if (!res)
	{
		sx_delete_and_null(p);
		return null;
	}

	//	verify that plugin is not exist in the system
	for (int i = 0; i < m_plugins.m_count; ++i)
	{
		if (sx_str_cmp(p->m_name, m_plugins[i]->m_name) == 0)
		{
			sx_print(L"Error: Plugin %S already loaded! Discard loading %s", p->m_name, filename);
			sx_delete_and_null(p);
			return m_plugins[i];
		}
	}

	//	add new plugin to the system
	m_mutex.lock();
	m_plugins.push_back(p);
	m_plugins.sort(CompPluginPriority);
	m_mutex.unlock();

	sx_print_a("Plugin %s has been added to system", p->m_name);
}

void PluginMan::remove(const wchar* name)
{
	//	verify that there is a plugin with given name
	for (int i = 0; i < m_plugins.m_count; ++i)
	{
		Plugin* p = m_plugins[i];
		if (sx_str_cmp(name, p->m_name) == 0)
		{
			//	remove plugin from the list
			m_mutex.lock();
			m_plugins.remove_index(i);
			m_mutex.unlock();

			//	delete the plugin object from memory
			sx_print_a("Plugin %s has been deleted.", p->m_name);
			sx_delete_and_null(p);
			return;
		}
	}
}

int PluginMan::count()
{
	return m_plugins.m_count;
}

Plugin* PluginMan::get(const int index)
{
	if (index < 0 || index >= m_plugins.m_count)
	{
		sx_print_a("Error: Plugin index is out of range. index:%d, count:%d", index, m_plugins.m_count);
		return null;
	}
	return m_plugins[index];
}

void PluginMan::get_plugins(Plugin* dest[], const int destSize)
{
	m_mutex.lock();

	for (int i = 0, d = 0; i < m_plugins.m_count; ++i)
	{
		if (d >= destSize) break;	// take care of destination size

		Plugin* p = m_plugins[i];
		if (p->m_active)
			dest[d++] = p;
	}

	m_mutex.unlock();
}

