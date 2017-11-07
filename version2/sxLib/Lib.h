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
#include "Md5.h"
#include "Json.h"
#include "Base64.h"
#include "SHA256.h"
#include "String.h"

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
