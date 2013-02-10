/********************************************************************
	created:	2010/07/22
	filename: 	Log.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Logger class 
*********************************************************************/
#ifndef GUARD_Log_HEADER_FILE
#define GUARD_Log_HEADER_FILE

#include "System_def.h"


//!	logger modes
#define LM_
#define LM_CONSOLE	0x01	//	log on console window 
#define LM_FILE		0x02	//	log on file

typedef void (*Logger_Callback)(const wchar* message);


//! use this config to initialize the logger
struct LoggerConfig
{
	wchar*				name;			//	name of the logger
	wchar*				fileName;		//	name of the file
	uint				mode;			//	logger mode
	Logger_Callback		callback;		//	pointer to callback function

	LoggerConfig( void ): name(null), fileName(null), mode(0), callback(0){}
};

//! main logger class of engine
class SEGAN_ENG_API Logger
{
	SEGAN_STERILE_CLASS(Logger);

public:

	Logger( void );
	~Logger( void );

	//!  initialize logger
	void Initialize( LoggerConfig* config = null );

	//! log with time stamp
	void Log( const wchar* format, ... );

	//! log without time stamp
	void Log_( const wchar* format, ... );

public:

	uint				m_mode;
	String				m_name;
	String				m_fileName;
	FILE*				m_fileStream;
	Logger_Callback		m_callback;
};

extern Logger* g_logger;

#endif // GUARD_Log_HEADER_FILE
