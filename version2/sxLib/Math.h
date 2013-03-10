/********************************************************************
	created:	2012/04/07
	filename: 	Math.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some basic math functions

	NOTE: some alghoritm used here came from Doom 3 GPL Source Code. Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 
*********************************************************************/
#ifndef GUARD_Math_HEADER_FILE
#define GUARD_Math_HEADER_FILE

#include "Def.h"
#include <math.h>


#ifdef PI
#undef PI
#endif

#ifdef INFINITY
#undef INFINITY
#endif

#ifdef EPSILON
#undef EPSILON
#endif

#define PI						3.14159265358979323846f
#define PI_MUL_2				6.28318530717958647692f
#define PI_DIV_2				1.57079632679489661923f
#define PI_DIV_4				0.78539816339744830961f
#define E						2.71828182845904523536f
#define SQRT_2					1.41421356237309504880f
#define SQRT_3					1.73205080756887729352f
#define SQRT_1_OVER_2			0.70710678118654752440f
#define SQRT_1_OVER_3			0.57735026918962576450f
#define DEG_TO_RAD				0.01745329251994329576f
#define RAD_TO_DEG				57.2957795130823208768f
#define SEC_TO_MS				1000.0f
#define MS_TO_SEC				0.001f
#define INFINITY				1e30f
#define EPSILON					0.00001f
#define MAXIMUM					3.402823466e+38F


//////////////////////////////////////////////////////////////////////////
//	functions
//////////////////////////////////////////////////////////////////////////
#define sx_floor( x )			( floorf( x ) )
#define sx_ceil( x )			( ceilf( x ) )
#define sx_ftoi( x )			( sint( x ) )
#define sx_pow( x, y )			( powf( x, y ) )
#define sx_sqrt( x )			( sqrt( x ) )
#define sx_exp( x )				( expf( x ) )
#define sx_log( x )				( logf( x ) )
#define sx_sin( x )				( sinf( x ) )
#define sx_cos( x )				( cosf( x ) )
#define sx_tan( x )				( tanf( x ) )
#define sx_atan( x )			( atanf( x ) )

SEGAN_LIB_INLINE float sx_snap_to_int( const float x )
{
	return floorf( x + 0.5f );
}

SEGAN_LIB_INLINE float sx_max_f( const float x, const float y )
{
	return ( x > y ) ? x : y;
}

SEGAN_LIB_INLINE float sx_min_f( const float x, const float y )
{
	return ( x > y ) ? y : x;
}

SEGAN_LIB_INLINE sint sx_max_i( const sint x, const sint y )
{
	return ( x > y ) ? x : y;
}

SEGAN_LIB_INLINE sint sx_min_i( const sint x, const sint y )
{
	return ( x > y ) ? y : x;
}

SEGAN_LIB_INLINE float sx_max_3f( const float x, const float y, const float z )
{
	return ( x > y ) ? ( ( x > z ) ? x : z ) : ( ( y > z ) ? y : z );
}

SEGAN_LIB_INLINE sint sx_max_3i( const sint x, const sint y, const sint z )
{
	return ( x > y ) ? ( ( x > z ) ? x : z ) : ( ( y > z ) ? y : z );
}

SEGAN_LIB_INLINE float sx_sign_f( const float x )
{
	return ( x > 0 ) ? 1.0f : ( ( x < 0 ) ? -1.0f : 0.0f );
}

SEGAN_LIB_INLINE float sx_sqr_f( const float x )
{
	return x * x;
}

SEGAN_LIB_INLINE sint sx_sign_i( const sint x )
{
	return ( x > 0 ) ? 1 : ( ( x < 0 ) ? -1 : 0 );
}

SEGAN_LIB_INLINE sint sx_sqr_i( const sint x )
{
	return x * x;
}

SEGAN_LIB_INLINE bool sx_is_sqr( const sint x )
{
	return ( x & ( x - 1 ) ) == 0 && x > 0;
}

SEGAN_LIB_INLINE float sx_cube_f( const float x )
{
	return x * x * x;
}

SEGAN_LIB_INLINE float sx_abs_f( const float x )
{
	return ( x < 0 ) ? -x : x;
}

SEGAN_LIB_INLINE sint sx_cube_i( const sint x )
{
	return x << 2;
}

SEGAN_LIB_INLINE sint sx_abs_i( const sint x )
{
	return ( x < 0 ) ? -x : x;
}

SEGAN_LIB_INLINE float sx_guard_zero( const float x )
{
	return  ( sx_abs_f( x ) < EPSILON ) ? ( sx_sign_f( x ) * EPSILON ) : x;
}

//! fast float to sint conversion but uses current FPU round mode (default round nearest)
SEGAN_LIB_INLINE float sx_round( const float x )
{
#if defined(_WIN32)
	sint t;
	__asm {
		fld  x
		fistp t
	}
	return static_cast<float>(t);
#else
	return (sint)x;
#endif
}

