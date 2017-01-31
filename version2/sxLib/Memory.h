/********************************************************************
	created:	2012/04/03
	filename: 	Memory.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some basic functions for conventional memory methods,
				two memory pool types and different memory managers suitable for other classes.
*********************************************************************/
#ifndef HEADER_DEFINED_Memory
#define HEADER_DEFINED_Memory

#include "Def.h"

//////////////////////////////////////////////////////////////////////////
//	BASIC MEMORY FUNCTIONS
//////////////////////////////////////////////////////////////////////////
typedef void (*CB_Memory)(void* userdata, const char* file, const uint line, const uint size, const uint tag, const bool corrupted);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    SEGAN_LIB_API void		mem_set_manager(const class MemMan* manager);
    SEGAN_LIB_API MemMan*	mem_get_manager(void);

    SEGAN_LIB_API void*		mem_alloc(const uint sizeinbyte);
    SEGAN_LIB_API void*		mem_realloc(void* p, const uint newsizeinbyte);
    SEGAN_LIB_API void*		mem_free(const void* p);
    SEGAN_LIB_API void		mem_copy(void* dest, const void* src, const uint size);
    SEGAN_LIB_API sint		mem_cmp(const void* src1, const void* src2, const uint size);
    SEGAN_LIB_API void		mem_set(void* dest, const sint val, const uint size);

#ifdef __cplusplus
}
#endif // __cplusplus


#define sx_mem_set_manager( manager )			mem_set_manager( manager )
#define sx_mem_get_manager()					mem_get_manager()
#define sx_mem_copy(dest, src, size)			mem_copy( dest, src, size )
#define sx_mem_cmp(src1, src2, size)			mem_cmp( src1, src2, size )
#define sx_mem_set(dest, val, size)				mem_set( dest, val, size )

#if ( SEGAN_MEMLEAK == 1 )

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    SEGAN_LIB_API void		mem_enable_debug(const bool enable, const uint tag = 0);
    SEGAN_LIB_API void*		mem_alloc_dbg(const uint sizeinbyte, const char* file, const int line);
    SEGAN_LIB_API void*		mem_realloc_dbg(void* p, const uint newsizeinbyte, const char* file, const int line);
    SEGAN_LIB_API void*		mem_free_dbg(const void* p);
    SEGAN_LIB_API void		mem_report_debug(CB_Memory callback, void* userdata, const uint tag = 0);
    SEGAN_LIB_API void		mem_report_debug_to_file(const wchar* fileName, const uint tag = 0);
    SEGAN_LIB_API void		mem_clear_debug(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
SEGAN_INLINE  void*		operator new (uint size, const char* file, int line) { return mem_alloc_dbg(size, file, line); }
SEGAN_INLINE  void		operator delete(void *p, const char* file, int line) { mem_free_dbg(p); }
SEGAN_INLINE  void		operator delete(void *p) { mem_free_dbg(p); }
#endif // __cplusplus


//! enable memory debugger and set a new tag. pass -1 to restore previews tag
#define sx_mem_enable_debug( enable, tag )				mem_enable_debug( enable, tag )

//! report memory debugger. pass 0 to show all tags
#define sx_mem_report_debug( callback, userdata, tag )	mem_report_debug( callback, userdata, tag )

//! report memory debugger to output window
#define sx_mem_report_debug_to_window( tag )			mem_report_debug_to_window( tag )

//! report memory debugger to a file. pass 0 to show all tags, pass -1 to show only memory corruptions
#define sx_mem_report_debug_to_file( fileName, tag )	mem_report_debug_to_file( fileName, tag )


#define sx_mem_alloc( sizeinbyte )				mem_alloc_dbg( sizeinbyte, __FILE__, __LINE__ )
#define sx_mem_realloc( p, newsizeinbyte )		mem_realloc_dbg( p, newsizeinbyte, __FILE__, __LINE__ )
#define sx_mem_free( p )						mem_free_dbg( p )
#define sx_mem_free_and_null( p )				{ mem_free_dbg( p ); p = null; }

#define sx_new									new( __FILE__, __LINE__ )
#define sx_delete								delete
#define sx_delete_and_null( obj )				{ delete(obj); obj = null; }
#define sx_safe_delete( obj )					{ if (obj) { delete(obj); } }				
#define sx_safe_delete_and_null( obj )			{ if (obj) { delete(obj); obj = null; } }

#else


#ifdef __cplusplus
//SEGAN_INLINE void*	operator new (uint size) { return mem_alloc(size); }
//SEGAN_INLINE void	operator delete (void *p) { mem_free(p); }
#endif // __cplusplus

#define sx_mem_enable_debug( enable )
#define sx_mem_report_debug( callback )
#define sx_mem_report_debug_to_window( tag )
#define sx_mem_report_debug_to_file( fileName, tag )

#define sx_mem_alloc( sizeinbyte )				mem_alloc( sizeinbyte )
#define sx_mem_realloc( p, newsizeinbyte )		mem_realloc( p, newsizeinbyte )
#define sx_mem_free( p )						mem_free( p )
#define sx_mem_free_and_null( p )				{ mem_free( p ); p = null; }

#define sx_new									new
#define sx_delete								delete
#define sx_delete_and_null( obj )				{ delete(obj); obj = null; }
#define sx_safe_delete( obj )					{ if (obj) { delete(obj) ; } }				
#define sx_safe_delete_and_null( obj )			{ if (obj) { delete(obj) ; obj = null; } }

#endif



#ifdef __cplusplus

//////////////////////////////////////////////////////////////////////////
//	MEMORY MANAGERS
//////////////////////////////////////////////////////////////////////////

/*! 
MemoryManagers are some different classes with same style 
that can be used in type of classes which will alloc/free
memory blocks frequently. 
*/
class MemMan
{
public:
	virtual void* alloc( const uint sizeInByte )					= 0;
	virtual void* realloc( const void* p, const uint sizeInByte )	= 0;
	virtual void* free( const void* p )								= 0;
	virtual uint size( const void* p )								= 0;
};


//////////////////////////////////////////////////////////////////////////
//	MEMORY MANAGER : FREE LIST POOL
//////////////////////////////////////////////////////////////////////////


/*! a fast general memory pool which allocates a memory block in initialization from OS and uses the allocated 
memory pool in any allocation call. using this memory manager has restriction of block protection. */
class SEGAN_LIB_API MemMan_Pool : public MemMan
{
	sx_sterile_class(MemMan_Pool);

public:
	MemMan_Pool( const uint poolSizeInBytes );
	~MemMan_Pool( void );
	
	void* alloc( const uint sizeInByte );
	void* realloc( const void* p, const uint sizeInByte );
	void* free( const void* p );
	uint size( const void* p );

public:

	enum ChunkState
	{
		CS_NULL			= 0xf0f0f0f0,
		CS_EMPTY		= 0xfefefefe,
		CS_FULL			= 0xfafafafa,
		CS_FORCE32BIT	= 0xffffffff
	};

	struct Chunk
	{
		Chunk*		behind;
		Chunk*		next;
		Chunk*		prev;
		uint		size;
		ChunkState	state;
	};
	typedef Chunk* PChunk;

private:

	void push( Chunk* ch );
	void pop( Chunk* ch );

public:
	pbyte		m_pool;
	Chunk*		m_root;

};

#endif // __cplusplus


#endif	//	HEADER_DEFINED_Memory
