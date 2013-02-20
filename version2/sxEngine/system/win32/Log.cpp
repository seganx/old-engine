#if defined(_WIN32)

#include "../Log.h"
#include "Win6.h"
#include <stdio.h>
#include <share.h>
#include <time.h>

extern Logger * g_logger = 0;

static CRITICAL_SECTION s_criticalSection;

Logger::Logger( void ): m_mode(LM_CONSOLE), m_fileStream(0), m_callback(0)
{
	::InitializeCriticalSection( &s_criticalSection );
}

Logger::~Logger( void )
{
	::DeleteCriticalSection( &s_criticalSection );
}

void Logger::Initialize( LoggerConfig* config /*= null */ )
{
	if ( config )
	{
		m_name = config->name;
		m_fileName = config->fileName;
		m_mode = config->mode;
		m_callback = config->callback;
	}
	else
	{
		m_name = L"SeganX Logger";
		m_fileName = L"sx_engine_log.txt";
		m_mode = LM_FILE | LM_CONSOLE;
	}


	if ( (m_mode & LM_FILE) && m_fileName.Text() )
	{
		m_fileStream = _wfsopen( m_fileName.Text(), L"w+b", _SH_DENYWR );
		if ( m_fileStream )
		{
			// make it Unicode text
			char c[2] = {-1, -2};
			fwrite( c, 1, 2, m_fileStream );
		}
		else
		{
			SEGAN_SET_REM( m_mode, LM_FILE );
		}
	}
}

void Logger::Log( const wchar* format, ... )
{
	if ( !format ) return;
	::EnterCriticalSection( &s_criticalSection );

	va_list argList;
	va_start(argList, format);

	int strLen = _vscwprintf( format, argList ) + 1;
	const int additionaLen = 32 * sizeof(wchar);	//	additional char length of time stamp and char return
	wchar* buffer = (wchar*)mem_alloc( additionaLen + strLen * sizeof(wchar) );

	//	apply time stamp
	int timeOffset = 0;
	{
		time_t rawTime;
		time( &rawTime );
		struct tm timeInfo;
		localtime_s( &timeInfo, &rawTime );
		timeOffset = (int)wcsftime( buffer, 40, L"%y/%m/%d %H:%M:%S > ", &timeInfo );
	}

	strLen = vswprintf_s( buffer+timeOffset, strLen, format, argList ) + timeOffset;
	va_end(argList);

	// fill end of line
	buffer[strLen+1] = '\n';
	buffer[strLen+2] = 0;

	if ( m_mode & LM_CONSOLE )
		wprintf( buffer );

	if ( m_callback )
		m_callback( buffer );

	if ( m_mode & LM_FILE )
	{
		buffer[strLen++] = '\r';
		buffer[strLen++] = '\n';
		buffer[strLen] = 0;

		fwrite( buffer, sizeof(wchar), strLen, m_fileStream );
		fflush( m_fileStream );
	}

	mem_free( buffer );

	::LeaveCriticalSection( &s_criticalSection );
}

void Logger::Log_( const wchar* format, ... )
{
	if ( !format ) return;
	::EnterCriticalSection( &s_criticalSection );

	va_list argList;
	va_start(argList, format);
	int strLen = _vscwprintf( format, argList ) + 5;
	wchar* buffer = (wchar*)mem_alloc( strLen * sizeof(wchar) );
	strLen = vswprintf_s( buffer, strLen, format, argList );
	va_end(argList);

	// fill end of line
	buffer[strLen+1] = '\n';
	buffer[strLen+2] = 0;

	if ( m_mode & LM_CONSOLE )
		wprintf( buffer );

	if ( m_callback )
		m_callback( buffer );

	if ( m_mode & LM_FILE )
	{
		// fill end of line
		buffer[strLen++] = '\r';
		buffer[strLen++] = '\n';
		buffer[strLen] = 0;

		fwrite( buffer, sizeof(wchar), strLen, m_fileStream );
		fflush( m_fileStream );
	}

	mem_free( buffer );

	::LeaveCriticalSection( &s_criticalSection );
}


#endif
