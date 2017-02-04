/********************************************************************
    created:	2012/04/03
    filename: 	Memory.h
    Author:		Sajad Beigjani
    eMail:		sajad.b@gmail.com
    Site:		www.SeganX.com
    Desc:		This file contain some basic functions for conventional memory methods,
                two memory pool types and different memory managers suitable for other classes.
*********************************************************************/
#ifndef HEADER_MEMORY
#define HEADER_MEMORY

#include "Def.h"


//! set a new memory manager as an internal memory manager
#define sx_mem_set_manager( manager )			mem_set_manager( manager )

//! return current internal memory manager
#define sx_mem_get_manager()					mem_get_manager()


#define sx_mem_copy(dest, src, size)			mem_copy( dest, src, size )
#define sx_mem_cmp(src1, src2, size)			mem_cmp( src1, src2, size )
#define sx_mem_set(dest, val, size)				mem_set( dest, val, size )


#if ( SEGAN_MEMLEAK == 1 )

//! enable or disable memory debugger
#define sx_mem_enable_debug( enable )				mem_enable_debug( enable )

//! report memory debugger to callback function
#define sx_mem_report_debug( callback, userdata )	mem_report_debug( callback, userdata )

//! report memory debugger to output window
#define sx_mem_report_debug_to_window()			    mem_report_debug_to_window()

//! report memory debugger to a file
#define sx_mem_report_debug_to_file( fileName )	    mem_report_debug_to_file( fileName )

#define sx_mem_alloc( size_in_byte )                mem_alloc_dbg( size_in_byte, __FILE__, __LINE__ )
#define sx_mem_realloc( p, new_size_in_byte )       mem_realloc_dbg( p, new_size_in_byte, __FILE__, __LINE__ )
#define sx_mem_free( p )                            mem_free_dbg( p )
#define sx_mem_free_and_null( p )                   { mem_free_dbg( p ); p = null; }

#else

#define sx_mem_enable_debug( enable )
#define sx_mem_report_debug( callback )
#define sx_mem_report_debug_to_window( tag )
#define sx_mem_report_debug_to_file( fileName, tag )

#define sx_mem_alloc( size_in_byte )                mem_alloc( size_in_byte )
#define sx_mem_realloc( p, new_size_in_byte )       mem_realloc( p, new_size_in_byte )
#define sx_mem_free( p )                            mem_free( p )
#define sx_mem_free_and_null( p )                   { mem_free( p ); p = null; }

#endif


#ifdef __cplusplus

#if ( SEGAN_MEMLEAK == 1 )
#define sx_new  new( __FILE__, __LINE__ )
#else
#define sx_new  new
#endif // ( SEGAN_MEMLEAK == 1 )

#define sx_delete						delete
#define sx_delete_and_null( obj )		{ delete(obj); obj = null; }
#define sx_safe_delete( obj )			{ if (obj) { delete(obj); } }				
#define sx_safe_delete_and_null( obj )  { if (obj) { delete(obj); obj = null; } }

#endif // __cplusplus




/*!
memory callback function used to travers all memories
*/
typedef void (*mem_callback)(void* userdata, const char* file, const int line, const uint size, const bool corrupted);

/*!
MemoryManagers are some different classes with same style
that can be used in type of classes which will alloc/free
memory blocks frequently.
*/
typedef struct sx_memory_manager
{
    void* (*alloc)(const uint size_in_byte);
    void* (*realloc)(const void* p, const uint new_size_in_byte);
    void* (*free)(const void* p);
}
sx_memory_manager;


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SEGAN_LIB_API void mem_set_manager(struct sx_memory_manager* manager);
SEGAN_LIB_API struct sx_memory_manager* mem_get_manager(void);

SEGAN_LIB_API void* mem_alloc(const uint size_in_byte);
SEGAN_LIB_API void* mem_realloc(void* p, const uint new_size_in_byte);
SEGAN_LIB_API void* mem_free(const void* p);
SEGAN_LIB_API void  mem_copy(void* dest, const void* src, const uint size);
SEGAN_LIB_API sint  mem_cmp(const void* src1, const void* src2, const uint size);
SEGAN_LIB_API void  mem_set(void* dest, const sint val, const uint size);

#if ( SEGAN_MEMLEAK == 1 )
SEGAN_LIB_API void  mem_enable_debug(const bool enable);
SEGAN_LIB_API void* mem_alloc_dbg(const uint size_in_byte, const char* file, const int line);
SEGAN_LIB_API void* mem_realloc_dbg(void* p, const uint new_size_in_byte, const char* file, const int line);
SEGAN_LIB_API void* mem_free_dbg(const void* p);
SEGAN_LIB_API void  mem_report_debug(mem_callback callback, void* userdata);
SEGAN_LIB_API void  mem_report_debug_to_file(const char* fileName);
SEGAN_LIB_API void  mem_clear_debug(void);
#endif

#ifdef __cplusplus
}

#if ( SEGAN_MEMLEAK == 1 )
SEGAN_INLINE void*  operator new (uint size, const char* file, int line) { return mem_alloc_dbg(size, file, line); }
SEGAN_INLINE void   operator delete(void *p, const char* file, int line) { mem_free_dbg(p); }
SEGAN_INLINE void   operator delete(void *p) { mem_free_dbg(p); }
#else
SEGAN_INLINE void*  operator new (uint size) { return mem_alloc(size); }
SEGAN_INLINE void   operator delete (void *p) { mem_free(p); }
#endif // ( SEGAN_MEMLEAK == 1 )

#endif // __cplusplus




#endif	//	HEADER_MEMORY
