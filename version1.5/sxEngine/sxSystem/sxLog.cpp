#include "sxLog.h"
#include "sxFile.h"
#include <stdio.h>
#include <time.h>



const int LOG_MAX_SIZE = 1024 * 10;


//! class of logger. you can easily create a new one or point to exist one
class LoggerClass
{
public:
	LoggerClass(UINT LM_ mode, HWND window, const WCHAR* filename )
	{
		_filestream = sx_new( sx::sys::FileStream );
		SetMode(mode);
		SetWindow(window);
		SetFileName(filename);
		SetCallback(NULL);

		m_buffer = (WCHAR*)sx_mem_alloc( LOG_MAX_SIZE * sizeof(WCHAR) );
	};

	~LoggerClass(void){
		
		//  free allocated memory
		sx_mem_free(m_buffer); m_buffer = null;

		sx_delete_and_null(_filestream);
	}

	void Log(const WCHAR* logmessage){
		if ( !logmessage ) return;

		if (SEGAN_SET_HAS(_mode, LM_CONSOLE))
			wprintf(logmessage);	

		if (SEGAN_SET_HAS(_mode, LM_WINDOW) && _window)
			PostMessage(_window, WM_SX_LOGGER, (WPARAM)logmessage, 0);

		if (SEGAN_SET_HAS(_mode, LM_FILE))
			_filestream->Write(logmessage, (int)wcslen(logmessage)*sizeof(WCHAR));

		if (_callback)
			_callback(logmessage);

	}

	void SetMode(UINT LM_ mode){
		_cs.Enter();
		_mode = mode;
		_cs.Leave();
	}

	UINT GetMode(void) LM_ {
		return _mode;
	}

	void SetWindow(HWND window){
		_cs.Enter();
		_window = window;
		_cs.Leave();
	}

	HWND GetWindow(void){
		return _window;
	}

	void SetFileName(const WCHAR* FileName, bool Rewrite = true)
	{
		_cs.Enter();

		_filename = FileName;
		((sx::sys::FileStream*)_filestream)->Close();

		if (FileName)
		{
			if (!sx::sys::FileStream::FileExist(FileName) || Rewrite)
			{
				((sx::sys::FileStream*)_filestream)->Open(FileName, FM_CREATE);

				// make it Unicode text
				char c[2] = {-1, -2};
				_filestream->Write(c, 2);

				((sx::sys::FileStream*)_filestream)->Close();
			}

			((sx::sys::FileStream*)_filestream)->Open(FileName, FM_OPEN_WRITE | FM_SHARE_READ);
			_filestream->Seek(ST_END);
		}

		_cs.Leave();
	}

	const WCHAR* GetFileName(void) {
		return _filename.Text();
	}

	void SetCallback(CallBack_Logger callback){
		_cs.Enter();
		_callback = callback;
		_cs.Leave();
	}

public:
	UINT				_mode;
	HWND				_window;
	String				_filename;
	PStream				_filestream;

	CriticalSection	_cs;
	CallBack_Logger		_callback;

	WCHAR*				m_buffer;
};
static LoggerClass _logger(LM_CONSOLE, NULL, NULL);

void sxLog::Log( const WCHAR* format, ... )
{
	//sx_callstack_push(sxLog::Log(format));

	if ( !format ) return;
	
	_logger._cs.Enter();

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
	buffer[strLen++] = '\r';
	buffer[strLen++] = '\n';
	buffer[strLen] = 0;

	_logger.Log(buffer );

	mem_free( buffer );

	_logger._cs.Leave();

}

void sxLog::Log_( const WCHAR* format, ... )
{
	if ( !format ) return;

	_logger._cs.Enter();

	va_list argList;
	va_start(argList, format);
	int len = vswprintf_s( _logger.m_buffer, LOG_MAX_SIZE, format, argList );
	va_end(argList);

	_logger.Log( _logger.m_buffer );

	_logger._cs.Leave();
}

void sxLog::SetMode( UINT LM_ mode )
{
	_logger.SetMode(mode);
}

UINT sxLog::GetMode( void ) LM_
{
	return _logger.GetMode();
}

void sxLog::SetWindow( HWND window )
{
	_logger.SetWindow(window);
}

HWND sxLog::GetWindow( void )
{
	return _logger.GetWindow();
}

void sxLog::SetFileName( const WCHAR* FileName, bool Rewrite /*= true*/ )
{
	_logger.SetFileName(FileName, Rewrite);
}

const WCHAR* sxLog::GetFileName( void )
{
	return _logger.GetFileName();
}

void sxLog::SetCallback( CallBack_Logger callback )
{
	_logger.SetCallback(callback);
}

