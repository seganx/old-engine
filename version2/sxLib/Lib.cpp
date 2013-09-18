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


#if ( defined(_DEBUG) || SEGAN_CALLSTACK )


SEGAN_INLINE sint lib_assert( const wchar* expression, const wchar* file, const sint line )
{

#if ( SEGAN_CALLSTACK == 1 )
	_CallStack _callstack( line, file, L"assertion '%s'", expression );
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
	lib_enter_cs();

	if ( callstack_index < CALLSTACK_MAX )
	{

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
	}
	lib_leave_cs();
}

SEGAN_INLINE _CallStack::_CallStack( const sint line, const wchar* file, const wchar* function, ... )
{
	lib_enter_cs();

	if ( callstack_index < CALLSTACK_MAX )
	{
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
				sx_str_copy( csd->name, 1023, function );
			va_end( argList );
		}
		else
		{
			csd->name[0] = null;
		}
	}
	lib_leave_cs();
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
				if ( maxlength < tmp.length() )
					maxlength = tmp.length();
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

				sint len = tmp.length();
				for ( sint spaces = maxlength + 5; spaces > len; spaces-- )
					tmp << ' ';

				if ( csd->function )
					tmp << csd->function << '\n';
				else if ( csd->name[0] )
					tmp << csd->name << '\n';
				else
					tmp << L"no name !\n";

				fputws( tmp.text(), f );
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
		s_manager->realloc( p, newsizeinbyte );
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
	return s_manager ? s_manager->size( p ) : (uint)::_msize( (void*)p );	
}

