#include <windows.h>
#include "plugin.h"

typedef	int(__cdecl * pGetPriority)(void);
typedef	int(__cdecl * pInitialize)(void);
typedef	int(__cdecl * pFinalize)(void);
typedef	int(__cdecl * pReset)(void);
typedef	int(__cdecl * pHandleRequest)(class RequestObject* request);

//////////////////////////////////////////////////////////////////////////
//	helper function
//////////////////////////////////////////////////////////////////////////
int loading_function_error(Plugin* p, const wchar* filename, const char* func)
{
	sx_print(L"Error: Can't load function '%S' from plugin %s!", func, filename);

	p->Initialize = null;
	p->Finalize = null;
	p->Reset = null;
	p->HandleRequest = null;
	if (p->m_module)
		FreeLibrary((HMODULE)p->m_module);
	p->m_module = null;

	return 0;
}

Plugin::Plugin()
{
	sx_mem_set(m_name, 0, sizeof(m_name));
}

Plugin::~Plugin()
{
	if (m_module)
		FreeLibrary((HMODULE)m_module);
}

int Plugin::Load(const wchar* filename)
{
	if (m_module)
	{
		sx_print(L"Error: Plugin %s already loaded! Discard loading %s", m_name, filename);
		return 0;
	}

	//	load library
	m_module = LoadLibrary(filename);
	if (!m_module) 
	{
		sx_print(L"Error: Can't load plugin %s du to %u", filename, GetLastError());
		return 0;
	}

	//	load functions
	Initialize = (pInitialize)GetProcAddress((HMODULE)m_module, "initialize");
	if (!Initialize) return loading_function_error(this, filename, "initialize");

	Finalize = (pFinalize)GetProcAddress((HMODULE)m_module, "finalize");
	if (!Finalize) return loading_function_error(this, filename, "finalize");

	Reset = (pReset)GetProcAddress((HMODULE)m_module, "reset");
	if (!Reset) return loading_function_error(this, filename, "reset");

	HandleRequest = (pHandleRequest)GetProcAddress((HMODULE)m_module, "handle_request");
	if (!HandleRequest) return loading_function_error(this, filename, "handle_request");

	//	load and set priority
	pGetPriority func_priority = (pGetPriority)GetProcAddress((HMODULE)m_module, "get_priority");
	if (!func_priority) return loading_function_error(this, filename, "get_priority");
	m_priority = func_priority();

	//	finally load name
	const wchar* name = sx_str_exclude_extension(sx_str_extract_filename(filename));
	sx_str_copy(m_name, 128, name);
}
