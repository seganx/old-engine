#include <memory>

#if defined(_WIN32)
#include <Windows.h>
#endif

#include "Memory.h"
#include "Assert.h"

#if SEGAN_LIB_MULTI_THREADED
#include "Mutex.h"
#endif

#define mem_min_size(a,b) (((a)<(b))?(a):(b))


//////////////////////////////////////////////////////////////////////////
//	memory management
//////////////////////////////////////////////////////////////////////////
static MemMan* s_manager = null;

SEGAN_INLINE void mem_set_manager( const MemMan* manager )
{
	s_manager = (MemMan*)manager;
}

SEGAN_INLINE MemMan* mem_get_manager( void )
{
	return s_manager;
}

SEGAN_INLINE void* mem_alloc( const uint sizeinbyte )
{
	return s_manager ? s_manager->alloc( sizeinbyte ) : ::malloc( sizeinbyte );
}

SEGAN_INLINE void* mem_realloc( void* p, const uint newsizeinbyte )
{
	if ( !newsizeinbyte )
		return mem_free( p );

	if ( s_manager )
		return s_manager->realloc( p, newsizeinbyte );

	void* newptr = ::realloc( p, newsizeinbyte );
	if ( !newptr )
	{
		if ( p )
		{
			//  if reallocate function failed then try to allocate new one and copy last data to new pool
			newptr = ::malloc( newsizeinbyte );
			memcpy( newptr, p, mem_min_size( sx_mem_size( p ), newsizeinbyte ) );
			::free( p );
		}
		else newptr = ::malloc( newsizeinbyte );
	}
	
	return newptr;
}

SEGAN_INLINE uint mem_size( const void* p )
{
	if ( !p ) return 0;
	return s_manager ? s_manager->size( p ) : (uint)::_msize( (void*)p );	
}

SEGAN_INLINE void* mem_free( const void* p )
{
	if ( s_manager )
		return s_manager->free( p );
	else
		::free( (void*)p );
	return null;
}

SEGAN_INLINE void mem_copy( void* dest, const void* src, const uint size )
{
	memcpy( dest, src, size );
}

SEGAN_LIB_API sint mem_cmp( void* src1, const void* src2, const uint size )
{
	return memcmp( src1, src2, size );
}

SEGAN_INLINE void mem_set( void* dest, const sint val, const uint size )
{
	memset( dest, val, size );
}



//////////////////////////////////////////////////////////////////////////
//	debug allocator used to detect memory leaks
#if ( SEGAN_MEMLEAK == 1 )

struct MemBlock
{
	char		sign[16];		//! sign to check if the block is a memory block. It's different from protection sign
	char*		file;
	uint		line;
	uint		size;
	uint		tag;
	bool		corrupted;
	MemBlock*	next;
	MemBlock*	prev;
};
static MemBlock*		s_mem_root			= null;
static bool				s_mem_enable_leak	= false;
static uint				s_mem_tag			= 0;
static uint				s_mem_last_tag		= 0;
static const char*		s_mem_protection	= "!protected area!";
static const uint		s_mem_protect_size	= 16;
static sint				s_mem_corruptions	= 0;


struct MemCodeReport
{
	void*		p;		//	user coded memory
	MemBlock*	mb;		//	memory block for log
};

SEGAN_INLINE void mem_block_to_str( wchar* dest, const int destsize, MemBlock* mb )
{
	if (mb->corrupted)
		swprintf_s(dest, destsize, L"%S(%d): error! memory corrupted - size = %d - tag = %d\n", mb->file, mb->line, mb->size, mb->tag);
	else
		swprintf_s(dest, destsize, L"%S(%d): warning! memory leak - size = %d - tag = %d\n", mb->file, mb->line, mb->size, mb->tag);
}

SEGAN_INLINE void mem_check_protection( MemBlock* mb )
{
	pbyte p = pbyte(mb) + sizeof(MemBlock);

	//	check beginning of memory block
	if ( memcmp( p, s_mem_protection, s_mem_protect_size ) )
	{
		mb->corrupted = true;
	}
	else
	{
		//	check end of memory block
		p += s_mem_protect_size + mb->size;
		mb->corrupted = memcmp( p , s_mem_protection, s_mem_protect_size ) != 0;
	}
}

