/********************************************************************
	created:	2013/01/18
	filename: 	Callstack.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some macro and functions for 
				call stack system
*********************************************************************/
#ifndef GUARD_Callstack_HEADER_FILE
#define GUARD_Callstack_HEADER_FILE


#include "Def.h"

#if SEGAN_CALLSTACK

class SEGAN_LIB_API _CallStack
{
public:
	_CallStack( const wchar* file, const sint line, const wchar* function );
	_CallStack( const sint line, const wchar* file, const wchar* function, ... );
	~_CallStack( void );
};

//! callback function for call stack
typedef void (*CB_CallStack)( const wchar* file, const sint line, const wchar* function );
SEGAN_LIB_API void callstack_report_to_file( const wchar* name, const wchar* title = L" " );
SEGAN_LIB_API void callstack_report( CB_CallStack callback );
SEGAN_LIB_API void callstack_clear( void );
SEGAN_LIB_API void detect_crash(void);


//! create a new call stack for function with out parameters
#define sx_callstack()								_CallStack sx_unique_name(callstack)( _CRT_WIDE(__FILE__), __LINE__, _CRT_WIDE(__FUNCTION__) )

//! create new call stack for function with name and parameters
#define sx_callstack_param(function,...)			_CallStack sx_unique_name(callstack)( __LINE__, _CRT_WIDE(__FILE__), _CRT_WIDE(#function), __VA_ARGS__ )

//! report call stack to a file
#define sx_callstack_report_to_file(name, tag)		callstack_report_to_file( name, tag )

//! report call stack to call back function
#define sx_callstack_report(callback)				callstack_report( callback )

//! clear call stack reports
#define sx_callstack_clear()						callstack_clear()

//! just place it at the end of the application to detect crash and report call stacks
#define sx_detect_crash()							detect_crash()

#else

#define sx_callstack()
#define sx_callstack_param(function,...)
#define sx_callstack_report_to_file(name, tag)
#define sx_callstack_report(callback)
#define sx_callstack_clear()
#define sx_detect_crash()

#endif

#endif	//	GUARD_Callstack_HEADER_FILE
