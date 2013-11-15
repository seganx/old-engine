/********************************************************************
	created:	2012/03/21
	filename: 	Lib.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain  all header files of SeganX library
*********************************************************************/
#ifndef GUARD_Lib_HEADER_FILE
#define GUARD_Lib_HEADER_FILE

#include "Def.h"
#include "Math.h"
#include "Assert.h"
#include "Memory.h"
#include "Callstack.h"
#include "Array.h"
#include "Queue.h"
#include "Stack.h"
#include "Map.h"
#include "String.h"
#include "Stream.h"
#include "Logger.h"
#include "Table.h"
#include "Utils.h"


// initialize internal library
void sx_lib_initialize( void );

// finalize internal library
void sx_lib_finalize( void );


#endif	//	GUARD_Lib_HEADER_FILE
