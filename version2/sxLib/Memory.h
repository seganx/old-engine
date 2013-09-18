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
#include "Assert.h"

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
#define sx_mem_free_and_null( p )				mem_free_dbg( p ); p = null

#define sx_new( obj )							( new( _CRT_WIDE(__FILE__), __LINE__ ) obj )
#define sx_delete( obj )						{ if (obj) { delete(obj) ; } }				
#define sx_delete_and_null( obj )				{ if (obj) { delete(obj) ; obj = null; } }

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
#define sx_mem_free_and_null( p )				mem_free( p ); p = null

#define sx_new( obj )							( new obj )
#define sx_delete( obj )						{ if (obj) { delete(obj) ; } }				
#define sx_delete_and_null( obj )				{ if (obj) { delete(obj) ; obj = null; } }

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
	virtual void* alloc( const uint sizeInByte )			= 0;
	virtual void realloc( void*& p, const uint sizeInByte )	= 0;
	virtual void free( const void* p )						= 0;
	virtual uint size( const void* p )						= 0;
};


//////////////////////////////////////////////////////////////////////////
//	MEMORY MANAGER : FREE LIST POOL
//////////////////////////////////////////////////////////////////////////


/*! a fast general memory pool which allocates a memory block in initialization from OS and uses the allocated 
memory pool in any allocation call. using this memory manager has restriction of block protection. */
class MemMan_Pool : public MemMan
{

	SEGAN_STERILE_CLASS(MemMan_Pool);
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
	} 
	typedef *PChunk;

public:
	MemMan_Pool( const uint poolSizeInBytes ): MemMan()
	{
		uint memsize = poolSizeInBytes + 0x0fff * sizeof(Chunk);
		m_pool = (pbyte)mem_alloc( memsize );
		sx_assert( m_pool );
#ifdef _DEBUG
		mem_set( m_pool, 0, memsize );
#endif
		//  set first empty chunk
		Chunk* ch	= PChunk(m_pool);
		ch->state	= CS_EMPTY;
		ch->size	= memsize - 2 * sizeof(Chunk);

		//  create last inactive chuck
		Chunk*	ich	= PChunk( m_pool + memsize - sizeof(Chunk) );
		ich->behind	= ch;
		ich->state	= CS_NULL;
		ich->size	= 0;
		m_root = ich;
		push( ch );
	}

	~MemMan_Pool( void )
	{
		mem_free( m_pool );
	}

	void* alloc( const uint sizeInByte )
	{
		Chunk* ch = m_root;
		while( ch->state == CS_EMPTY )
		{
			if ( ch->size >= sizeInByte )
			{
				//  free chunk founded
				pop( ch );
				if ( ( ch->size - sizeInByte ) > sizeof(Chunk) )
				{
					//  create new empty chunk by remaining size
					Chunk* ech  = PChunk( pbyte(ch) + sizeof(Chunk) + sizeInByte );
					ech->behind = ch;
					ech->size	= ch->size - ( sizeof(Chunk) + sizeInByte );
					ech->state	= CS_EMPTY;
					push( ech );
					PChunk( pbyte(ech) + sizeof(Chunk) + ech->size )->behind = ech;
					ch->size = sizeInByte;
				}
				ch->state = CS_FULL;
				return ( pbyte(ch) + sizeof(Chunk) );
			}
			ch = ch->next;
		}
		sx_assert( "MemMan_Pool::alloc(...) failed due to there is no free chunk exist whit specified size!"<0 );
		return null;
	}

	SEGAN_LIB_INLINE void realloc( void*& p, const uint sizeInByte )
	{
		if ( !p )
		{
			p = alloc( sizeInByte );
		}
		else
		{
			sx_assert( pbyte(p) > m_pool && pbyte(p) < ( m_pool + mem_size(m_pool) ) );
			void* tmp = alloc( sizeInByte );
			mem_copy( tmp, p, sx_min_i( size(p), sizeInByte ) );
			free( p );
			p = tmp;
		}
	}

	SEGAN_LIB_INLINE void free( const void* p )
	{
		if ( !p ) return;
		sx_assert( pbyte(p) > m_pool && pbyte(p) < ( m_pool + mem_size(m_pool) ) );

		Chunk* ch = PChunk( pbyte(p) - sizeof(Chunk) );
		sx_assert( ch->state == CS_FULL );	//	avoid to free a chunk more that once

#ifdef _DEBUG
		mem_set( (void*)p, 0, ch->size );
#endif

		//  look at neighbor chunk in right side
		Chunk* rch = PChunk( pbyte(p) + ch->size );
		if ( rch->state == CS_EMPTY )
		{
			PChunk( pbyte(rch) + sizeof(Chunk) + rch->size )->behind = ch;
			ch->size += sizeof(Chunk) + rch->size;
			pop( rch );

#ifdef _DEBUG
			mem_set( rch, 0, sizeof(Chunk) );
#endif
		}

		//  look at neighbor chunk in left side
		if ( ch->behind )
		{	
			if ( ch->behind->state == CS_EMPTY )
			{
				PChunk( pbyte(ch) + sizeof(Chunk) + ch->size )->behind = ch->behind;
				ch->behind->size += sizeof(Chunk) + ch->size;
#ifdef _DEBUG
				mem_set( ch, 0, sizeof(Chunk) );
#endif
				return;	//	this one is already exist in list
			}
		}

		//	push to free list
		ch->state = CS_EMPTY;
		push( ch );
	}

	SEGAN_LIB_INLINE uint size( const void* p )
	{
		if ( !p ) return 0;
		sx_assert( pbyte(p) > m_pool && pbyte(p) < ( m_pool + mem_size(m_pool) ) );
		Chunk* ch = PChunk( pbyte(p) - sizeof(Chunk) );
		return ch->size;
	}