SEGAN_INLINE void mem_free( const void* p )
{
	if ( s_manager )
	{
		s_manager->free( p );
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


SEGAN_INLINE void mem_set( void* dest, const sint val, const uint size )
{
	memset( dest, val, size );
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

			//	report call stack
			lib_assert( L"memory block has been corrupted !", memreport.mb->file, memreport.mb->line );
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

			//	report call stack
			lib_assert( L"memory block has been corrupted !", memreport.mb->file, memreport.mb->line );
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

				OutputDebugString( tmp.text() );
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

			OutputDebugString( tmp.text() );

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
						fputws( tmp.text(), f );
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

						fputws( tmp.text(), f );
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

				fputws( tmp.text(), f );

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
SEGAN_INLINE float sx_random_f( const float range )
{
	return  ( range ) / RAND_MAX;
}

SEGAN_INLINE sint sx_random_i( const sint range )
{
	return  ( range ) / RAND_MAX;
}


//////////////////////////////////////////////////////////////////////////
//	id generator
//////////////////////////////////////////////////////////////////////////
uint id_counter = 0;

SEGAN_INLINE void sx_id_set_counter( const uint id )
{
	id_counter = 0;
}

SEGAN_INLINE uint sx_id_generate( void )
{
	return id_counter++;
}

//////////////////////////////////////////////////////////////////////////
//	crc32 algorithm
//////////////////////////////////////////////////////////////////////////
unsigned long crc32_table[256];

unsigned long crc32_reflect( unsigned long ref, char ch )
{
	unsigned long value = 0;
	for ( sint i = 1; i < (ch + 1); ++i )
	{
		if ( ref & 1 )
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}

uint crc32_init_table( void )
{
	unsigned long ulPolynomial = 0x04c11db7;
	for ( sint i = 0; i <= 0xFF; ++i )
	{
		crc32_table[i]= crc32_reflect( i, 8 ) << 24;
		for ( sint j = 0; j < 8; ++j )
			crc32_table[i] = (crc32_table[i] << 1) ^ ( crc32_table[i] & (1 << 31) ? ulPolynomial : 0 );
		crc32_table[i] = crc32_reflect( crc32_table[i], 32 );
	}
	return 0;
}

SEGAN_INLINE uint sx_crc32_a( const char* str )
{
	sint len = sx_str_len( str );
	if ( len < 1 ) return 0;

	unsigned char* buffer = (unsigned char*)str;

	unsigned long ulCRC(0xffffffff);
	while( len-- )
		ulCRC = (ulCRC >> 8) ^ crc32_table[ (ulCRC & 0xFF) ^ *buffer++ ];

	return ulCRC ^ 0xffffffff;
}

SEGAN_INLINE uint sx_crc32_w( const wchar* str )
{
	sint len = sx_str_len( str );
	if ( len < 1 ) return 0;

	wchar* buffer = (wchar*)str;
	unsigned long ulCRC(0xffffffff);
	while( len-- )
	{
		union hchar {
			struct {
				char c1;
				char c2;
			};
			wchar c;
		} hc = *(hchar*)(buffer++);
		ulCRC = (ulCRC >> 8) ^ crc32_table[ (ulCRC & 0xFF) ^ hc.c1 ];
		if ( hc.c2 )
			ulCRC = (ulCRC >> 8) ^ crc32_table[ (ulCRC & 0xFF) ^ hc.c2 ];
		
	}

	return ulCRC ^ 0xffffffff;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//	string functions
//	for multi threaded support we can use a simple string pool
wchar* str_pop( void )
{
	typedef wchar StrItem[1024];
	static StrItem str_pool[512];
	static sint index = 0;
	lib_enter_cs();
	if ( index >= 512 ) index = 0;
	wchar* res = str_pool[index++];
	lib_leave_cs();
	return res;

}

SEGAN_INLINE uint sx_wchar_to_utf8( char* dest, const uint destsize, const uint ch )
{//	code from : http://www.opensource.apple.com/source/OpenLDAP/OpenLDAP-186/OpenLDAP/libraries/libldap/utf-8-conv.c
	uint len = 0;
	if ( !dest || !destsize )   /* just determine the required UTF-8 char length. */
	{
		if ( ch < 0 )			return 0;
		if ( ch < 0x80 )		return 1;
		if ( ch < 0x800 )		return 2; 
		if ( ch < 0x10000 )		return 3;
		if ( ch < 0x200000 )	return 4;
		if ( ch < 0x4000000 )	return 5;
	}
	else if ( ch < 0 )
	{
		len = 0;
	}
	else if ( ch < 0x80 )
	{
		if ( destsize >= 1 )
		{
			dest[len++] = (char)ch;
		}

	}
	else if ( ch < 0x800 )
	{
		if ( destsize >= 2 )
		{
			dest[len++] = 0xc0 | ( ch >> 6 );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}

	}
	else if ( ch < 0x10000 )
	{
		if ( destsize >= 3 )
		{	
			dest[len++] = 0xe0 | ( ch >> 12 );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}
	
	}
	else if ( ch < 0x200000 )
	{
		if (destsize >= 4)
		{
			dest[len++] = 0xf0 | ( ch >> 18 );
			dest[len++] = 0x80 | ( (ch >> 12) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}

	}
	else if ( ch < 0x4000000 )
	{
		if ( destsize >= 5 )
		{
			dest[len++] = 0xf8 | ( ch >> 24 );
			dest[len++] = 0x80 | ( (ch >> 18) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 12) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}
	}
	else
	{
		if ( destsize >= 6 )
		{
			dest[len++] = 0xfc | ( ch >> 30 );
			dest[len++] = 0x80 | ( (ch >> 24) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 18) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 12) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}
	}
	return len;
}


SEGAN_INLINE uint sx_str_to_utf8( char* dest, const uint destsize, const wchar* src )
{
	int r = 0;
	char tmp[32];
	char* d = dest;
	while ( *src )
	{
		r = sx_wchar_to_utf8( tmp, 32, *src++ );
		if ( r > 0 )
		{
			memcpy( d, tmp, r );
			d += r;
		}
		else
		{
			*d++ = (char)*src++;
		}
	}
	*d = 0;
	return (uint)( d - dest );
}


//////////////////////////////////////////////////////////////////////////
//	code from :
//	http://www.opensource.apple.com/source/OpenLDAP/OpenLDAP-186/OpenLDAP/libraries/libldap/utf-8.c

const char ldap_utf8_lentab[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0 };

const char ldap_utf8_mintab[] = {
	(char)0x20, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
	(char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
	(char)0x30, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
	(char)0x38, (char)0x80, (char)0x80, (char)0x80, (char)0x3c, (char)0x80, (char)0x00, (char)0x00 };

	/* LDAP_MAX_UTF8_LEN is 3 or 6 depending on size of wchar_t */
#define LDAP_MAX_UTF8_LEN		 ( sizeof(wchar) * 3/2 )
#define LDAP_UTF8_ISASCII(p)	 ( !(*(const unsigned char *)(p) & 0x80 ) )
#define LDAP_UTF8_CHARLEN(p)	 ( LDAP_UTF8_ISASCII(p) ? 1 : ldap_utf8_lentab[*(const unsigned char *)(p) ^ 0x80] )
#define LDAP_UTF8_CHARLEN2(p, l) ( ( ( l = LDAP_UTF8_CHARLEN( p )) < 3 || ( ldap_utf8_mintab[*(const unsigned char *)(p) & 0x1f] & (p)[1] ) ) ? l : 0 )

SEGAN_INLINE uint sx_utf8_to_wchar( wchar dest, const uint destwords, const char* src )
{
	if ( !src ) return 0;

	/* Get UTF-8 sequence length from 1st byte */
	sint utflen = LDAP_UTF8_CHARLEN2(src, utflen);

	if ( utflen==0 || utflen > (int)LDAP_MAX_UTF8_LEN ) return 0;

	/* First byte minus length tag */
	unsigned char mask[] = { 0, 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
	wchar ch = (wchar)( src[0] & mask[utflen] );

	for( sint i = 1; i < utflen; ++i )
	{
		/* Subsequent bytes must start with 10 */
		if ( ( src[i] & 0xc0 ) != 0x80 ) return 0;

		ch <<= 6;			/* 6 bits of data in each subsequent byte */
		ch |= (wchar)( src[i] & 0x3f );
	}

	dest = ch;
	return utflen;
}

SEGAN_INLINE uint sx_utf8_to_str( wchar* dest, const uint destwords, const char* src )
{
	/* If input ptr is NULL or empty... */
	if ( !src || !*src )
	{
		if ( dest ) *dest = 0;
		return 0;
	}

	/* Examine next UTF-8 character.  If output buffer is NULL, ignore count */
	uint wclen = 0;
	while ( *src && ( !dest || wclen < destwords ) )
	{
		/* Get UTF-8 sequence length from 1st byte */
		sint utflen = LDAP_UTF8_CHARLEN2(src, utflen);

		if( !utflen || utflen > (sint)LDAP_MAX_UTF8_LEN ) return 0;

		/* First byte minus length tag */
		unsigned char mask[] = { 0, 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
		wchar ch = (wchar)( src[0] & mask[utflen] );

		for( sint i = 1; i < utflen; ++i )
		{
			/* Subsequent bytes must start with 10 */
			if ( (src[i] & 0xc0) != 0x80 ) return 0;

			ch <<= 6;			/* 6 bits of data in each subsequent byte */
			ch |= (wchar)( src[i] & 0x3f );
		}

		if ( dest ) dest[wclen] = ch;

		src += utflen;		/* Move to next UTF-8 character */
		wclen++;			/* Count number of wide chars stored/required */
	}

	/* Add null terminator if there's room in the buffer. */
	if ( dest && wclen < destwords ) dest[wclen] = 0;

	return wclen;
}


//////////////////////////////////////////////////////////////////////////


SEGAN_INLINE const wchar* sx_utf8_to_str( const char* src )
{
	wchar* tmp = str_pop();
	sx_utf8_to_str( tmp, 1024, src );
	return tmp;
}

SEGAN_LIB_API const char* sx_str_to_utf8( const wchar* src )
{
	char* tmp = (char*)str_pop();
	sx_str_to_utf8( tmp, 2048, src );
	return tmp;
}

SEGAN_INLINE const wchar* sx_int_to_str( const int number )
{
	wchar* tmp = str_pop();
	_itow_s( number, tmp, 64, 10 );
	return tmp;
}

SEGAN_INLINE const wchar* sx_uint_to_str( const uint number )
{
	wchar* tmp = str_pop();
	_ultow_s( number, tmp, 64, 10 );
	return tmp;
}

SEGAN_INLINE const wchar* sx_uint64_to_str( const uint64 number )
{
	wchar* tmp = str_pop();
	_ui64tow_s( number, tmp, 64, 10 );
	return tmp;
}

SEGAN_INLINE const wchar* sx_float_to_str( float number, int precision )
{
	sx_assert( precision > 0 && precision < 7 );
	wchar* res = str_pop();
	if ( sx_abs_f( number ) < 0.000001f )
	{
		sx_str_copy( res, precision + 3, L"0.000000" );
	}
	else
	{
		char tmp[64];
		int decimal, sign;
		_fcvt_s( tmp, 64, number, precision, &decimal, &sign );
		if ( tmp[0] )
		{
			int k = 0;
			if ( sign ) res[k++] = '-';
			if ( decimal > 0 )
			{
				int d = 0;
				for ( ; d < decimal; )
					res[k++] = tmp[d++];
				res[k++] = '.';
				for ( ; tmp[d]; )
					res[k++] = tmp[d++];
			}
			else
			{
				res[k++] = '0';
				res[k++] = '.';
				for (; decimal<0; decimal++ )
					res[k++] = '0';
				for (; tmp[decimal]; decimal++ )
					res[k++] = tmp[decimal];
			}
			res[k++] = 0;
		}
		else sx_str_copy( res, precision + 3, L"0.000000" );
	}
	return res;
}

SEGAN_INLINE const wchar* sx_str_extract_filepath( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '\\' || filename[i] == '/' )
			p = i + 1;
	}
	sint i = 0;
	for ( ; i < p; i++ )
		res[i] = filename[i];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_extract_filename( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '\\' || filename[i] == '/' )
			p = i + 1;
	}
	sint i = 0;
	for ( ; filename[p]; ++i, ++p )
		res[i] = filename[p];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_extract_extension( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '.' )
			p = i + 1;
	}
	sint i = 0;
	for ( ; filename[p]; ++i, ++p )
		res[i] = filename[p];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_exclude_extension( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '.' )
			p = i;
	}
	sint i = 0;
	for ( ; i < p; ++i )
		res[i] = filename[i];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_make_pathstyle( const wchar* filepath )
{
	if ( !filepath ) return null;
	sint len = sx_str_len( filepath );
	if ( len )
	{
		wchar ch = filepath[--len];
		if ( ch != '/' && ch != '\\' )
		{
			++len;
			wchar* res = str_pop();
			for ( int i=0; i<len; ++i )
				res[i] = filepath[i];
			res[len++] = PATH_PART;
			res[len] = 0;
			return res;
		}
	}
	return filepath;
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


	//	initialize crc32 table
	crc32_init_table();
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

