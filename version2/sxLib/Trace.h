/********************************************************************
    created:	2017/02/03
    filename: 	Trace.h
    Author:		Sajad Beigjani
    eMail:		sajad.b@gmail.com
    Site:		www.segans.com
    Desc:		This file contain some macro and functions for
                profiling, stack tracing and crash handling
*********************************************************************/
#ifndef HEADER_TRACE
#define HEADER_TRACE

#include "Def.h"

#if (SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)

#define sx_trace()                          { trace_push( __FILE__, __LINE__, __FUNCTION__ ); }
#define sx_trace_param(function,...)        { trace_push_param( __FILE__, __LINE__, #function, __VA_ARGS__ ); }
#define sx_return(expression)               { trace_pop(); return expression; }

#if __cplusplus
extern "C" {
#endif

SEGAN_LIB_API void trace_attach( uint stack_size );
SEGAN_LIB_API void trace_detach( void );
SEGAN_LIB_API void trace_push( const char* file, const int line, const char* function );
SEGAN_LIB_API void trace_push_param( const char* file, const int line, const char* function, ... );
SEGAN_LIB_API void trace_pop( void );

#if __cplusplus
}
#endif


#else

#define sx_trace
#define sx_trace_param(function,...)
#define sx_return(expression)               return

#endif  //  SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER

#endif