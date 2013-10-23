#include "Lib.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

extern bool	callstack_end;


#if ( SEGAN_CRITICAL_SECTION == 1 )

#if defined(_WIN32)
#include <Windows.h>
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

#if SEGAN_CALLSTACK
	callstack_clear();
#endif

}

// finalize internal library
void sx_lib_finalize( void )
{

#if SEGAN_CALLSTACK
	if ( !callstack_end )	//	verify that application crashed
	{
		callstack_report_to_file( L"sx_crash_report", L"application closed unexpectedly !" );

#if SEGAN_MEMLEAK
		mem_report_debug_to_file( L"sx_mem_corruption_report.txt", -1 );
		mem_clear_debug();
#endif

	}
#if SEGAN_MEMLEAK
	else
	{
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
	}
#endif

	callstack_clear();
#elif SEGAN_MEMLEAK
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
#endif

#if SEGAN_CRITICAL_SECTION
	lib_finit_cs();
#endif
}

