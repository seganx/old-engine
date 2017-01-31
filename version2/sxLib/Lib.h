/********************************************************************
	created:	2012/03/21
	filename: 	Lib.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain  all header files of SeganX library
*********************************************************************/
#ifndef HEADER_DEFINED_Lib
#define HEADER_DEFINED_Lib

#include "Def.h"
#include "Platform.h"
#include "Math.h"
#include "Assert.h"
#include "Memory.h"
#include "Crash.h"
#include "Array.h"
#include "Queue.h"
#include "Stack.h"
#include "Map.h"
#include "String.h"
#include "Stream.h"
#include "Logger.h"
#include "Dictionary.h"
#include "Partition.h"
#include "Protocol.h"
#include "Timer.h"
#include "Thread.h"
#include "Md5.h"
#include "Utils.h"
#include "Json.h"
#include "Table.h"
#include "Bigint.h"
#include "Base64.h"

//////////////////////////////////////////////////////////////////////////
//	global singleton class
//////////////////////////////////////////////////////////////////////////
#if SX_LIB_SINGLETON
class SEGAN_LIB_API Library
{
private:
    Library(void);
    ~Library( void );
	sx_sterile_class(Library);

public:
	static Library* GetSingelton(void);

public:
	Randomer*	m_randomer;
	Timer*		m_timer;
	Logger*		m_logger;
};
extern Library* g_lib;
#endif

// initialize internal library
void sx_lib_initialize( void );

// finalize internal library
void sx_lib_finalize( void );


#endif	//	HEADER_DEFINED_Lib
