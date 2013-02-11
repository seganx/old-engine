#include "Lib.h"
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


#if defined(_WIN32)
#include <Windows.h>
CRITICAL_SECTION	s_mem_cs;
SEGAN_INLINE void lib_init_cs( void )
{
	InitializeCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_finit_cs( void )
{
	DeleteCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_enter_cs( void )
{
	EnterCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_leave_cs( void )
{
	LeaveCriticalSection( &s_mem_cs );
}
#endif

// initialize internal library
extern void sx_lib_initialize( void );

// finalize internal library
extern void sx_lib_finalize( void );


#if ( defined(_DEBUG) || SEGAN_CALLSTACK )


SEGAN_INLINE sint lib_assert( const wchar* expression, const wchar* file, const sint line )
{

#if ( SEGAN_CALLSTACK == 1 )
	_CallStack _callstack( file, line, L"assertion '%s'", expression );
#endif

#if defined(_DEBUG)
	__debugbreak();	//	just move your eyes down and look at the call stack list in IDE to find out what happened !
#else
	callstack_report_to_file( L"sx_assertion", L"assertion failed !" );
#endif

	return 0;
}

#endif

#if SEGAN_CALLSTACK

//////////////////////////////////////////////////////////////////////////
//	simple call stack system
//////////////////////////////////////////////////////////////////////////
/* call stack system should be safe. o memory allocation or using 
any other service so we need a pool and fill it consecutively. */
#define CALLSTACK_MAX	128

struct CallStackData
{
#if SEGAN_CALLSTACK_PARAMS
	wchar	function[512];
#else
	wchar*	function;
#endif
	wchar*	file;
	sint	line;
};
CallStackData	callstack_pool[CALLSTACK_MAX];
uint			callstack_index = 0;
bool			callstack_end	= false;

SEGAN_INLINE void callstack_clean( CallStackData *csd )
{
	csd->file = null;
	csd->line = 0;
#if SEGAN_CALLSTACK_PARAMS
	csd->function[0] = 0;
#else
	csd->function = null;
#endif
}

SEGAN_INLINE _CallStack::_CallStack( const wchar* file, const sint line, const wchar* function, ... )
{
	if ( callstack_index < CALLSTACK_MAX )
	{
		lib_enter_cs();

		CallStackData* csd = &callstack_pool[callstack_index++];

		csd->file = (wchar*)file;
		if ( csd->file )
		{
			wchar *s = csd->file;
			wchar *c1 = s;
			wchar *c2 = s;
			while ( c1 || c2 )
			{
				c1 = wcsstr( s, L"\\" );
				c2 = wcsstr( s, L"/" );
				if ( c1 ) s = ++c1;
				if ( c2 ) s = ++c2;
			}
			csd->file = s;
		}

		csd->line = line;
		if ( function )
		{
#if SEGAN_CALLSTACK_PARAMS
			va_list argList;
			va_start( argList, function );
			sint len = _vscwprintf( function, argList);
			if ( len < 511 )
				vswprintf_s( csd->function, 511, function, argList );
			else
				String::Copy( csd->function, 511, function );
			va_end( argList );
#else
			csd->function = (wchar*)function;
#endif
		}
		else
			csd->function = null;

		lib_leave_cs();
	}
}

SEGAN_INLINE _CallStack::~_CallStack( void )
{
	if ( callstack_index > 0 )
	{
		callstack_clean( &callstack_pool[--callstack_index] );
	}
}

void callstack_report( CallStack_Callback callback )
{
	if ( !callback ) return;

	for ( int i=0; i<CALLSTACK_MAX; i++ )
	{
		CallStackData* csd = &callstack_pool[i];
		if ( csd->line && csd->file )
			(*callback)( csd->file, csd->line, csd->function );
	}
}

void callstack_clear( void )
{
	for ( int i=0; i<CALLSTACK_MAX; i++ )
	{
		callstack_clean( &callstack_pool[i] );
	}
}

void detect_crash( void )
{
	callstack_end = true;
}

void callstack_report_to_file( const wchar* name, const wchar* title /*= L" "*/ )
{
	lib_enter_cs();

	wchar strName[128];

	//	make a name
	{
		time_t rawTime;
		time( &rawTime );
		struct tm timeInfo;
		localtime_s( &timeInfo, &rawTime );
		wchar strTime[64];
		wcsftime( strTime, 64, L"%H_%M_%S", &timeInfo );
		static int logCount = 0;
		swprintf_s( strName, 128, L"%s_%s_%d.txt", name, strTime, ++logCount );
	}
	
	//	report called functions in the file
	FILE* f = null;
	if( _wfopen_s( &f, strName, L"w, ccs=UNICODE" ) == 0 )
	{
		fwprintf( f, L"seganx call stack report : %s\n\n", title );

		//	compute maximum length of every line
		sint maxlength = 0;
		for ( int i=0; i<CALLSTACK_MAX; i++ )
		{
			CallStackData* csd = &callstack_pool[i];
			if ( csd->line && csd->file )
			{
				str1024 tmp;
				tmp << csd->file << '(' << csd->line << L") : ";
				if ( maxlength < tmp.Length() )
					maxlength = tmp.Length();
			}
		}

		//	write lines to the file
		for ( int i=0; i<CALLSTACK_MAX; i++ )
		{
			CallStackData* csd = &callstack_pool[i];
			if ( csd->line && csd->file )
			{
				str1024 tmp;
				tmp << csd->file << '(' << csd->line << L") : ";

				sint len = tmp.Length();
				for ( sint spaces = maxlength + 5; spaces > len; spaces-- )
					tmp << ' ';

				if ( csd->function )
					tmp << csd->function << '\n';
				else
					tmp << L"no name !\n";

				fputws( tmp.Text(), f );
			}
		}
		fclose( f );
	}

	lib_leave_cs();
}


#endif



//////////////////////////////////////////////////////////////////////////
//	memory management
//////////////////////////////////////////////////////////////////////////
static MemMan*			s_manager			= null;

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
	return s_manager ? s_manager->Alloc( sizeinbyte ) : ::malloc( sizeinbyte );
}

SEGAN_INLINE void mem_realloc( void*& p, const uint newsizeinbyte )
{
	if ( !newsizeinbyte )
	{
		sx_mem_free( p );
		p = null;
		return;
	}

	if ( s_manager )
	{
		s_manager->Realloc( p, newsizeinbyte );
		return;
	}

	void* newptr = ::realloc( p, newsizeinbyte );
	if ( newptr )
	{
		p = newptr;
	}
	else
	{
		if ( p )
		{
			//  if reallocate function failed then try to allocate new one and copy last data to new pool
			newptr = ::malloc( newsizeinbyte );
			memcpy( newptr, p, sx_min_i( sx_mem_size( p ), newsizeinbyte ) );
			::free( p );
			p = newptr;
		}
		else
		{
			p = ::malloc( newsizeinbyte );
		}
	}
}

SEGAN_INLINE uint mem_size( const void* p )
{
	if ( !p ) return 0;
	return s_manager ? s_manager->Size( p ) : (uint)::_msize( (void*)p );	
}

SEGAN_INLINE void mem_free( const void* p )
{
	if ( s_manager )
	{
		s_manager->Free( p );
	}
	else
	{
		::free( (void*)p );
	}
}

SEGAN_INLINE void mem_copy( void* dest, const void* src, const uint size )
{
	memcpy( dest, src, size );
}


SEGAN_INLINE void mem_set( void* dest, const sint value, const uint size )
{
	memset( dest, value, size );
}



//////////////////////////////////////////////////////////////////////////
//	debug allocator used to detect memory leaks
#if ( SEGAN_MEMLEAK == 1 )

struct MemBlock
{
	void*		mem;
	wchar*		file;
	uint		line;
	uint		size;
	uint		tag;
	bool		corrupted;
	MemBlock*	next;
};
static MemBlock*		s_mem_root			= null;
static bool				s_mem_enable_leak	= false;
static uint				s_mem_tag			= 0;
static uint				s_mem_last_tag		= 0;
static char*			s_mem_protection	= "!protected area!";
static uint				s_mem_protect_size	= 16;
static sint				s_mem_corruptions	= 0;

// use an static memory pool to hold data
static MemMan_Pool		s_mem_pool( 10 * 1024 * 1024 );


SEGAN_INLINE void* mem_code_protection( void* p, const uint realsizeinbyte )
{
	if ( !p ) return null;

	//	sign beginning of memory block
	u32* first = (u32*)p;
	memcpy( first, s_mem_protection, s_mem_protect_size );

	//	sign end of memory block
	u32* last = (u32*)( pbyte(p) + realsizeinbyte + s_mem_protect_size );
	memcpy( last, s_mem_protection, s_mem_protect_size );

	//	prepare pointer
	return ( pbyte(p) + s_mem_protect_size );
}

struct MemDecodedReport
{
	void*	p;			//	decoded memory address
	bool	corrupted;	//	memory has been corrupted
};
SEGAN_INLINE MemDecodedReport mem_decode_protection( const void* p, const uint realsizeinbyte )
{
	MemDecodedReport res;
	if ( !p )
	{
		res.p = null;
		res.corrupted = false;
		return res;
	}

	res.p = ( pbyte(p) - s_mem_protect_size );

	//	check beginning of memory block
	u32* first = (u32*)( res.p );
	if ( memcmp( first, s_mem_protection, s_mem_protect_size ) )
	{
		res.corrupted = true;
	}
	else
	{
		//	check end of memory block
		u32* last = (u32*)( pbyte(p) + realsizeinbyte );
		if ( memcmp( last, s_mem_protection, s_mem_protect_size ) )
			res.corrupted = true;
		else
			res.corrupted = false;
	}

	return res;
}

SEGAN_INLINE void mem_debug_push( MemBlock* mb )
{
	if ( !s_mem_root )
	{
		mb->next = null;
		s_mem_root = mb;
	}
	else
	{
		mb->next = s_mem_root;
		s_mem_root = mb;
	}
}

SEGAN_INLINE MemBlock* mem_debug_pop( const void* mem )
{
	if ( !s_mem_root || !mem ) return null;

	if ( s_mem_root->mem == mem )
	{
		MemBlock* res = s_mem_root;
		s_mem_root = res->next;
		return res;
	}

	MemBlock* prev = s_mem_root;	
	MemBlock* curr = s_mem_root->next;
	while ( curr )
	{
		if ( curr->mem == mem )
		{
			prev->next = curr->next;
			return curr;
		}
		prev = curr;
		curr = curr->next;
	}

	return null;
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

SEGAN_INLINE void* mem_alloc_dbg( const uint sizeinbyte, const wchar* file, const int line )
{
	void* res = null;

	if ( s_mem_enable_leak )
	{
		res = mem_alloc( sizeinbyte + 2 * s_mem_protect_size );

		//	sign memory to check memory corruption
		res = mem_code_protection( res, sizeinbyte );

		/*store memory info. we must allocate a separate memory block info.
		because it kept safe even if the user memory has been corrupted  */
		lib_enter_cs();
		MemBlock* mb = (MemBlock*)s_mem_pool.Alloc( sizeof(MemBlock) );
		if ( mb )
		{
			mb->mem = res;
			mb->file = (wchar*)file;
			mb->line = line;
			mb->size = sizeinbyte;
			mb->tag = s_mem_tag;
			mb->corrupted = false;
			mem_debug_push( mb );
		}
		lib_leave_cs();
	}
	else
	{
		res = mem_alloc( sizeinbyte );
	}

	return res;
}

SEGAN_INLINE void mem_realloc_dbg( void*& p, const uint newsizeinbyte, const wchar* file, const int line )
{
	if ( !newsizeinbyte )
	{
		mem_free_dbg( p );
		return;
	}

	lib_enter_cs();
	MemBlock* mb = mem_debug_pop( p );
	if ( mb )
	{
		//	decode memory to check corruption
		MemDecodedReport memblock = mem_decode_protection( p, mb->size );

		//	if memory has been corrupted we should hold the corrupted memory info
		if ( memblock.corrupted )
		{
			mb->corrupted = true;
			mb->mem = null;
			mem_debug_push( mb );
			mb = null;

			//	report memory allocations to file
			str64 memreportfile = L"sx_mem_report_"; memreportfile << s_mem_corruptions; memreportfile << L".txt";
			mem_report_debug_to_file( memreportfile, -1 );

			//	report call stack
			lib_assert( L"memory block has been corrupted !", mb->file, mb->line );

			//	store new memory info
			mb = (MemBlock*)s_mem_pool.Alloc( sizeof(MemBlock) );
			if ( mb )
				mb->corrupted = false;
		}

		//	realloc the memory block
		mem_realloc( memblock.p, newsizeinbyte + 2 * s_mem_protect_size );

		//	sign memory to check protection
		p = mem_code_protection( memblock.p, newsizeinbyte );

		if ( mb )
		{
			mb->mem = p;
			mb->file = (wchar*)file;
			mb->line = line;
			mb->size = newsizeinbyte;
			mb->tag = s_mem_tag;
			mem_debug_push( mb );
		}
	}
	else
	{
		if ( s_mem_enable_leak )
		{
			//	realloc the memory block
			mem_realloc( p, newsizeinbyte + 2 * s_mem_protect_size );

			//	sign memory to check memory corruption
			p = mem_code_protection( p, newsizeinbyte );

			//	store memory info
			mb = (MemBlock*)s_mem_pool.Alloc( sizeof(MemBlock) );
			if ( mb )
			{
				mb->mem = p;
				mb->file = (wchar*)file;
				mb->line = line;
				mb->size = newsizeinbyte;
				mb->tag = s_mem_tag;
				mb->corrupted = false;
				mem_debug_push( mb );
			}
		}
		else
		{
			mem_realloc( p, newsizeinbyte );
		}
	}
	lib_leave_cs();
}

SEGAN_INLINE void mem_free_dbg( const void* p )
{
	if ( !p ) return;

	lib_enter_cs();

	MemBlock* mb = mem_debug_pop( p );
	if ( mb )
	{
		MemDecodedReport mem = mem_decode_protection( p, mb->size );
		if ( mem.corrupted )
		{
			mb->corrupted = true;
			mb->mem = null;
			mem_debug_push( mb );

			//	report memory allocations to file
			str64 memreportfile = L"sx_mem_report_"; memreportfile << s_mem_corruptions; memreportfile << L".txt";
			mem_report_debug_to_file( memreportfile, -1 );

			//	report call stack
			lib_assert( L"memory block has been corrupted !", mb->file, mb->line );
		}
		else
			s_mem_pool.Free( mb );

		mem_free( mem.p );
	}
	else
		mem_free( p );

	lib_leave_cs();
}

SEGAN_INLINE void mem_report_debug( CB_Memory callback, const uint tag /*= 0 */ )
{
	if ( !s_mem_root || !callback ) return;

	MemBlock* leaf = s_mem_root;
	if ( tag )
	{
		while ( leaf )
		{
			if ( leaf->tag == tag )
			{
				if ( leaf->mem )
					leaf->corrupted = mem_decode_protection( leaf->mem, leaf->size ).corrupted;
				(*callback)( leaf->file, leaf->line, leaf->size, leaf->tag, leaf->corrupted );
			}
			leaf = leaf->next;
		}
	}
	else
	{
		while ( leaf )
		{
			if ( leaf->mem )
				leaf->corrupted = mem_decode_protection( leaf->mem, leaf->size ).corrupted;
			(*callback)( leaf->file, leaf->line, leaf->size, leaf->tag, leaf->corrupted );
			leaf = leaf->next;
		}
	}
}

#if defined(_DEBUG)
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
				if ( leaf->mem )
					leaf->corrupted = mem_decode_protection( leaf->mem, leaf->size ).corrupted;

				str1024 tmp;
				if ( leaf->corrupted )
					tmp << leaf->file << L"(" << leaf->line << L"): error : memory corrupted : size = " << leaf->size << L" tag = " << leaf->tag;
				else
					tmp << leaf->file << L"(" << leaf->line << L"): warning : memory leak : size = " << leaf->size << L" tag = " << leaf->tag;
				tmp << L"\n";

				OutputDebugString( tmp.Text() );
			}
			leaf = leaf->next;
		}
	}
	else
	{
		while ( leaf )
		{
			if ( leaf->mem )
				leaf->corrupted = mem_decode_protection( leaf->mem, leaf->size ).corrupted;

			str1024 tmp;
			if ( leaf->corrupted )
				tmp << leaf->file << L"(" << leaf->line << L"): error : memory corrupted : size = " << leaf->size << L" tag = " << leaf->tag;
			else
				tmp << leaf->file << L"(" << leaf->line << L"): warning : memory leak : size = " << leaf->size << L" tag = " << leaf->tag;
			tmp << L"\n";

			OutputDebugString( tmp.Text() );

			leaf = leaf->next;
		}
	}
}
#endif

