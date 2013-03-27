/********************************************************************
	created:	2012/03/21
	filename: 	Def.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		Perprocess defines will be here
*********************************************************************/
#ifndef GUARD_Def_HEADER_FILE
#define GUARD_Def_HEADER_FILE

//////////////////////////////////////////////////////////////////////////
//  basic type definitions
//////////////////////////////////////////////////////////////////////////

typedef unsigned __int8		u8,			byte;
typedef unsigned __int16	u16,		word;
typedef __int32				i32,		sint;
typedef unsigned __int32	u32,		uint, 		dword;
typedef __int64				i64,		int64;
typedef unsigned __int64	u64,		uint64;
typedef void				*pvoid, 	*handle;
typedef wchar_t				wchar;
typedef int					hresult;
typedef byte				*pbyte;



//////////////////////////////////////////////////////////////////////////
//  functions type and classes type preprocessors
//////////////////////////////////////////////////////////////////////////

// SeganX is unicode
#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

#ifndef IN_OUT
	#define IN_OUT
#endif

#define NOMINMAX

#if defined(null)
	#undef null
#endif
#define null	0

#if	defined(true)
	#undef true
#endif
#define true	1

#if defined(false)
	#undef false
#endif
#define false	0

//////////////////////////////////////////////////////////////////////////
//!!!    CHANGE THESE PREPROCESSORS TO CHANGE COMPILER BEHAVIOR      !!!//
//////////////////////////////////////////////////////////////////////////
#if defined( SEGAN_IMPORT )
	#define SEGAN_LIB_API		__declspec(dllimport)
#else
	#define SEGAN_LIB_API		__declspec(dllexport)
#endif

#if defined( SEGAN_IMPORT )
	#define SEGAN_ENG_API		__declspec(dllimport)
#else
	#define SEGAN_ENG_API		__declspec(dllexport)
#endif


#define SEGAN_LIB_INLINE		__forceinline

#define SEGAN_INLINE			inline

#define SEGAN_ALIGN_16			__declspec(align(16))

#define SEGAN_MEMLEAK			1		// use first version of memory leak detector

#define SEGAN_CALLSTACK			1		// enable call stack system to log stack of function

#define SEGAN_LIB_ASSERT		1		// check and log some special events on containers

#define SEGAN_MATH_SIMD			1		// use SIMD instruction in some math functions


//////////////////////////////////////////////////////////////////////////
//!!!  DO NOT CHANGE THIS AREA ANY MORE	 !!!//
//////////////////////////////////////////////////////////////////////////

//! avoid class from copy constructor and assign operator
#define SEGAN_STERILE_CLASS(classname)		private: classname(classname& obj); void operator= (classname& obj);


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

#define sx_fourcc(ch0, ch1, ch2, ch3)		( (dword)(byte)(ch0) | ((dword)(byte)(ch1) << 8) | ((dword)(byte)(ch2) << 16) | ((dword)(byte)(ch3) << 24 ) )


//! disable container warnings
#pragma warning(disable:4251)
#pragma warning(disable:4275)


#endif	//	GUARD_Def_HEADER_FILE
