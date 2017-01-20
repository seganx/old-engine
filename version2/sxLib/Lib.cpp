#include "Lib.h"


#if defined(_WIN32)
#include <Windows.h>
#endif

#if SX_LIB_SINGLETON
extern Library* g_lib = null;
#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if SEGAN_CRASHRPT
void memory_callback_default(void* userdata, const char* file, const uint line, const uint size, const uint tag, const bool corrupted)
{
	FILE* f = (FILE*)userdata;
	if (corrupted)
		sx_fprintf(f, "%s(%d): size = %d - CORRUPTED!\n", file, line, size);
	else
		sx_fprintf(f, "%s(%d): size = %d\n", file, line, size);
	fflush(f);
}

void sx_crash_callback_default(CrashReport* rc)
{
	char currtime[64];

	//	get the current time
	{
		time_t rawTime;
		time(&rawTime);
		struct tm timeInfo;
		sx_localtime(&timeInfo, &rawTime);
		strftime(currtime, 64, "%Y/%m/%d %H:%M:%S", &timeInfo);
	}

	//	report called functions in the file
	FILE* f = fopen("seganx_crash_report.txt", "a+");
	if (f != null)
	{
		sx_fprintf(f, "Application crashes on %s\n\n", currtime);

		sx_fprintf(f, "Code: %u\n", rc->code);
		sx_fprintf(f, "Reason: %s\n\n", sx_crash_translate(rc->reason));
		fflush(f);

#if SEGAN_CRASHRPT_CALLSTACK
		if (rc->callstack->file != null)
		{
			sx_fprintf(f, "Available callstack:\n", "");
			for ( CrashCallStack* cs = rc->callstack; cs->file != null; ++cs )
				sx_fprintf(f, "%s(%d): %s\n", cs->file, cs->line, (cs->func ? cs->func : cs->param));
		}
		sx_fprintf(f, "\n");
		fflush(f);
#endif
#if SEGAN_MEMLEAK
		sx_fprintf(f, "Allocated memory report:\n");
		sx_mem_report_debug(memory_callbacl_default, f, 0);
		sx_fprintf(f, "\n");
#endif

		sx_fprintf(f, "========================================================================\n");

		fclose(f);
	}

	// just terminate crashed process
#if _DEBUG

#else
#if defined(_WIN32)
	TerminateProcess(GetCurrentProcess(), 0);
#else

#endif
#endif
}
#endif

#if 0
SEGAN_LIB_API Bigint<128> sx_power(const uint64& x, const uint64& y)
{
	Bigint<128> res;
	return res.power(x, y);
}
#endif

#if SX_LIB_SINGLETON

//////////////////////////////////////////////////////////////////////////
//	global singleton class
//////////////////////////////////////////////////////////////////////////
Library::Library(void)
{
	m_timer = sx_new Timer();
	m_randomer = sx_new Randomer();
	m_logger = sx_new Logger();
}

Library::~Library(void)
{
    sx_delete_and_null(m_logger);
    sx_delete_and_null(m_randomer);
    sx_delete_and_null(m_timer);
}

Library* Library::GetSingelton(void)
{
	static Library* instance = null;
	if (instance == null)
		instance = sx_new Library();
	return instance;
}
#endif

//////////////////////////////////////////////////////////////////////////
// initialize internal library
void sx_lib_initialize( void )
{
#if SEGAN_MEMLEAK
	mem_enable_debug( true, 0 );
#endif

#if SEGAN_CRASHRPT_CALLSTACK
	sx_crash_callback(sx_crash_callback_default);
#endif

#if SX_LIB_SINGLETON
	//	create basic objects
	g_lib = Library::GetSingelton();
#endif
}

// finalize internal library
void sx_lib_finalize( void )
{
#if SX_LIB_SINGLETON
	sx_delete_and_null(g_lib);
#endif

	sx_print("seganx library finalized!\n");

#if SEGAN_MEMLEAK
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
#endif
}

