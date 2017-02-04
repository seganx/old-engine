#include "Assert.h"
#include "Crash.h"


#if ( defined(_DEBUG) || SEGAN_ASSERT )

SEGAN_INLINE int lib_assert( const char* expression, const char* file, const int line )
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