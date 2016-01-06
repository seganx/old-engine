#include "Lib.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


#if defined(_WIN32)
#include <Windows.h>
#endif

extern Library* g_lib = null;

#if ( SEGAN_CRITICAL_SECTION == 1 )

#if defined(_WIN32)
CRITICAL_SECTION	s_mem_cs;
SEGAN_INLINE void lib_init_cs( void )
{
	InitializeCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_finit_cs( void )
{
	DeleteCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_enter_cs( void )
{
	EnterCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_leave_cs( void )
{
	LeaveCriticalSection( &s_mem_cs );
}
#endif

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if SEGAN_CRASHRPT
void memory_callbacl_default(void* userdata, const char* file, const uint line, const uint size, const uint tag, const bool corrupted)
{
	FILE* f = (FILE*)userdata;
	if (corrupted)
		fprintf_s(f, "%s(%d): size = %d - CORRUPTED!\n", file, line, size);
	else
		fprintf_s(f, "%s(%d): size = %d\n", file, line, size);
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
		localtime_s(&timeInfo, &rawTime);
		strftime(currtime, 64, "%Y/%m/%d %H:%M:%S", &timeInfo);
	}

	//	report called functions in the file
	FILE* f = null;
	if (fopen_s(&f, "seganx_crash_report.txt", "a+") == 0)
	{
		fprintf_s(f, "Application crashes on %s\n\n", currtime);

		fprintf_s(f, "Code: %u\n", rc->code);
		fprintf_s(f, "Reason: %s\n\n", sx_crash_translate(rc->reason));
		fflush(f);

#if SEGAN_CRASHRPT_CALLSTACK
		if (rc->callstack->file != null)
		{
			fprintf_s(f, "Available callstack:\n");
			for ( CrashCallStack* cs = rc->callstack; cs->file != null; ++cs )
				fprintf_s(f, "%s(%d): %s\n", cs->file, cs->line, (cs->func ? cs->func : cs->param));
		}
		fprintf_s(f, "\n");
		fflush(f);
#endif
#if SEGAN_MEMLEAK
		fprintf_s(f, "Allocated memory report:\n");
		sx_mem_report_debug(memory_callbacl_default, f, 0);
		fprintf_s(f, "\n");
#endif

		fprintf_s(f, "========================================================================\n");

		fclose(f);
	}

	// just terminate crashed process
#if _DEBUG

#else
	TerminateProcess(GetCurrentProcess(), 0);
#endif
}
#endif



//////////////////////////////////////////////////////////////////////////
//	global singleton class
//////////////////////////////////////////////////////////////////////////
Library::Library(void)
{
	m_timer = sx_new Timer();
	m_randomer = sx_new Randomer();
	m_logger = sx_new Logger();
}

Library* Library::GetSingelton(void)
{
	static Library* instance = null;
	if (instance == null)
		instance = sx_new Library();
	return instance;
}


//////////////////////////////////////////////////////////////////////////
// initialize internal library
void sx_lib_initialize( void )
{
#if SEGAN_CRITICAL_SECTION
	lib_init_cs();
#endif

#if SEGAN_MEMLEAK
	mem_enable_debug( true, 0 );
#endif

#if SEGAN_CRASHRPT_CALLSTACK
	sx_crash_callback(sx_crash_callback_default);
#endif

	//	create basic objects
	g_lib = Library::GetSingelton();
}

// finalize internal library
void sx_lib_finalize( void )
{
	sx_delete_and_null(g_lib->m_logger);
	sx_delete_and_null(g_lib->m_randomer);
	sx_delete_and_null(g_lib->m_timer);
	sx_delete_and_null(g_lib);

	sx_print_a("seganx library finalized!\n");

#if SEGAN_MEMLEAK
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
#endif

#if SEGAN_CRITICAL_SECTION
	lib_finit_cs();
#endif

}