SEGAN_INLINE void mem_report_debug_to_file( const wchar* fileName, const uint tag /*= 0 */ )
{
	if ( !s_mem_root ) return;

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
					if ( leaf->mem )
						leaf->corrupted = mem_decode_protection( leaf->mem, leaf->size ).corrupted;

					if ( leaf->corrupted )
					{
						str1024 tmp;
						tmp << leaf->file << L"(" << leaf->line << L"): error : memory corrupted : size = " << leaf->size << L" tag = " << leaf->tag << L"\n";
						fputws( tmp.Text(), f );
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
						if ( leaf->mem )
							leaf->corrupted = mem_decode_protection( leaf->mem, leaf->size ).corrupted;

						str1024 tmp;
						if ( leaf->corrupted )
							tmp << leaf->file << L"(" << leaf->line << L"): error : memory corrupted : size = " << leaf->size << L" tag = " << leaf->tag;
						else
							tmp << leaf->file << L"(" << leaf->line << L"): warning : memory leak : size = " << leaf->size << L" tag = " << leaf->tag;
						tmp << L"\n";

						fputws( tmp.Text(), f );
					}
					leaf = leaf->next;
				}
			}
		}
		else
		{
			while ( leaf )
			{
				if ( leaf->mem )
					leaf->corrupted = mem_decode_protection( leaf->mem, leaf->size ).corrupted;

				str1024 tmp;
				if ( leaf->corrupted )
					tmp << leaf->file << L"(" << leaf->line << L"): error : memory corrupted : size = " << leaf->size << L" tag = " << leaf->tag;
				else
					tmp << leaf->file << L"(" << leaf->line << L"): warning : memory leak : size = " << leaf->size << L" tag = " << leaf->tag;
				tmp << L"\n";

				fputws( tmp.Text(), f );

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
		s_mem_pool.Free( leaf );
		leaf = s_mem_root;
	}
}

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//	fast sine/cosine lookup table.
//	code from : http://codepaste.ru/2483/

