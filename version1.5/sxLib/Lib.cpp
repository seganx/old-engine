#include "Lib.h"
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(_DEBUG)
#if defined(_MSC_VER)
#if ( _MSC_VER >= 1400 )
#define DEBUG_OUTPUT_WINDOW
#endif
#endif
#endif

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


#if ( defined(_DEBUG) || SEGAN_LIB_ASSERT )


SEGAN_INLINE sint lib_assert( const wchar* expression, const wchar* file, const sint line )
{

#if ( SEGAN_CALLSTACK == 1 )
	_CallStack _callstack( line, file, L"assertion '%s'", expression );
#endif

#if defined(_DEBUG)
	__debugbreak();	//	just move your eyes down and look at the call stack list in IDE to find out what happened !
#else

#if ( SEGAN_CALLSTACK == 1 )
	callstack_report_to_file( L"sx_assertion", L"assertion failed !" );
#endif

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
#define CALLSTACK_MAX	64

struct CallStackData
{
	wchar	name[1024];
	wchar*	function;
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
	csd->name[0] = 0;
	csd->function = null;
}

SEGAN_INLINE _CallStack::_CallStack( const wchar* file, const sint line, const wchar* function )
{
	if ( callstack_index < CALLSTACK_MAX )
	{
		lib_enter_cs();

		CallStackData* csd = &callstack_pool[callstack_index++];

		csd->file = (wchar*)file;
#if 0
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
#endif

		csd->line = line;
		csd->name[0] = 0;
		if ( function )
		{
			csd->function = (wchar*)function;
		}
		else
			csd->function = null;

		lib_leave_cs();
	}
}

SEGAN_INLINE _CallStack::_CallStack( const sint line, const wchar* file, const wchar* function, ... )
{
	if ( callstack_index < CALLSTACK_MAX )
	{
		lib_enter_cs();

		CallStackData* csd = &callstack_pool[callstack_index++];

		csd->file = (wchar*)file;
#if 0
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
#endif

		csd->line = line;
		csd->function = null;
		if ( function )
		{
			va_list argList;
			va_start( argList, function );
			sint len = _vscwprintf( function, argList);
			if ( len < 1023 )
				vswprintf_s( csd->name, 1023, function, argList );
			else
				String::Copy( csd->name, 1023, function );
			va_end( argList );
		}
		else
		{
			csd->name[0] = null;
		}

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
			(*callback)( csd->file, csd->line, csd->function ? csd->function : csd->name );
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
				else if ( csd->name[0] )
					tmp << csd->name << '\n';
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
		mem_free( p );
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
	char		sign[16];
	wchar*		file;
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
static char*			s_mem_protection	= "!protected area!";
static uint				s_mem_protect_size	= 16;
static sint				s_mem_corruptions	= 0;


struct MemCodeReport
{
	void*		p;		//	user coded memory
	MemBlock*	mb;		//	memory block for log
};

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
		if ( memcmp( p , s_mem_protection, s_mem_protect_size ) )
			mb->corrupted = true;
		else
			mb->corrupted = false;
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

	// extract mem block
	res.mb = (MemBlock*)p;
	res.mb->corrupted = false;

	//	set protection sign for mem block
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
		res.p	= res.mb;

		// check protection sign for mem block
		if ( memcmp( res.mb->sign, s_mem_protection, s_mem_protect_size ) )
		{
			res.p = null;
			res.mb = null;
		}
		else mem_check_protection( res.mb );
	}

	return res;
}

SEGAN_INLINE void mem_debug_push( MemBlock* mb )
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

SEGAN_INLINE void mem_debug_pop( MemBlock* mb )
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
		lib_enter_cs();

		//	first block for holding data, second block for protection, memory and close with other protection
		res = mem_alloc( sizeof(MemBlock) + s_mem_protect_size + sizeinbyte + s_mem_protect_size );

		if ( res )
		{
			//	sign memory to check memory corruption
			MemCodeReport memreport = mem_code_protection( res, sizeinbyte );

			//	store mem block to link list
			if ( memreport.mb )
			{
				memreport.mb->file	= (wchar*)file;
				memreport.mb->line	= line;
				memreport.mb->size	= sizeinbyte;
				memreport.mb->tag	= s_mem_tag;
		
				mem_debug_push( memreport.mb );
			}

			//	set user memory as result
			res = memreport.p;
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

	MemCodeReport memreport = mem_decode_protection( p );

	if ( memreport.mb )
	{
		//	if memory has been corrupted we should hold the corrupted memory info
		if ( memreport.mb->corrupted )
		{
			//	report memory allocations to file
			str64 memreportfile = L"sx_mem_report_"; memreportfile << s_mem_corruptions; memreportfile << L".txt";
			mem_report_debug_to_file( memreportfile, -1 );

#if ( defined(_DEBUG) || SEGAN_LIB_ASSERT )
			//	report call stack
			lib_assert( L"memory block has been corrupted !", memreport.mb->file, memreport.mb->line );
#endif
		}
		else
		{
			//	pop mem block from the list
			mem_debug_pop( memreport.mb );
		}

		//	realloc the memory block
		mem_realloc( memreport.p, sizeof(MemBlock) + s_mem_protect_size + newsizeinbyte + s_mem_protect_size );

		//	sign memory to check protection
		memreport = mem_code_protection( memreport.p, newsizeinbyte );

		if ( memreport.mb )
		{
			memreport.mb->file = (wchar*)file;
			memreport.mb->line = line;
			memreport.mb->size = newsizeinbyte;
			memreport.mb->tag = s_mem_tag;

			mem_debug_push( memreport.mb );
		}

		//	set as result
		p = memreport.p;
	}
	else
	{
		if ( s_mem_enable_leak )
		{
			//	realloc the memory block
			mem_realloc( p, sizeof(MemBlock) + s_mem_protect_size + newsizeinbyte + s_mem_protect_size );

			//	sign memory to check protection
			memreport = mem_code_protection( p, newsizeinbyte );

			if ( memreport.mb )
			{
				memreport.mb->file = (wchar*)file;
				memreport.mb->line = line;
				memreport.mb->size = newsizeinbyte;
				memreport.mb->tag = s_mem_tag;

				mem_debug_push( memreport.mb );
			}

			//	set as result
			p = memreport.p;
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

	MemCodeReport memreport = mem_decode_protection( p );

	if ( memreport.mb )
	{
		//	if memory has been corrupted we should hold the corrupted memory info
		if ( memreport.mb->corrupted )
		{
			//	report memory allocations to file
			str64 memreportfile = L"sx_mem_report_"; memreportfile << s_mem_corruptions; memreportfile << L".txt";
			mem_report_debug_to_file( memreportfile, -1 );

#if ( defined(_DEBUG) || SEGAN_LIB_ASSERT )
			//	report call stack
			lib_assert( L"memory block has been corrupted !", memreport.mb->file, memreport.mb->line );
#endif
		}
		else
		{
			//	pop mem block from the list
			mem_debug_pop( memreport.mb );
			mem_free( memreport.p );
		}
	}
	else mem_free( p );

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
				mem_check_protection( leaf );
				(*callback)( leaf->file, leaf->line, leaf->size, leaf->tag, leaf->corrupted );
			}
			leaf = leaf->next;
		}
	}
	else
	{
		while ( leaf )
		{
			mem_check_protection( leaf );
			(*callback)( leaf->file, leaf->line, leaf->size, leaf->tag, leaf->corrupted );
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
			mem_check_protection( leaf );

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
					mem_check_protection( leaf );

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
						mem_check_protection( leaf );

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
				mem_check_protection( leaf );

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
		
		mem_free( leaf );

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
#if defined(DEBUG_OUTPUT_WINDOW)
	mem_report_debug_to_window( 0 );
#endif
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
	}
#endif

	callstack_clear();
#elif SEGAN_MEMLEAK
#if defined(DEBUG_OUTPUT_WINDOW)
	mem_report_debug_to_window( 0 );
#endif
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
#endif


	lib_finit_cs();
}