private:
	SEGAN_LIB_INLINE void push( Chunk* ch )
	{
		ch->next = m_root;
		ch->prev = null;
		m_root->prev = ch;
		m_root = ch;
	}

	SEGAN_LIB_INLINE void pop( Chunk* ch )
	{
		if(ch->prev)	ch->prev->next = ch->next;
		if(ch->next)	ch->next->prev = ch->prev;
		if(ch==m_root)	m_root = ch->next;
	}

public:

	pbyte		m_pool;
	Chunk*		m_root;

};





#if 0

/*! 
default settings of hybrid memory pool: 
init size will define size of memory blocks in initialization for every chunk.
*/
#define	_SX_MEM_CH_INITSIZE_	2048	//  2KB
#define	_SX_MEM_CH_SAMPLER_		2048	//	2KB

/*! 
a fast hybrid memory pool use system heap management and memory chunk.
this memory pool has static chunk count with variable chunk size. 
*/
class MemPool_hybrid
{

	SEGAN_STERILE_CLASS(MemPool_hybrid);

public:
	
	MemPool_hybrid( uint ChunkCount )
	{
		SEGAN_ASSERT( ChunkCount );
		m_pool = (PChunk)sx_mem_alloc( ChunkCount * sizeof(CChunk) );
		m_root = m_pool;
		uint i=0;
		for ( ; i<ChunkCount; i++ ){
			m_pool[i].data = (pbyte)sx_mem_alloc( _SX_MEM_CH_INITSIZE_ + sizeof(PChunk) );
			m_pool[i].size = _SX_MEM_CH_INITSIZE_;
			PPointer(m_pool[i].data)->ch = &m_pool[i];
			if ( i )
				m_pool[i-1].next = &m_pool[i];
		}
		m_pool[i-1].next = null;
	}

	~MemPool_hybrid()
	{
		uint c = sx_mem_size(m_pool) / sizeof(CChunk);
		for (uint i=0; i<c; i++)
			sx_mem_free( m_pool[i].data );
		sx_mem_free( m_pool );
	}

	template <class T>
	T Alloc( uint sizeInByte ) {
		if ( !m_root ) return null;
		PChunk ch = m_root;
		m_root = ch->next;
		ch->next = null;
		if ( sizeInByte>ch->size || ch->size-sizeInByte>_SX_MEM_CH_SAMPLER_ ) {
			ch->size = sint( sizeInByte/_SX_MEM_CH_SAMPLER_ ) * _SX_MEM_CH_SAMPLER_ + _SX_MEM_CH_SAMPLER_;
			sx_mem_free( ch->data );
			ch->data = (pbyte)sx_mem_alloc( ch->size + sizeof(PChunk) );
			PPointer(ch->data)->ch = ch;
			return static_cast<T>( ch->data + sizeof(PChunk) );
		}
		return static_cast<T>( ch->data + sizeof(PChunk) );
	}

	template <class T>
	void Realloc( T& varPtr, uint newSize )
	{
		if ( !varPtr ) {
			varPtr = Alloc<T>(newSize);		
			return;
		}
		if (!newSize) {
			Free(varPtr);
			varPtr = null;
			return;
		}
		PChunk ch = PPointer( pbyte(varPtr) - sizeof(pvoid) )->ch;
		if ( newSize>ch->size || ch->size-newSize>_SX_MEM_CH_SAMPLER_ ) {
			ch->size = sint( newSize/_SX_MEM_CH_SAMPLER_ ) * _SX_MEM_CH_SAMPLER_ + _SX_MEM_CH_SAMPLER_;
			sx_mem_realloc( ch->data, ch->size + sizeof(PChunk) );
			PPointer(ch->data)->ch = ch;
			varPtr = static_cast<T>( ch->data + sizeof(PChunk) );
		}
	}

	void Free( void*& p )
	{
		if ( !p ) return;
		PChunk ch = PPointer( static_cast<pbyte>(p) - sizeof(PChunk) )->ch;
		ch->next = m_root;
		m_root = ch;
		p = null;
	}

	uint Size( void* p )
	{
		if ( !p ) return 0;
		PChunk ch = PPointer( static_cast<pbyte>(p) - sizeof(PChunk) )->ch;
		return ch->size;
	}

private:
	typedef struct CChunk
	{
		uint		size;
		byte*		data;		//  use Byte* to easily steps between bytes in memory
		CChunk*		next;
	} *PChunk;

	typedef struct CPointer
	{
		PChunk		ch;
	} *PPointer;