SEGAN_INLINE MemCodeReport mem_code_protection( void* p, const uint realsizeinbyte )
{
	MemCodeReport res;
	if ( !p )
	{
		res.p = null;
		res.mb = null;
		return res;
	}

	// extract memory block
	res.mb = (MemBlock*)p;
	res.mb->corrupted = false;

	//	set protection sign for memory block
	memcpy( res.mb->sign, s_mem_protection, s_mem_protect_size );

	// prepare the result
	p = pbyte(p) + sizeof(MemBlock);
	res.p = pbyte(p) + s_mem_protect_size;

	//	sign beginning of memory block
	memcpy( p, s_mem_protection, s_mem_protect_size );

	//	sign end of memory block
	memcpy( pbyte(res.p) + realsizeinbyte, s_mem_protection, s_mem_protect_size );

	return res;
}


SEGAN_INLINE MemCodeReport mem_decode_protection( const void* p )
{
	MemCodeReport res;
	if ( !p )
	{
		res.p = null;
		res.mb = null;
	}
	else
	{
		res.mb	= (MemBlock*)( pbyte(p) - s_mem_protect_size - sizeof(MemBlock) );
		res.p	= (void*)p;

		// check protection sign for memory block
		if ( memcmp( res.mb->sign, s_mem_protection, s_mem_protect_size ) )
		{
			res.p = null;
			res.mb = null;
		}
		else mem_check_protection( res.mb );
	}
	return res;
}

SEGAN_INLINE void mem_debug_push_unsafe( MemBlock* mb )
{
	if ( !s_mem_root )
	{
		mb->next = null;
		mb->prev = null;
		s_mem_root = mb;
	}
	else
	{
		s_mem_root->prev = mb;
		mb->next = s_mem_root;
		mb->prev = null;
		s_mem_root = mb;
	}
}

SEGAN_INLINE void mem_debug_pop_unsafe( MemBlock* mb )
{
	if ( s_mem_root )
	{
		// unlink from the list
		if ( mb == s_mem_root )
		{
			s_mem_root = s_mem_root->next;
			if ( s_mem_root )
				s_mem_root->prev = null;
		}
		else
		{
			mb->prev->next = mb->next;
			if ( mb->next )
				mb->next->prev = mb->prev;
		}
	}
}

SEGAN_INLINE void mem_debug_push_pop(MemBlock* mb, const bool pop = false)
{
#if SEGAN_LIB_MULTI_THREADED
	static Mutex s_mutex;
	s_mutex.lock();
#endif

	if ( pop )
		mem_debug_pop_unsafe( mb );
	else
		mem_debug_push_unsafe( mb );
	
#if SEGAN_LIB_MULTI_THREADED
	s_mutex.unlock();
#endif
}


SEGAN_INLINE void mem_enable_debug( const bool enable, const uint tag /*= 0 */ )
{
	s_mem_enable_leak = enable;
	if ( tag == -1 )
	{
		s_mem_tag = s_mem_last_tag;
	}
	else
	{
		s_mem_last_tag = s_mem_tag;
		s_mem_tag = tag;
	}
}

SEGAN_INLINE void* mem_alloc_dbg( const uint sizeinbyte, const char* file, const int line )
{
	void* res = null;

	if ( s_mem_enable_leak )
	{
		//	first block for holding data, second block for protection and memory, then close with other protection
		res = mem_alloc( sizeof(MemBlock) + s_mem_protect_size + sizeinbyte + s_mem_protect_size );
		sx_assert( res );
		
		//	sign memory to check memory corruption
		MemCodeReport memreport = mem_code_protection( res, sizeinbyte );

		//	store memory block to link list
		if ( memreport.mb )
		{
			memreport.mb->file	= (char*)file;
			memreport.mb->line	= line;
			memreport.mb->size	= sizeinbyte;
			memreport.mb->tag	= s_mem_tag;

			// push the block
			mem_debug_push_pop( memreport.mb );
		}

		//	set user memory as result
		res = memreport.p;
	}
	else res = mem_alloc( sizeinbyte );

	return res;
}

