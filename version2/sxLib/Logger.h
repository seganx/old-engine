/********************************************************************
	created:	2013/10/24
	filename: 	Logger.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class and function to log
				anythings
*********************************************************************/
#ifndef GUARD_Logger_HEADER_FILE
#define GUARD_Logger_HEADER_FILE

#include "Def.h"


//! callback function will called by logger
typedef uint (*CB_Logger)( const wchar* str );

//! simple logger to log events
class SEGAN_LIB_API Logger
{
public:
	Logger( void );

	//! initialize the logger
	void initialize( const bool console, const wchar* filename, CB_Logger callback );

	//!	log the message with a time stamp as prefix and go to the next line
	void log( const wchar* format, ... );

	//! log the message by simply copy to the logger
	void log_( const wchar* format, ... );

private:
	bool		m_console;
	wchar		m_filename[512];
	CB_Logger	m_callback;
};

#endif	//	GUARD_Logger_HEADER_FILE