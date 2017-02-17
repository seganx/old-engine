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
#define sx_mem_set_manager( manager )   mem_set_manager( manager )

//! return current internal memory manager
#define sx_mem_get_manager()            mem_get_manager()

#define sx_mem_copy(dest, src, size)    mem_copy( dest, src, size )
#define sx_mem_cmp(src1, src2, size)    mem_cmp( src1, src2, size )
#define sx_mem_set(dest, val, size)     mem_set( dest, val, size )


#ifdef __cplusplus

#define sx_delete                       delete
#define sx_delete_and_null( obj )       { delete(obj); obj = null; }
#define sx_safe_delete( obj )           { if (obj) { delete(obj); } }				
#define sx_safe_delete_and_null( obj )  { if (obj) { delete(obj); obj = null; } }

#endif // __cplusplus


/*!
MemoryManagers are some different classes with same style that can be 
used in type of classes which will alloc/free memory blocks frequently.
*/
typedef struct sx_memory_manager
{
    void* (*alloc)( const uint size_in_byte );
    void* (*realloc)( const void* p, const uint new_size_in_byte );
    void* (*free)( const void* p );
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

#ifdef __cplusplus
}
#endif // __cplusplus

#endif	//	HEADER_MEMORY
