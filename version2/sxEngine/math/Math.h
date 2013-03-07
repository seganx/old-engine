/********************************************************************
	created:	2012/05/01
	filename: 	math.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain all math functions and classes
*********************************************************************/
#ifndef GUARD_math_HEADER_FILE
#define GUARD_math_HEADER_FILE

#include "Math_vec.h"
#include "Math_utils.h"
#include "Math_tools.h"

//! initialize math system to choose instruction sets. pass true to force using generic math library
SEGAN_ENG_API void sx_math_initialize( bool useGenericMath = false );

//! finalize math system
SEGAN_ENG_API void sx_math_finalize( void );


#endif	//	GUARD_math_HEADER_FILE