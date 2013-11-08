// containers.cpp : Defines the entry point for the console application.
//

#include "ImportLib.h"
#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment( lib, "winmm.lib" )


void lib_callback( const wchar* message )
{
	printf( "%S\n", message );
	//int i = (*((int*)0));
}

void callstack_callback( const wchar* file, const sint line, const wchar* function )
{
	printf( "called : %S - %d - %S\n", file, line, function  );
}


int Hi( int a )
{
	sx_callstack_param(Hi(%d), a );
	return a << 1;
}

int power( int n, int m )
{
	sx_callstack_param(power(%d, %d), n, m);
	return Hi(n);
}


wchar* m_buffer = null;

void Logger_Log( const wchar* format, ... )
{
	wchar tmp1[400000];
	tmp1[0] = 0;

	sx_mem_realloc( m_buffer, 1024 );

	va_list argList;
	va_start(argList, format);
	int len = _vscwprintf( format, argList );
	vswprintf_s( m_buffer, 512, format, argList );
	va_end(argList);

	sx_mem_free( m_buffer );

}


bool table_callback( void* userdata, const wchar* name, int& data )
{
	wprintf( L"%s\n", name );
// 	if ( sx_str_cmp( name, "s" ) == 0 )
// 		return false;
// 	else
		return true;
}

int main(int argc, char* argv[])
{
	SetThreadAffinityMask( GetCurrentThread(), 1 );

	//Logger_Log( L" this is a simple test" );

#if 0
	{
		Randomer rnd(2);
		for ( uint i=0; i<1000; ++i )
		{
			printf( "%d ", rnd.get_i( 1000 ) );
		}
		printf( "\n" );
	}
#endif

#if 0
	{
		String str = "hello";
		bool e = str == L"hello1";
		const wchar* tst = sx_str_make_pathstyle( L"D:/SeganX/version.2/sxLib.txt" );
		str = tst;
		wprintf( L"%s\n", str );

		char* s1 = "4567";
		sx_callstack();
		char* s2 = "1234";
		sx_callstack();
		printf( "strcmp( %s, %s ) = %d\n", s1, s2, sx_str_cmp( s1, s2 ) );

		String test = L"salam";
		test.format( L"this is %S \n", s1 );
		wprintf( test );
	}
#endif

#if 0
	{
		Array<int> array_i;

		array_i.push_back(3);
		sx_callstack_param(array_i.PushBack(3));
		int i= array_i[0];

		printf( "%d\n", sx_sqr_i(-2) );
		printf( "%d\n", sx_cube_i(-2) );
		printf( "%d\n", sx_abs_i(-2) );

		for ( int i = 0; i < 7; i++ )
			printf( "%d\n", power(i, i*i) );
		printf( "\n" );

		for ( int i=0; i<10; i++ )
			printf( "%.2f\n", sx_random_f( 20.0f ) );
		printf( "\n" );

		for ( int i=0; i<10; i++ )
			printf( "%d\n", sx_random_i( 20 ) );
		printf( "\n" );
	}
#endif

#if 0
	{

		char buffer[32];
		for ( int i=0; i<32; i++ )
			buffer[i] = i+65;

		MemoryStream stream;
		sx_stream_write( buffer );
		stream.set_pos(0);

		char newbuffer[64] = {0};
		sx_stream_read( newbuffer );
		newbuffer[63] = 0;

		stream.set_pos(0);
		MemoryStream mem;
		mem.copy_from( stream, 67 );
	}
#endif

#if 0
	{

		MemMan_Pool pool( 60 );
		int* b = (int*)pool.m_pool;

		printf( "\n" );
		for ( int i=0; i<6; i++ )
			printf( "%.8x %.8x %.8x %.8x %.8x \n", *b++, *b++, *b++, *b++, *b++ );

		int* a = (int*)pool.alloc( 20 );
		a[0] = 0xaaaaaaaa;
		a[1] = 0xaaaaaaaa;
		a[2] = 0xaaaaaaaa;
		a[3] = 0xaaaaaaaa;
		a[4] = 0xaaaaaaaa;

		printf( "\n" );
		b = (int*)pool.m_pool;
		for ( int i=0; i<6; i++ )
			printf( "%.8x %.8x %.8x %.8x %.8x \n", *b++, *b++, *b++, *b++, *b++ );

		int* c = (int*)pool.alloc( 20 );
		c[0] = 0xcccccccc;
		c[1] = 0xcccccccc;
		c[2] = 0xcccccccc;
		c[3] = 0xcccccccc;
		c[4] = 0xcccccccc;

		printf( "\n" );
		b = (int*)pool.m_pool;
		for ( int i=0; i<6; i++ )
			printf( "%.8x %.8x %.8x %.8x %.8x \n", *b++, *b++, *b++, *b++, *b++ );

		int* e = (int*)pool.alloc( 20 );
		e[0] = 0xeeeeeeee;
		e[1] = 0xeeeeeeee;
		e[2] = 0xeeeeeeee;
		e[3] = 0xeeeeeeee;
		e[4] = 0xeeeeeeee;

		printf( "\n" );
		b = (int*)pool.m_pool;
		for ( int i=0; i<6; i++ )
			printf( "%.8x %.8x %.8x %.8x %.8x \n", *b++, *b++, *b++, *b++, *b++ );

		pool.free(a);

		printf( "\n" );
		b = (int*)pool.m_pool;
		for ( int i=0; i<6; i++ )
			printf( "%.8x %.8x %.8x %.8x %.8x \n", *b++, *b++, *b++, *b++, *b++ );

		pool.free(c);

		printf( "\n" );
		b = (int*)pool.m_pool;
		for ( int i=0; i<6; i++ )
			printf( "%.8x %.8x %.8x %.8x %.8x \n", *b++, *b++, *b++, *b++, *b++ );
	}
#endif

#if 0
	{
		int* a = (int*)sx_mem_alloc( 16 );
		a[6] = 4;
	}
#endif

#if 0
	{
		sx_callstack();

		for ( uint i=0; i < 10; ++i )
		{
			float r = sx_hermite_lerp( 30, 50, 10, 20, i * 0.1f, 0, 0 );
			printf( "%.1f ", r );
		}

		sx_callstack_param(salam%d, 192);
	}
#endif

#if 0
	{
		Logger log;
		log.initialize( true, L"D:/test.txt", null );
		log.log( L"Hi this is a simple test %d", 10 );
		log.log( L"Hi this is a simple test %d", 20 );
		log.log( L"Hi this is a simple test %d", 30 );
		log.log_( L"Hi this is a simple test %d", 40 );
		log.log_( L"Hi this is a simple test %d", 50 );
	}
#endif


#if 1
	{
		Table<int> table;

		FILE* f = 0;
		if ( fopen_s( &f, "D:/test.txt", "r, ccs=UNICODE" ) == 0 )
		{
			int cnt = 0;
			wchar tmp[1024] = {0};
			wchar c = 0; int index = 0;
			while ( fread_s( &c, 2, 2, 1, f ) )
			{
				if ( c == '\n' )
				{
					tmp[index] = 0;
					if ( index )
						table.insert_sort( tmp, ++cnt );
					index = 0;
					tmp[0] = 0;
				}
				else if ( c && c!='\r' )
				{
					tmp[index++] = c;
				}
			}
			fclose(f);
		}

		table.iterate( null, &table_callback );
		printf( "\ncount = %d\n\n", table.m_count );

#if _DEBUG
		table.print( table.m_root, 0 );
#endif
		table.clear();
	}
#endif

	getchar();
	sx_detect_crash();

	return 0;
}

