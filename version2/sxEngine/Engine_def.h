/********************************************************************
	created:	2013/01/22
	filename: 	Engine_def.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some macro definition for engine
*********************************************************************/
#ifndef GUARD_Engine_def_HEADER_FILE
#define GUARD_Engine_def_HEADER_FILE


#include "../../sxLib/Lib.h"


#if defined( SEGAN_IMPORT )
#define SEGAN_ENG_API			__declspec(dllimport)
#else
#define SEGAN_ENG_API			__declspec(dllexport)
#endif


#define SEGAN_MATH_SIMD			0		// use SIMD instruction in some math functions


#endif	//	GUARD_Engine_def_HEADER_FILE
