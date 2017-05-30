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

#if SEGANX_TRACE_ASSERT

#define sx_assert(expression)	((expression) ? (void) (0) : trace_assert(#expression, __FILE__, __LINE__))

#if __cplusplus
extern "C" {
#endif // __cplusplus

//! assertion function will stop application and report call stack
SEGAN_LIB_API void trace_assert(const char* expression, const char* file, const int line);

#if __cplusplus
}
#endif // __cplusplus

#else
#define sx_assert(expression)
#endif


#if (SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER || SEGANX_TRACE_MEMORY)

#define sx_trace_attach( stack_level, filename )    trace_attach( stack_level, filename )
#define sx_trace_detach()                           trace_detach()

#if __cplusplus
extern "C" {
#endif // __cplusplus

SEGAN_LIB_API void trace_attach( uint stack_level, const char* filename );
SEGAN_LIB_API void trace_detach( void );

#if __cplusplus
}
#endif // __cplusplus

#else

#define sx_trace_attach(stack_size, filename)
#define sx_trace_detach()

#endif  //  SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER



#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)

#define sx_trace()                          { trace_push( __FILE__, __LINE__, __FUNCTION__ ); }
#define sx_trace_param(function,...)        { trace_push_param( __FILE__, __LINE__, #function, __VA_ARGS__ ); }
#define sx_return(expression)               { trace_pop(); return expression; }

#if __cplusplus
extern "C" {
#endif // __cplusplus

SEGAN_LIB_API void trace_push(const char* file, const int line, const char* function);
SEGAN_LIB_API void trace_push_param(const char* file, const int line, const char* function, ...);
SEGAN_LIB_API void trace_pop(void);

#if __cplusplus
}
#endif // __cplusplus

#else

#define sx_trace()
#define sx_trace_param(function,...)
#define sx_return(expression)               return

#endif  //  SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER


#if SEGANX_TRACE_MEMORY

#define sx_mem_alloc( size_in_byte )            trace_mem_alloc( size_in_byte, __FILE__, __LINE__ )
#define sx_mem_calloc( size_in_byte )           trace_mem_calloc( size_in_byte, __FILE__, __LINE__ )
#define sx_mem_realloc( p, new_size_in_byte )   trace_mem_realloc( p, new_size_in_byte, __FILE__, __LINE__ )
#define sx_mem_free( p )                        trace_mem_free( p )
#define sx_mem_free_and_null( p )               { trace_mem_free( p ); p = null; }

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SEGAN_LIB_API void* trace_mem_alloc(const uint size_in_byte, const char* file, const int line);
SEGAN_LIB_API void* trace_mem_calloc(const uint size_in_byte, const char* file, const int line);
SEGAN_LIB_API void* trace_mem_realloc(void* p, const uint new_size_in_byte, const char* file, const int line);
SEGAN_LIB_API void* trace_mem_free(const void* p);

#ifdef __cplusplus
}
#define sx_new      new( __FILE__, __LINE__ )
SEGAN_INLINE void*  operator new (uint size, const char* file, int line) { return trace_mem_alloc(size, file, line); }
SEGAN_INLINE void   operator delete(void *p, const char* file, int line) { trace_mem_free(p); }
SEGAN_INLINE void   operator delete(void *p) { trace_mem_free(p); }
#endif // __cplusplus

#else

#define sx_mem_alloc( size_in_byte )                mem_alloc( size_in_byte )
#define sx_mem_realloc( p, new_size_in_byte )       mem_realloc( p, new_size_in_byte )
#define sx_mem_free( p )                            mem_free( p )
#define sx_mem_free_and_null( p )                   { mem_free( p ); p = null; }

#ifdef __cplusplus
#define sx_new      new
SEGAN_INLINE void*  operator new (uint size) { return mem_alloc(size); }
SEGAN_INLINE void   operator delete (void *p) { mem_free(p); }
#endif // __cplusplus

#endif // SEGANX_TRACE_MEMORY


#endif // HEADER_TRACE
