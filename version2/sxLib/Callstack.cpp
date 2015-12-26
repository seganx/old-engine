#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "Callstack.h"
#include "Assert.h"



#if ( defined(_DEBUG) || SEGAN_CALLSTACK || SEGAN_ASSERT )


SEGAN_INLINE sint lib_assert( const char* expression, const char* file, const sint line )
{

#if ( SEGAN_CALLSTACK == 1 )
	_CallStack _callstack( line, file, "assertion '%s'", expression );
#endif

#if defined(_DEBUG)
	__debugbreak();	//	just move your eyes down and look at the call stack list in IDE to find out what happened !
#elif ( SEGAN_CALLSTACK == 1 )
	callstack_report_to_file( L"sx_assertion", L"assertion failed !" );
#endif

	return 0;
}

#endif



#if SEGAN_CALLSTACK

//////////////////////////////////////////////////////////////////////////
//	simple call stack system
//////////////////////////////////////////////////////////////////////////
/* call stack system should be safe. o memory allocation or using 
any other service so we need a pool and fill it consecutively. */
#define CALLSTACK_MAX	128

struct CallStackData
{
	char	name[512];
	char*	function;
	char*	file;
	sint	line;
};
CallStackData	callstack_pool[CALLSTACK_MAX];
uint			callstack_index = 0;
extern bool		callstack_end	= false;

SEGAN_INLINE void callstack_clean(CallStackData *csd)
{
	csd->file = null;
	csd->line = 0;
	csd->name[0] = 0;
	csd->function = null;
}

CallStackData* callstack_push_pop(bool push)
{
	sx_enter_cs();
	CallStackData* res = null;

	if ( push )
	{
		if ( callstack_index > 0 )
			callstack_clean( &callstack_pool[--callstack_index] );
	}
	else 
	{
		if (callstack_index < CALLSTACK_MAX)
			res = &callstack_pool[callstack_index++];
	}

	sx_leave_cs();
	return res;
}

SEGAN_INLINE _CallStack::_CallStack( const char* file, const sint line, const char* callfunction )
{
	CallStackData* csd = callstack_push_pop(false);
	if ( csd )
	{
		csd->file = (char*)file;
		csd->line = line;
		csd->name[0] = 0;
		if ( callfunction )
			csd->function = (char*)callfunction;
		else
			csd->function = null;
	}
}

SEGAN_INLINE _CallStack::_CallStack( const sint line, const char* file, const char* callfunction, ... )
{
	CallStackData* csd = callstack_push_pop(false);
	if (csd)
	{
		csd->file = (char*)file;
		csd->line = line;
		csd->function = null;
		if ( callfunction )
		{
			va_list argList;
			va_start( argList, callfunction );
			sint len = _vscprintf( callfunction, argList);
			if ( len < 1023 )
			{
				vsprintf_s( csd->name, 1023, callfunction, argList );
			}
			else
			{
				uint i = 0;
				for ( ; i<1022 && callfunction[i]; ++i )
					csd->name[i] = callfunction[i];
				csd->name[i] = 0;
			}
			va_end( argList );
		}
		else csd->name[0] = 0;
	}
}

SEGAN_INLINE _CallStack::~_CallStack( void )
{
	callstack_push_pop(true);
}

void callstack_report( CB_CallStack callback )
{
	if ( !callback ) return;

	for ( int i=0; i<CALLSTACK_MAX; i++ )
	{
		CallStackData* csd = &callstack_pool[i];
		if ( csd->line && csd->file )
			(*callback)( csd->file, csd->line, csd->function ? csd->function : csd->name );
	}
}

void callstack_clear( void )
{
	for ( int i=0; i<CALLSTACK_MAX; i++ )
		callstack_clean( &callstack_pool[i] );
}

void detect_crash( void )
{
	callstack_end = true;
}

void callstack_report_to_file( const wchar* name, const wchar* title /*= L" "*/ )
{
	if ( callstack_pool[0].line < 1 || callstack_pool[0].file == null ) return;

	sx_enter_cs();

	wchar strName[128];

	//	make a name
	{
		time_t rawTime;
		time( &rawTime );
		struct tm timeInfo;
		localtime_s( &timeInfo, &rawTime );
		wchar strTime[64];
		wcsftime( strTime, 64, L"%H_%M_%S", &timeInfo );
		static int logCount = 0;
		swprintf_s( strName, 128, L"%s_%s_%d.txt", name, strTime, ++logCount );
	}

	//	report called functions in the file
	FILE* f = null;
	if( _wfopen_s( &f, strName, L"w, ccs=UNICODE" ) == 0 )
	{
		fwprintf( f, L"seganx call stack report : %s\n\n", title );

		//	compute maximum length of every line
		sint maxlength = 0;
		for ( int i=0; i<CALLSTACK_MAX; i++ )
		{
			CallStackData* csd = &callstack_pool[i];
			if ( csd->line && csd->file )
			{
				wchar tmp[1024] = {0};
				const sint len = swprintf_s( tmp, 1024, L"%S(%d) : ", csd->file, csd->line );
				if ( maxlength < len )
					maxlength = len;
			}
			else break;
		}

		//	write lines to the file
		for ( int i=0; i<CALLSTACK_MAX; i++ )
		{
			CallStackData* csd = &callstack_pool[i];
			if ( csd->line && csd->file )
			{
				wchar tmp[1024] = {0};
				sint len = swprintf_s( tmp, 1024, L"%S(%d) : ", csd->file, csd->line );

				{
					sint spaces = maxlength + 5 - len;
					for ( sint i=0; i < spaces; ++i )
						tmp[len + i] = ' ';
					len += spaces;
				}

				char* c = null;
				if ( csd->function )
					c = csd->function;
				else if ( csd->name[0] )
					c = csd->name;
				else
					c = "no name !";

				while ( *c && len < 1022 )
					tmp[len++] = *c++;
				tmp[len++] = '\n';
				tmp[len] = 0;

				fputws( tmp, f );
			}
			else break;
		}
		fclose( f );
	}

	sx_leave_cs();
}


#endif


