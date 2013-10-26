#include <share.h>
#include <time.h>
#include <memory>
#include <cwchar>

#if defined(_WIN32)
#include <Windows.h>
#endif

#include "Logger.h"


Logger::Logger( void )
:	m_console(0)
,	m_callback(0)
{
	memset( m_filename, 0, sizeof(m_filename) );
}

void Logger::initialize( const bool console, const wchar* filename, CB_Logger callback )
{
	m_console = console;
	wcscpy_s( m_filename, 512, filename );
	m_callback = callback;

	if ( filename )
	{
		//	append message to the end of the file
		FILE* f = null;
		if( _wfopen_s( &f, m_filename, L"w, ccs=UNICODE" ) == 0 )
		{
			fputws( L"SeganX\n\n", f );
			fclose( f );
		}
	}
}

void Logger::log( const wchar* format, ... )
{
	if ( !format ) return;
	sx_enter_cs();

	va_list argList;
	va_start(argList, format);

	int strLen = _vscwprintf( format, argList ) + 1;
	const int additionaLen = 32 * sizeof(wchar);	//	additional char length of time stamp and char return
	wchar* buffer = (wchar*)malloc( additionaLen + strLen * sizeof(wchar) );

	//	apply time stamp
	int timeOffset = 0;
	{
		time_t rawTime;
		time( &rawTime );
		struct tm timeInfo;
		localtime_s( &timeInfo, &rawTime );
		timeOffset = (int)wcsftime( buffer, 40, L"%Y/%m/%d %H:%M:%S > ", &timeInfo );
	}

	strLen = vswprintf_s( buffer+timeOffset, strLen, format, argList ) + timeOffset;
	va_end(argList);

	// fill end of line
	buffer[strLen++] = '\n';
	buffer[strLen] = 0;

	if ( m_console )
	{
		wprintf( buffer );
#if defined(DEBUG_OUTPUT_WINDOW)
		OutputDebugString( buffer );
#endif
	}

	if ( m_callback )
		m_callback( buffer );

	if ( m_filename[0] )
	{
		//	append message to the end of the file
		FILE* f = null;
		if( _wfopen_s( &f, m_filename, L"a+, ccs=UNICODE" ) == 0 )
		{
			fputws( buffer, f );
			fclose( f );
		}
	}

	free( buffer );

	sx_leave_cs();
}

void Logger::log_( const wchar* format, ... )
{
	if ( !format ) return;
	sx_enter_cs();

	va_list argList;
	va_start(argList, format);
	int strLen = _vscwprintf( format, argList ) + 5;
	wchar* buffer = (wchar*)malloc( strLen * sizeof(wchar) );
	strLen = vswprintf_s( buffer, strLen, format, argList );
	va_end(argList);

	if ( m_console )
	{
		wprintf( buffer );
#if defined(DEBUG_OUTPUT_WINDOW)
		OutputDebugString( buffer );
#endif
	}

	if ( m_callback )
		m_callback( buffer );

	if ( m_filename[0] )
	{
		//	append message to the end of the file
		FILE* f = null;
		if( _wfopen_s( &f, m_filename, L"a+, ccs=UNICODE" ) == 0 )
		{
			fputws( buffer, f );
			fclose( f );
		}
	}

	free( buffer );

	sx_leave_cs();
}