#define angle2int( angle )			( sint( (angle) * _ANGLE2INT ) )
#define int2angle( val )			( (val) * _INT2ANGLE )

const sint	_SINCOS_NUM_BITS		= 12;
const sint	_SINCOS_TABLE_SIZE   	= 1 << _SINCOS_NUM_BITS;
const sint	SINCOS_TABLE_SHIFT  	= 16 - _SINCOS_NUM_BITS;
const sint	_SINCOS_TABLE_MASK   	= _SINCOS_TABLE_SIZE - 1;
const sint	_SINCOS_PI           	= 32768;
const sint	_SINCOS_2PI          	= 2 * _SINCOS_PI;
const sint	_SINCOS_PI_DIV_2     	= _SINCOS_PI / 2;
const float _ANGLE2INT				= float(_SINCOS_PI / PI);
const float _INT2ANGLE				= float(PI / _SINCOS_PI);

__declspec (align(16))  float sin_table[_SINCOS_TABLE_SIZE];

SEGAN_INLINE float sx_sin_fast( const float x )
{
	sint a = angle2int( x );
	return sin_table[ ( a >> SINCOS_TABLE_SHIFT ) & _SINCOS_TABLE_MASK ];
}

SEGAN_INLINE float sx_cos_fast( const float x )
{
	sint a = angle2int( x ) + _SINCOS_PI_DIV_2;
	return sin_table[ ( a >> SINCOS_TABLE_SHIFT ) & _SINCOS_TABLE_MASK ];
}

