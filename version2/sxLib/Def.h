/********************************************************************
	created:	2012/03/21
	filename: 	Def.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.seganx.com
	Desc:		Perprocess defines will be here
*********************************************************************/
#ifndef HEADER_DEFINED_def
#define HEADER_DEFINED_def

//////////////////////////////////////////////////////////////////////////
//  basic type definitions
//////////////////////////////////////////////////////////////////////////

typedef unsigned char		    u8,			byte;
typedef unsigned short		    u16,		word;
typedef unsigned int		    u32,		uint,	dword;
typedef int					    i32,		sint;
typedef long				    i64,		int64;
typedef unsigned long		    u64,		uint64;
typedef long long			    i128,       int128;
typedef unsigned long long	    u128,       uint128;
typedef void*                   handle;
typedef int					    hresult;

#if __cplusplus
#else
#ifdef bool
#undef bool
#endif
#ifdef true
#undef true
#endif
#ifdef false
#undef false
#endif
typedef unsigned char bool;
#define true 1
#define false 0
#endif


//////////////////////////////////////////////////////////////////////////
//  functions type and classes type preprocessors
//////////////////////////////////////////////////////////////////////////
#define NOMINMAX

#ifdef IN
#undef IN
#endif
#define IN

#ifdef OUT
#undef OUT
#endif
#define OUT

#ifdef IN_OUT
#undef IN_OUT
#endif
#define IN_OUT

#ifdef null
#undef null
#endif
#define null	NULL

//////////////////////////////////////////////////////////////////////////
//!!!    CHANGE THESE PREPROCESSORS TO CHANGE COMPILER BEHAVIOR      !!!//
//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
#if defined( SEGAN_IMPORT )
	#define SEGAN_LIB_API					__declspec(dllimport)
#else
	#define SEGAN_LIB_API					__declspec(dllexport)
#endif
#else
	#define SEGAN_LIB_API
#endif

#if defined(_WIN32)
#define SEGAN_LIB_INLINE					__forceinline
#else
#define SEGAN_LIB_INLINE					inline
#endif

#define SEGAN_INLINE						inline

#if defined(_WIN32)
	#define SEGAN_ALIGN_16					__declspec(align(16))
#else
	#define SEGAN_ALIGN_16
#endif

#define SEGAN_MEMLEAK						1		//	use first version of memory leak detector

#define SEGAN_ASSERT						0		//	check and log some special events on containers

#define SEGAN_CRASHRPT						0		//	allow the crash reporter system to install it's handlers

#define SEGAN_CRASHRPT_CALLSTACK			0		//	enable call stack system to log stack of function

#define SEGAN_LIB_MULTI_THREADED			0		//	enable core library multi-threaded safe 

#define SX_LIB_SINGLETON                    0

#define SEGANX_TRACE_CRASHRPT               0

#define SEGANX_TRACE_CALLSTACK              1

#define SEGANX_TRACE_PROFILER               1


//////////////////////////////////////////////////////////////////////////
//!!!  DO NOT CHANGE THIS AREA ANY MORE	 !!!//
//////////////////////////////////////////////////////////////////////////

// release preprocessors
#define sx_release(Obj)						{ if (Obj) { Obj->Release(); } }
#define sx_release_and_null(Obj)			{ if (Obj) { Obj->Release(); Obj = null; } }

// some useful macros for cardinal handling
#define sx_set_has(set, subset)				( set & subset )
#define sx_set_hasnt(set, subset)			( !( set & subset ) )
#define sx_set_add(set, subset)				( set |= subset )
#define sx_set_rem(set, subset)				( set &= ~subset )

// some useful functions for byte operations
#define sx_byte_of(var, index)				( ( (byte*)(&var) )[index] )
#define sx_1th_byte_of(var)					( ( (byte*)(&var) )[0] )
#define sx_2th_byte_of(var)					( ( (byte*)(&var) )[1] )
#define sx_3th_byte_of(var)					( ( (byte*)(&var) )[2] )
#define sx_4th_byte_of(var)					( ( (byte*)(&var) )[3] )

#define sx_1th_word_of(var)					( ( (word*)(&var) )[0] )
#define sx_2th_word_of(var)					( ( (word*)(&var) )[1] )

#define sx_fourcc(ch0, ch1, ch2, ch3)		( (dword)(byte)(ch0) | ((dword)(byte)(ch1) << 8) | ((dword)(byte)(ch2) << 16) | ((dword)(byte)(ch3) << 24 ) )

//!	takes a structure name s, and a field name f in s
#define sx_offset_of(s, f)		        	( (dword)&( ((s *)0)->f ) )

//! return number of items in a static array
#define sx_array_count(x)					( sizeof(x) / sizeof(x[0]) )

//! avoid class from copy constructor and assign operator
#define sx_sterile_class(classname)		    private: classname(classname& obj); void operator= (classname& obj)

//	some crazy macro to define unique names
#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res
#define sx_unique_name(base) PP_CAT(base, __COUNTER__)

//	use debug output window in IDE
#if defined(_DEBUG)
#if defined(_MSC_VER)
#if ( _MSC_VER >= 1400 )
#define DEBUG_OUTPUT_WINDOW
#endif
#endif
#endif

#if defined(_WIN32)
//! disable container warnings
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

//! includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#endif	//	HEADER_DEFINED_def