SEGAN_INLINE void* mem_realloc_dbg( void* p, const uint newsizeinbyte, const char* file, const int line )
{
	if ( !newsizeinbyte )
		return mem_free_dbg( p );

	MemCodeReport memreport = mem_decode_protection( p );

	if ( memreport.mb )
	{
		//	if memory has been corrupted we should hold the corrupted memory info
		if ( memreport.mb->corrupted )
		{
			//	report memory allocations to file
			wchar tmp[64] = {0};
			swprintf_s( tmp, 64, L"sx_mem_report_%d.txt", s_mem_corruptions );
			mem_report_debug_to_file( tmp, -1 );

#if ( defined(_DEBUG) || SEGAN_ASSERT )
			//	report call stack
			lib_assert( "memory block has been corrupted !", memreport.mb->file, memreport.mb->line );
#endif
		}
		else
		{
			//	pop memory block from the list
			mem_debug_push_pop( memreport.mb, true );
		}

		//	realloc the memory block
		void* tmp = mem_realloc( memreport.mb, sizeof(MemBlock) + s_mem_protect_size + newsizeinbyte + s_mem_protect_size );

		//	sign memory to check protection
		memreport = mem_code_protection( tmp, newsizeinbyte );

		if ( memreport.mb )
		{
			memreport.mb->file = (char*)file;
			memreport.mb->line = line;
			memreport.mb->size = newsizeinbyte;
			memreport.mb->tag = s_mem_tag;

			//	push new block
			mem_debug_push_pop( memreport.mb );
		}

		//	set as result
		return memreport.p;
	}
	else
	{
		if ( s_mem_enable_leak )
		{
			//	realloc the memory block
			p = mem_realloc( p, sizeof(MemBlock) + s_mem_protect_size + newsizeinbyte + s_mem_protect_size );

			//	sign memory to check protection
			memreport = mem_code_protection( p, newsizeinbyte );

			if ( memreport.mb )
			{
				memreport.mb->file = (char*)file;
				memreport.mb->line = line;
				memreport.mb->size = newsizeinbyte;
				memreport.mb->tag = s_mem_tag;

				//	push the block
				mem_debug_push_pop( memreport.mb );
			}

			//	set as result
			return memreport.p;
		}
	}

	return mem_realloc( p, newsizeinbyte );
}

SEGAN_INLINE void* mem_free_dbg( const void* p )
{
	if ( !p ) return null;

	MemCodeReport memreport = mem_decode_protection( p );

	if ( memreport.mb )
	{
		//	if memory has been corrupted we should hold the corrupted memory info
		if ( memreport.mb->corrupted )
		{
			//	report memory allocations to file
			wchar tmp[64] = {0};
			swprintf_s( tmp, 64, L"sx_mem_report_%d.txt", s_mem_corruptions );
			mem_report_debug_to_file( tmp, -1 );

#if ( defined(_DEBUG) || SEGAN_ASSERT )
			//	report call stack
			lib_assert( "memory block has been corrupted !", memreport.mb->file, memreport.mb->line );
#endif
		}
		else
		{
			//	pop memory block from the list
			mem_debug_push_pop( memreport.mb, true );
			mem_free( memreport.mb );
		}
	}
	else mem_free( p );

	return null;
}

uint mem_report_compute_num( const uint tag )
{
	if ( !s_mem_root ) return 0;

	uint result = 0;
	MemBlock* leaf = s_mem_root;
	if (tag)
	{
		if (tag == -1)
		{
			while (leaf)
			{
				mem_check_protection(leaf);
				if (leaf->corrupted)
					result++;
				leaf = leaf->next;
			}
		}
		else
		{
			while (leaf)
			{
				if (leaf->tag == tag)
				{
					mem_check_protection(leaf);
					result++;
				}
				leaf = leaf->next;
			}
		}
	}
	else
	{
		while (leaf)
		{
			mem_check_protection(leaf);
			result++;
			leaf = leaf->next;
		}
	}

	return result;
}

SEGAN_INLINE void mem_report_debug( CB_Memory callback, void* userdata, const uint tag /*= 0 */ )
{
	if ( !s_mem_root || !callback ) return;

	MemBlock* leaf = s_mem_root;
	if ( tag )
	{
		while ( leaf )
		{
			if ( leaf->tag == tag )
			{
				mem_check_protection( leaf );
				(*callback)( userdata, leaf->file, leaf->line, leaf->size, leaf->tag, leaf->corrupted );
			}
			leaf = leaf->next;
		}
	}
	else
	{
		while ( leaf )
		{
			mem_check_protection( leaf );
			(*callback)( userdata, leaf->file, leaf->line, leaf->size, leaf->tag, leaf->corrupted );
			leaf = leaf->next;
		}
	}
}