SEGAN_INLINE void sx_sin_cos_fast( const float IN x, float& OUT s, float& OUT c)
{
	sint a = angle2int( x );
	s = sin_table[ ( a >> SINCOS_TABLE_SHIFT ) & _SINCOS_TABLE_MASK ];
	a += _SINCOS_PI_DIV_2;
	c = sin_table[ ( a >> SINCOS_TABLE_SHIFT ) & _SINCOS_TABLE_MASK ];
}



//////////////////////////////////////////////////////////////////////////
//	table of randomize number
//////////////////////////////////////////////////////////////////////////
#define RND_TABLE_SIZE 4096
uint rnd_table[RND_TABLE_SIZE];
uint rnd_index = 0;

SEGAN_INLINE void sx_random_set_data( const uint* data )
{
	memcpy( rnd_table, data, sizeof(rnd_table) );
}


SEGAN_INLINE void sx_random_sync( const uint index )
{
	rnd_index = index;
}


SEGAN_INLINE float sx_random_f( const float range )
{
	lib_enter_cs();
	if ( ++rnd_index >= RND_TABLE_SIZE ) rnd_index = 0;
	lib_leave_cs();
	return  ( range * rnd_table[rnd_index] ) / RAND_MAX;
}

SEGAN_INLINE sint sx_random_i( const sint range )
{
	lib_enter_cs();
	if ( ++rnd_index >= RND_TABLE_SIZE ) rnd_index = 0;
	lib_leave_cs();
	return  ( range * rnd_table[rnd_index] ) / RAND_MAX;
}