	PChunk	m_pool;
	PChunk	m_root;
};

/*! 
Memory manager that use standard OS memory management system 
*/
class MemManOS : public MemMan
{
	SEGAN_STERILE_CLASS(MemManOS);

public:
	MemManOS( void ) {}
	virtual ~MemManOS( void ) {}

	SEGAN_LIB_INLINE void* Alloc( const uint sizeInByte )
	{
		return sx_mem_alloc( sizeInByte );
	}

	SEGAN_LIB_INLINE void Realloc( void*& p, const uint sizeInByte )
	{
		sx_mem_realloc( p, sizeInByte );
	}

	SEGAN_LIB_INLINE void Free( const void* p )
	{
		sx_mem_free( p );
	}

	SEGAN_LIB_INLINE uint Size( const void* p )
	{
		return sx_mem_size( p );
	}
};

/*!
Memory manager that use memory pool system 
*/
class MemManPool : public MemMan
{
	SEGAN_STERILE_CLASS(MemManPool);

public:
	MemManPool( const uint poolSizeInByte ): MemMan(), m_pool( poolSizeInByte ) {};
	virtual ~MemManPool( void ) {}

	SEGAN_LIB_INLINE void* Alloc( const uint sizeInByte )
	{
		return m_pool.Alloc<void*>( sizeInByte );
	}

	SEGAN_LIB_INLINE void Realloc( void*& p, const uint sizeInByte )
	{
		m_pool.Realloc( p, sizeInByte );
	}

	SEGAN_LIB_INLINE void Free( const void* p )
	{
		m_pool.Free( (void*&)p );
	}

	SEGAN_LIB_INLINE uint Size( const void* p )
	{
		m_pool.Size( (void*)p );
	}

private:
	MemPool m_pool;
};

/*! 
Memory manager that use fast HYBRID memory pool system 
*/
class MemManPool_hybrid : public MemMan
{
	SEGAN_STERILE_CLASS(MemManPool_hybrid);

public:
	MemManPool_hybrid( const uint ChunkCount ): MemMan(), m_pool( ChunkCount ) {};
	virtual ~MemManPool_hybrid( void ) {}

	SEGAN_LIB_INLINE void* Alloc( const uint sizeInByte )
	{
		return m_pool.Alloc<void*>( sizeInByte );
	}

	SEGAN_LIB_INLINE void Realloc( void*& p, const uint sizeInByte )
	{
		m_pool.Realloc<void*>( p, sizeInByte );
	}

	SEGAN_LIB_INLINE void Free( const void* p )
	{
		m_pool.Free( (void*&)p );
	}

	SEGAN_LIB_INLINE uint Size( const void* p )
	{
		m_pool.Size( (void*)p );
	}

private:
	MemPool_hybrid	m_pool;
};

/*! 
Fixed memory manager that use a simple fixed Size memory block. useful for strings or array
*/
class MemManFixed : public MemMan
{
	SEGAN_STERILE_CLASS(MemManFixed);

public:
	MemManFixed( const uint sizeInByte ): MemMan(), m_internal(true)
	{
		m_mem = sx_mem_alloc( sizeInByte );
	}

	MemManFixed( void* buffer ): MemMan(), m_internal(false)
	{
		m_mem = buffer;
	}

	//!  use set buffer to specify a memory block
	MemManFixed( void ): MemMan(), m_internal(false)
	{
		//  use set buffer to specify a memory block
	}

	virtual ~MemManFixed( void )
	{
		if ( m_internal ) sx_mem_free( m_mem );
	}

	SEGAN_LIB_INLINE void* Alloc( const uint sizeInByte )
	{
		return m_mem;
	}

	SEGAN_LIB_INLINE void Realloc( void*& p, const uint sizeInByte )
	{
		p = m_mem;
	}

	SEGAN_LIB_INLINE void Free( const void* p )
	{
		//	:)
	}

	SEGAN_LIB_INLINE uint Size( const void* p )
	{
		return 0;
	}

	SEGAN_LIB_INLINE void SetBuffer( void* buffer )
	{
		m_mem = buffer;
	}

	void*	m_mem;
	bool	m_internal;
};

/*! 
Fixed memory manager that use fixed Size memory block from memory stack in functions. useful for strings or array
NOTE: use with care in recursive functions.
*/
template<uint memSizeInByte>
class MemManFixed_inline : public MemMan
{
	SEGAN_STERILE_CLASS(MemManFixed_inline);

public:
	MemManFixed_inline( void ): MemMan() {}
	virtual ~MemManFixed_inline( void ) {}

	SEGAN_LIB_INLINE void* Alloc( const uint sizeInByte )
	{
		return m_mem;
	}

	SEGAN_LIB_INLINE void Realloc( void*& p, const uint sizeInByte )
	{
		p = m_mem;
	}

	SEGAN_LIB_INLINE void Free( const void* p )
	{
		//	:)
	}

	SEGAN_LIB_INLINE uint Size( const void* p )
	{
		return 0;
	}

public:
	byte m_mem[memSizeInByte];
};

#endif



#endif	//	GUARD_Memory_HEADER_FILE