//! fast sqrt root using Log Base 2 Approximation With One Extra Babylonian Steps
SEGAN_LIB_INLINE float sx_sqrt_fast( const float x )
{
	//  code from : http://ilab.usc.edu/wiki/index.php/Fast_Square_Root
	union
	{
		sint i;
		float x;
	} u;
	u.x = x;
	u.i = (1<<29) + (u.i >> 1) - (1<<22); 

	// One Babylonian Step
	u.x = 0.5f * (u.x + x / u.x);

	return u.x;
}

//! return the closest power of two number to the given number
SEGAN_LIB_INLINE uint sx_pow2_clossest( const uint x )
{
	uint i = 1;
	while ( i < x ) i += i;
	if ( 4 * x < 3 * i ) i >>= 1;
	return i;
}

//! return the upper power of two number from the given number
SEGAN_LIB_INLINE uint sx_pow2_upper( const uint x )
{
	uint i = 1;
	while ( i < x ) i += i;
	return i;
}

//! return the lower power of two number from the given number
SEGAN_LIB_INLINE uint sx_pow2_lower( const uint x )
{
	uint i = 1;
	while ( i <= x ) i += i;
	return i >> 1;
}

//! clamp value between min and max
SEGAN_LIB_INLINE sint sx_clamp_i( const sint value, const sint imin, const sint imax )
{
	sint result = value;
	if ( result < imin ) result = imin;
	if ( result > imax ) result = imax;
	return result;
}

//! clamp value between min and max
SEGAN_LIB_INLINE float sx_clamp_f( const float value, const float fmin, const float fmax )
{
	float result = value;
	if ( result < fmin ) result = fmin;
	if ( result > fmax ) result = fmax;
	return result;
}

//! return true if value is in range
SEGAN_LIB_INLINE float sx_between_i( const sint value, const sint imin, const sint imax )
{
	return ( imin <= value && value <= imax );
}

//! return true if value is in range
SEGAN_LIB_INLINE float sx_between_f( const float value, const float fmin, const float fmax )
{
	return ( fmin <= value && value <= fmax );
}

//! normalize angle between 0 and 360
SEGAN_LIB_INLINE float sx_angle_normalize_360( const float angle )
{
	if ( ( angle >= 360.0f ) || ( angle < 0.0f ) )
		return angle - ( sx_floor( angle / 360.0f ) * 360.0f );
	else
		return angle;
}

//! normalize angle between 0 and 180
SEGAN_LIB_INLINE float sx_angle_normalize_180( const float angle )
{
	if ( sx_angle_normalize_360( angle ) > 180.0f )
		return angle - 360.0f;
	else
		return angle;
}

//! compute sine and cosine of the angle x in same time
SEGAN_LIB_INLINE void sx_sin_cos( const float IN x, float& OUT s, float& OUT c)
{
#if defined(_WIN32)
	_asm {
		fld		x
		fsincos
		mov		ecx, c
		mov		edx, s
		fstp	dword ptr [ecx]
		fstp	dword ptr [edx]
	}
#else
	s = sinf( x );
	c = cosf( x );
#endif
}

//! return arc sine( x ). input is clamped to [-1, 1] to avoid a silent NaN
SEGAN_LIB_INLINE float sx_asin( const float x )
{
	if ( x >=  1.0f ) return  PI_MUL_2;
	if ( x <= -1.0f ) return -PI_MUL_2;
	return asinf( x );
}

//! return arc cosine( x ). input is clamped to [-1, 1] to avoid a silent NaN
SEGAN_LIB_INLINE float sx_acos( const float x )
{
	if ( x <= -1.0f ) return PI;
	if ( x >=  1.0f ) return 0.0f;
	return acosf( x );
}

//! return sine( x ) from table. maximum absolute error is 0.001f
SEGAN_LIB_API float sx_sin_fast( const float x );

//! return cosine( x ) from table. maximum absolute error is 0.001f
SEGAN_LIB_API float sx_cos_fast( const float x );

//! compute sine and cosine of the angle x in same time. maximum absolute error is 0.001f
SEGAN_LIB_API void sx_sin_cos_fast( const float IN x, float& OUT s, float& OUT c);

//! fill the random table by external data. data should contain at least 4096 unsigned integer number between 0 and RAND_MAX 
SEGAN_LIB_API void sx_random_set_data( const uint* data );

//! sync random table counter by other applications
SEGAN_LIB_API void sx_random_sync( const uint index );

//! return random number from the random table
SEGAN_LIB_API float sx_random_f( const float range );

//! return random number from the random table
SEGAN_LIB_API sint sx_random_i( const sint range );

//! return random number from the random table
SEGAN_LIB_INLINE float sx_random_f_limit( const float minRange, const float maxRange )
{
	float len = maxRange - minRange;
	return sx_random_f( len ) + minRange;
}

//! return random number from the random table
SEGAN_LIB_INLINE sint sx_random_i_limit( const sint minRange, const sint maxRange )
{
	sint len = maxRange - minRange + 1;
	return sx_random_i( len ) + minRange;
}


#include "math/Math_vec.h"
#include "math/Math_utils.h"
#include "math/Math_tools.h"

#endif	//	GUARD_Math_HEADER_FILE