//////////////////////////////////////////////////////////////////////////
//	string functions

SEGAN_INLINE const wchar* IntToStr( const int number )
{
	lib_enter_cs();
	static wchar tmp[64];
	_itow_s(number, tmp, 64, 10);
	lib_leave_cs();
	return tmp;
}

SEGAN_LIB_API const wchar* FloatToStr( float number, int precision )
{
	lib_enter_cs();
	static str128 res;
	if ( abs(number)<0.000001f )
		number = 0;
	char tmp[64];
	int decimal, sign;
	_fcvt_s(tmp, 64, number, precision, &decimal, &sign);
	res = tmp;
	if(precision>0)
	{
		if (decimal>0) res.Insert('.', decimal);
		else
		{
			for (int i=decimal; i<=0; res.Insert('0'), i++);
			res.Insert('.', 1);
		}
	}
	else if(decimal<0) res.Insert('0');
	if (sign && str128::StrToFloat(res)>0) res.Insert('-');
	lib_leave_cs();
	return res;
}





//////////////////////////////////////////////////////////////////////////
// initialize internal library
void sx_lib_initialize( void )
{
	lib_init_cs();

#if SEGAN_MEMLEAK
	mem_enable_debug( true, 0 );
#endif

#if SEGAN_CALLSTACK
	callstack_clear();
#endif

	//  fill sine/cosine table
	for ( sint i = 0; i < _SINCOS_TABLE_SIZE; i++ )
	{
		const float angle = int2angle( i << SINCOS_TABLE_SHIFT );
		sin_table[i] = sinf(angle);
	}

	//  initialize random seed
	srand( (uint)time(NULL) );

	//	fill random table
	const uint halfrand = RAND_MAX / 2;
	for ( int i = 0; i < RND_TABLE_SIZE; i++ )
	{
		if ( rand() > halfrand )
		{
			rand();
			rnd_table[i] = rand();
		}
		else rnd_table[i] = rand();
	}
}

// finalize internal library
void sx_lib_finalize( void )
{

#if SEGAN_CALLSTACK
	if ( !callstack_end )	//	verify that application crashed
	{
		callstack_report_to_file( L"sx_crash_report", L"application closed unexpectedly !" );

#if SEGAN_MEMLEAK
		mem_report_debug_to_file( L"sx_mem_corruption_report.txt", -1 );
		mem_clear_debug();
#endif

	}
#if SEGAN_MEMLEAK
	else
	{
#if defined(_DEBUG)
	mem_report_debug_to_window( 0 );
#endif
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
	}
#endif

	callstack_clear();
#elif SEGAN_MEMLEAK
#if defined(_DEBUG)
	mem_report_debug_to_window( 0 );
#endif
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
#endif


	lib_finit_cs();
}