#if defined(DEBUG_OUTPUT_WINDOW)
void mem_report_debug_to_window( const uint tag /*= 0 */ )
{
	if ( !s_mem_root ) return;

	OutputDebugString( L"seganx memory report : \n" );

	MemBlock* leaf = s_mem_root;
	if ( tag )
	{
		while ( leaf )
		{
			if ( leaf->tag == tag )
			{
				mem_check_protection( leaf );
				wchar tmp[1024] = { 0 };
				mem_block_to_str(tmp, 1024, leaf);
				OutputDebugString( tmp );
			}
			leaf = leaf->next;
		}
	}
	else
	{
		while ( leaf )
		{
			mem_check_protection( leaf );
			wchar tmp[1024] = {0};
			mem_block_to_str(tmp, 1024, leaf);
			OutputDebugString( tmp );
			leaf = leaf->next;
		}
	}
}
#endif

SEGAN_INLINE void mem_report_debug_to_file( const wchar* fileName, const uint tag /*= 0 */ )
{
	if ( !s_mem_root ) return;

#if defined(DEBUG_OUTPUT_WINDOW)
	mem_report_debug_to_window( 0 );
#endif

	if ( mem_report_compute_num(tag) < 1 ) return;

	//	report called functions in the file
	FILE* f = null;
	if( _wfopen_s( &f, fileName, L"w, ccs=UNICODE" ) == 0 )
	{
		fputws( L"seganx memory debug report : \n\n", f );
		MemBlock* leaf = s_mem_root;
		if ( tag )
		{
			if ( tag == -1 )
			{
				while ( leaf )
				{
					mem_check_protection( leaf );

					if ( leaf->corrupted )
					{
						wchar tmp[1024] = {0};
						mem_block_to_str(tmp, 1024, leaf);
						fputws( tmp, f );
					}

					leaf = leaf->next;
				}
			}
			else
			{
				while ( leaf )
				{
					if ( leaf->tag == tag )
					{
						mem_check_protection( leaf );
						wchar tmp[1024] = {0};
						mem_block_to_str(tmp, 1024, leaf);
						fputws( tmp, f );
					}
					leaf = leaf->next;
				}
			}
		}
		else
		{
			while ( leaf )
			{
				mem_check_protection( leaf );
				wchar tmp[1024] = {0};
				mem_block_to_str(tmp, 1024, leaf);
				fputws( tmp, f );
				leaf = leaf->next;
			}
		}
		fclose( f );
	}
}

SEGAN_INLINE void mem_clear_debug( void )
{
	MemBlock* leaf = s_mem_root;
	while ( leaf )
	{
		s_mem_root = leaf->next;
		mem_free( leaf );
		leaf = s_mem_root;
	}
}

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////
//	MEMORY MANAGER : FREE LIST POOL
//	a fast general memory pool which allocates a memory block in 
//	initialization from OS and uses the allocated 
//	memory pool in any allocation call. using this memory manager has 
//	restriction of block protection.
//////////////////////////////////////////////////////////////////////////
MemMan_Pool::MemMan_Pool( const uint poolSizeInBytes ): MemMan()
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

MemMan_Pool::~MemMan_Pool( void )
{
	mem_free( m_pool );
}

void* MemMan_Pool::alloc( const uint sizeInByte )
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

SEGAN_INLINE void* MemMan_Pool::realloc( const void* p, const uint sizeInByte )
{
	if ( !p )
	{
		return alloc( sizeInByte );
	}
	else
	{
		sx_assert( pbyte(p) > m_pool && pbyte(p) < ( m_pool + mem_size(m_pool) ) );
		void* tmp = alloc( sizeInByte );
		mem_copy( tmp, p, mem_min_size( size(p), sizeInByte ) );
		free( p );
		return tmp;
	}
}

SEGAN_INLINE void* MemMan_Pool::free( const void* p )
{
	if ( !p ) return null;
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
			return null;	//	this one is already exist in list
		}
	}

	//	push to free list
	ch->state = CS_EMPTY;
	push( ch );
	return null;
}

SEGAN_INLINE uint MemMan_Pool::size( const void* p )
{
	if ( !p ) return 0;
	sx_assert( pbyte(p) > m_pool && pbyte(p) < ( m_pool + mem_size(m_pool) ) );
	Chunk* ch = PChunk( pbyte(p) - sizeof(Chunk) );
	return ch->size;
}


SEGAN_INLINE void MemMan_Pool::push( Chunk* ch )
{
	ch->next = m_root;
	ch->prev = null;
	m_root->prev = ch;
	m_root = ch;
}

SEGAN_INLINE void MemMan_Pool::pop( Chunk* ch )
{
	if(ch->prev)	ch->prev->next = ch->next;
	if(ch->next)	ch->next->prev = ch->prev;
	if(ch==m_root)	m_root = ch->next;
}