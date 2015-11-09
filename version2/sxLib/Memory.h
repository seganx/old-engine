/********************************************************************
	created:	2012/04/03
	filename: 	Memory.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some basic functions for conventional memory methods,
				two memory pool types and different memory managers suitable for other classes.
*********************************************************************/
#ifndef GUARD_Memory_HEADER_FILE
#define GUARD_Memory_HEADER_FILE

#include "Def.h"

//////////////////////////////////////////////////////////////////////////
//	BASIC MEMORY FUNCTIONS
//////////////////////////////////////////////////////////////////////////
typedef void (*CB_Memory)(const wchar* file, const uint line, const uint size, const uint tag, const bool corrupted);

SEGAN_LIB_API void		mem_set_manager( const class MemMan* manager );
SEGAN_LIB_API MemMan*	mem_get_manager( void );

SEGAN_LIB_API void*		mem_alloc( const uint sizeinbyte );
SEGAN_LIB_API void		mem_realloc( void*& p, const uint newsizeinbyte );
SEGAN_LIB_API uint		mem_size( const void* p );
SEGAN_LIB_API void		mem_free( const void* p );
SEGAN_LIB_API void		mem_copy( void* dest, const void* src, const uint size );
SEGAN_LIB_API void		mem_set( void* dest, const sint val, const uint size );

#define sx_mem_set_manager( manager )			mem_set_manager( manager )
#define sx_mem_get_manager()					mem_get_manager()
#define sx_mem_copy(dest, src, size)			mem_copy( dest, src, size )
#define sx_mem_set(dest, val, size)				mem_set( dest, val, size )

#if ( SEGAN_MEMLEAK == 1 )

SEGAN_LIB_API void		mem_enable_debug( const bool enable, const uint tag = 0 );
SEGAN_LIB_API void*		mem_alloc_dbg( const uint sizeinbyte, const wchar* file, const int line );
SEGAN_LIB_API void		mem_realloc_dbg( void*& p, const uint newsizeinbyte, const wchar* file, const int line );
SEGAN_LIB_API void		mem_free_dbg( const void* p );
SEGAN_LIB_API void		mem_report_debug( CB_Memory callback, const uint tag = 0 );
SEGAN_LIB_API void		mem_report_debug_to_file( const wchar* fileName, const uint tag = 0 );
SEGAN_LIB_API void		mem_clear_debug( void );

inline	  void*		operator new ( uint size, const wchar* file, int line ){ return mem_alloc_dbg( size, file, line ); }
inline    void		operator delete( void *p, const wchar* file, int line ){ mem_free_dbg(p); }
inline    void		operator delete( void *p ){ mem_free_dbg(p); }

//! enable memory debugger and set a new tag. pass -1 to restore previews tag
#define sx_mem_enable_debug( enable, tag )				mem_enable_debug( enable, tag )

//! report memory debugger. pass 0 to show all tags
#define sx_mem_report_debug( callback, tag )			mem_report_debug( callback, tag )

//! report memory debugger to output window
#define sx_mem_report_debug_to_window( tag )			mem_report_debug_to_window( tag )

//! report memory debugger to a file. pass 0 to show all tags, pass -1 to show only memory corruptions
#define sx_mem_report_debug_to_file( fileName, tag )	mem_report_debug_to_file( fileName, tag )


#define sx_mem_alloc( sizeinbyte )				mem_alloc_dbg( sizeinbyte, _CRT_WIDE(__FILE__), __LINE__ )
#define sx_mem_realloc( p, newsizeinbyte )		mem_realloc_dbg( (void*&)p, newsizeinbyte, _CRT_WIDE(__FILE__), __LINE__ )
#define sx_mem_size( p )						mem_size( p )
#define sx_mem_free( p )						mem_free_dbg( p )
#define sx_mem_free_and_null( p )				{ mem_free_dbg( p ); p = null; }

#define sx_new									new( _CRT_WIDE(__FILE__), __LINE__ )
#define sx_delete								delete
#define sx_delete_and_null( obj )				{ delete(obj); obj = null; }
#define sx_safe_delete( obj )					{ if (obj) { delete(obj); } }				
#define sx_safe_delete_and_null( obj )			{ if (obj) { delete(obj); obj = null; } }

#else

inline	  void*		operator new ( uint size ){ return mem_alloc( size ); }
inline	  void		operator delete ( void *p ){ mem_free(p); }

#define sx_mem_enable_debug( enable )
#define sx_mem_report_debug( callback )
#define sx_mem_report_debug_to_window( tag )
#define sx_mem_report_debug_to_file( fileName, tag )

#define sx_mem_alloc( sizeinbyte )				mem_alloc( sizeinbyte )
#define sx_mem_realloc( p, newsizeinbyte )		mem_realloc( (void*&)p, newsizeinbyte )
#define sx_mem_size( p )						mem_size( p )
#define sx_mem_free( p )						mem_free( (void*&)p )
#define sx_mem_free_and_null( p )				{ mem_free( p ); p = null }

#define sx_new									new
#define sx_delete								delete
#define sx_delete_and_null( obj )				{ delete(obj); obj = null; }
#define sx_safe_delete( obj )					{ if (obj) { delete(obj) ; } }				
#define sx_safe_delete_and_null( obj )			{ if (obj) { delete(obj) ; obj = null; } }

#endif


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
	virtual ~MemMan( void ) {}
	virtual void* alloc( const uint sizeInByte )					= 0;
	virtual void* realloc( const void* p, const uint sizeInByte )	= 0;
	virtual void free( const void* p )								= 0;
	virtual uint size( const void* p )								= 0;
};


//////////////////////////////////////////////////////////////////////////
//	MEMORY MANAGER : FREE LIST POOL
//////////////////////////////////////////////////////////////////////////


/*! a fast general memory pool which allocates a memory block in initialization from OS and uses the allocated 
memory pool in any allocation call. using this memory manager has restriction of block protection. */
class SEGAN_LIB_API MemMan_Pool : public MemMan
{
	SEGAN_STERILE_CLASS(MemMan_Pool);

public:
	MemMan_Pool( const uint poolSizeInBytes );
	~MemMan_Pool( void );
	
	void* alloc( const uint sizeInByte );
	void* realloc( const void* p, const uint sizeInByte );
	void free( const void* p );
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



#endif	//	GUARD_Memory_HEADER_FILE
