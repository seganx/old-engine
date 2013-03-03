#include "Math.h"
#include "Math_Generic.h"
#include "../System/Log.h"

#if SEGAN_MATH_SIMD
#include "Math_SSE.h"
#endif

enum MathModule
{
	MM_NULL	= 0,
	MM_GENERIC,
	MM_SSE,
	MM_SSE2,
	MM_SSE3,
} s_math_module = MM_NULL;


//////////////////////////////////////////////////////////////////////////
//	CPU functions from id software.
//	Doom 3 GPL Source Code
//	Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 
bool HasCPUID( void ) 
{
	__asm 
	{
		pushfd						// save eflags
		pop		eax
		test	eax, 0x00200000		// check ID bit
		jz		set21				// bit 21 is not set, so jump to set_21
		and		eax, 0xffdfffff		// clear bit 21
		push	eax					// save new value in register
		popfd						// store new value in flags
		pushfd
		pop		eax
		test	eax, 0x00200000		// check ID bit
		jz		good
		jmp		err					// cpuid not supported
set21:
		or		eax, 0x00200000		// set ID bit
		push	eax					// store new value
		popfd						// store new value in EFLAGS
		pushfd
		pop		eax
		test	eax, 0x00200000		// if bit 21 is on
		jnz		good
		jmp		err
	}

err:
	return false;
good:
	return true;
}

#define _REG_EAX		0
#define _REG_EBX		1
#define _REG_ECX		2
#define _REG_EDX		3

void CPUID( int func, unsigned regs[4] )
{
	unsigned regEAX, regEBX, regECX, regEDX;

	__asm {
		pusha
		mov eax, func
		__emit 00fh
		__emit 0a2h
		mov regEAX, eax
		mov regEBX, ebx
		mov regECX, ecx
		mov regEDX, edx
		popa
	}

	regs[_REG_EAX] = regEAX;
	regs[_REG_EBX] = regEBX;
	regs[_REG_ECX] = regECX;
	regs[_REG_EDX] = regEDX;
}

bool HasSSE( void )
{
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 25 of EDX denotes SSE existence
	if ( regs[_REG_EDX] & ( 1 << 25 ) ) {
		return true;
	}
	return false;
}

bool HasSSE2( void )
{
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 26 of EDX denotes SSE2 existence
	if ( regs[_REG_EDX] & ( 1 << 26 ) ) {
		return true;
	}
	return false;
}

bool HasSSE3( void )
{
	unsigned regs[4];

	// get CPU feature bits
	CPUID( 1, regs );

	// bit 0 of ECX denotes SSE3 existence
	if ( regs[_REG_ECX] & ( 1 << 0 ) ) {
		return true;
	}
	return false;
}
//	Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
//	math init/finite implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_ENG_API void sx_math_initialize( bool useGenericMath /*= false*/ )
{
	if ( s_math_module )
	{
		g_logger->Log( L"Warning: calling sx_math_initialize() failed. Math system has been initialized !" );
		return;
	}

	s_math_module = MM_GENERIC;

	if ( useGenericMath )
	{
		g_logger->Log( L"Info: force to use generic math functions." );
	}
	else
	{
		if ( HasCPUID() )
		{
			if ( HasSSE() )
			{
				s_math_module = MM_SSE;
			}
			if ( HasSSE2() )
			{
				s_math_module = MM_SSE2;
			}
			// 		if ( HasSSE3() )
			// 			s_math_module = MM_SSE3;
		}
		else
		{
			g_logger->Log( L"Warning: CPUID not found for math initialization! using generic math functions." );
		}

		switch ( s_math_module )
		{
		case MM_GENERIC	: g_logger->Log( L" Math initialized : no sse instruction supported !" );	break;
		case MM_SSE		: g_logger->Log( L" Math initialized : no sse2 instruction supported !" );	break;
		case MM_SSE2	: g_logger->Log( L" Math initialized : sse2 instructions supported." );		break;
		}
	}
}

SEGAN_ENG_API void sx_math_finalize( void )
{
	if ( !s_math_module)
	{
		g_logger->Log( L"Warning! calling sx_math_finalize() failed. Math system is not initialized !" );
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
//	matrix implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE matrix::matrix( const float * p )
{
	memcpy( &m00, p, sizeof(matrix) );
}

SEGAN_INLINE matrix::matrix( const matrix& m )
{
	memcpy( &m00, &m.m00, sizeof(matrix) );
}

SEGAN_INLINE matrix::matrix( float f11, float f12, float f13, float f14, float f21, float f22, float f23, float f24, float f31, float f32, float f33, float f34, float f41, float f42, float f43, float f44 )
{
	m00 = f11; m01 = f12; m02 = f13; m03 = f14;
	m10 = f21; m11 = f22; m12 = f23; m13 = f24;
	m20 = f31; m21 = f32; m22 = f33; m23 = f34;
	m30 = f41; m31 = f42; m32 = f43; m33 = f44;
}

SEGAN_INLINE void matrix::Empty( void )
{
	sx_mem_set( this, 0, sizeof(matrix) );
}

SEGAN_INLINE void matrix::Identity( void )
{
	m00=1; m01=0; m02=0; m03=0;
	m10=0; m11=1; m12=0; m13=0;
	m20=0; m21=0; m22=1; m23=0;
	m30=0; m31=0; m32=0; m33=1;
}

SEGAN_INLINE bool matrix::EqualTo( const float* p )
{
	const matrix* mat = (const matrix*)p;
	for ( int i=0; i<4; i++ )
	{
		for ( int j=0; i<4; i++ )
		{
			if ( sx_abs_f( m[i][j] - mat->m[i][j] ) > EPSILON) return false;
		}
	}
	return true;
}

SEGAN_INLINE float matrix::Determinant( void )
{
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	return gen_matrix_det( this );
	case MM_SSE:		return gen_matrix_det( this );
	case MM_SSE2:		return sse_matrix_det( this );
	}
	return 0;
#else
	return gen_matrix_det( this );
#endif
}

SEGAN_INLINE void matrix::Transpose( const float* p )
{
	const matrix* mat = (const matrix*)p;
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_transpose( this, mat );	break;
	case MM_SSE:		gen_matrix_transpose( this, mat );	break;
	case MM_SSE2:		sse_matrix_transpose( this, mat );	break;
	}
#else
	gen_matrix_transpose( this, mat );
#endif
}

SEGAN_INLINE void matrix::Inverse( const float* p )
{
	const matrix* mat = (const matrix*)p;
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_inv( this, mat );		break;
	case MM_SSE:		gen_matrix_inv( this, mat );		break;
	case MM_SSE2:		sse_matrix_inv( this, mat );		break;
	}
#else
	gen_matrix_inv( this, mat );
#endif
}

SEGAN_INLINE void matrix::Add( const float* p1, const float* p2 )
{
	const matrix* mat1 = (const matrix*)p1;
	const matrix* mat2 = (const matrix*)p2;
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
 	{
 	case MM_GENERIC:	gen_matrix_add( this, mat1, mat2 );		break;
 	case MM_SSE:		gen_matrix_add( this, mat1, mat2 );		break;
	case MM_SSE2:		sse_matrix_add( this, mat1, mat2 );		break;
 	}
#else
	gen_matrix_add( this, mat1, mat2 );
#endif
}

SEGAN_INLINE void matrix::Subtract( const float* p1, const float* p2 )
{
	const matrix* mat1 = (const matrix*)p1;
	const matrix* mat2 = (const matrix*)p2;
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_sub( this, mat1, mat2 );		break;
	case MM_SSE:		gen_matrix_sub( this, mat1, mat2 );		break;
	case MM_SSE2:		sse_matrix_sub( this, mat1, mat2 );		break;
	}
#else
	gen_matrix_sub( this, mat1, mat2 );
#endif
}

SEGAN_INLINE void matrix::Multiply( const float* p1, const float* p2 )
{
	const matrix* mat1 = (const matrix*)p1;
	const matrix* mat2 = (const matrix*)p2;
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_mul( this, mat1, mat2 );		break;
	case MM_SSE:		gen_matrix_mul( this, mat1, mat2 );		break;
	case MM_SSE2:		sse_matrix_mul( this, mat1, mat2 );		break;
	}
#else
	gen_matrix_mul( this, mat1, mat2 );
#endif
}

SEGAN_INLINE void matrix::Divide( const float* m1, const float* m2 )
{
	matrix mat2Inv;
	mat2Inv.Inverse( m2 );
	Multiply( m1, m2 );
}

SEGAN_INLINE void matrix::SetRotationPitchYawRoll( const float pitch, const float yaw, const float roll )
{
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_setrotate_pyr( this, pitch, yaw, roll );		break;
	case MM_SSE:		gen_matrix_setrotate_pyr( this, pitch, yaw, roll );		break;
	case MM_SSE2:		sse_matrix_setrotate_pyr( this, pitch, yaw, roll );		break;
	}
#else
	gen_matrix_setrotate_pyr( this, pitch, yaw, roll );
#endif
}

SEGAN_INLINE void matrix::GetRotationPitchYawRoll( float& OUT pitch, float& OUT yaw, float& OUT roll )
{
	pitch = asinf( - m21 ); 

	if ( cosf( pitch ) > 0.001f )
	{ 
		yaw = atan2f( m01, m11 ); 
		roll = atan2f( m20, m22 ); 
	}
	else 
	{ 
		yaw = atan2f( -m10, m00 ); 
		roll = 0.0f; 
	} 
}

SEGAN_INLINE void matrix::SetDirection( const float* dir, const float* up )
{
#if 0
	Matrix lookat;
	float eye[3] = {0};
	lookat.LookAt( eye, dir, up );
	Inverse( lookat );
#else
	gen_matrix_direction( this, dir, up );
#endif
}

SEGAN_INLINE void matrix::GetDirection( float* OUT dir, float* OUT up /*= null */ ) const
{
#if 0
	float d[3] = { 0.0f, 0.0f, 1.0f };
	float u[3] = { 0.0f, 1.0f, 0.0f };
	//	TODO :
#else
	if ( dir )
	{
		dir[0] = m20;
		dir[1] = m21;
		dir[2] = m22;
	}

	if ( up )
	{
		up[0] = m10;
		up[1] = m11;
		up[2] = m12;
	}
#endif
}

SEGAN_INLINE void matrix::SetTranslation( const float x, const float y, const float z )
{
	m30 = x;
	m31 = y;
	m32 = z;
	m33 = 1.0f;
}

SEGAN_INLINE void matrix::TransformNormal( float* OUT dest, const float* src ) const
{
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_transform_norm( dest, src, this );	break;
	case MM_SSE:		gen_matrix_transform_norm( dest, src, this );	break;
	case MM_SSE2:		sse_matrix_transform_norm( dest, src, this );	break;
	}
#else
	gen_matrix_transform_norm( dest, src, this );
#endif
}

SEGAN_INLINE void matrix::TransformPoint( float* OUT dest, const float* src ) const
{
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_transform_point( dest, src, this );	break;
	case MM_SSE:		gen_matrix_transform_point( dest, src, this );	break;
	case MM_SSE2:		sse_matrix_transform_point( dest, src, this );	break;
	}
#else
	gen_matrix_transform_point( dest, src, this );
#endif
}

SEGAN_INLINE void matrix::Scale( const float x, const float y, const float z )
{
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_scale( this, x, y, z );		break;
	case MM_SSE:		gen_matrix_scale( this, x, y, z );		break;
	case MM_SSE2:		sse_matrix_scale( this, x, y, z );		break;
	}
#else
	gen_matrix_scale( this, x, y, z );
#endif
}

SEGAN_INLINE void matrix::LookAt( const float* eye, const float* at, const float* up )
{
#if 0
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_lookat( this, eye, at, up );		break;
	case MM_SSE:		gen_matrix_lookat( this, eye, at, up );		break;
	case MM_SSE2:		sse_matrix_lookat( this, eye, at, up );		break;
	}
#else
	gen_matrix_lookat( this, eye, at, up );
#endif
}

SEGAN_INLINE void matrix::PerspectiveFov( float fov, float aspect, float nearZ, float farZ )
{
	float yscale = 1 / sx_tan( fov * 0.5f );
	float zfzn = farZ / ( farZ - nearZ );

	m00 = aspect * yscale;
	m01 = 0.0f;
	m02 = 0.0f;
	m03 = 0.0f;

	m10 = 0.0f;
	m11 = yscale;
	m12 = 0.0f;
	m13 = 0.0f;

	m20 = 0.0f;
	m21 = 0.0f;
	m22 = zfzn;
	m23 = 1.0f;

	m30 = 0.0f;
	m31 = 0.0f;
	m32 = -nearZ * zfzn;
	m33 = 0.0f;

}

SEGAN_INLINE void matrix::Orthographic( float width, float height, float nearZ, float farZ )
{
	float znzf = 1 / ( nearZ - farZ );

	m00 = 2 / width;
	m01 = 0.0f;
	m02 = 0.0f;
	m03 = 0.0f;

	m10 = 0.0f;
	m11 = 2 / height;
	m12 = 0.0f;
	m13 = 0.0f;

	m20 = 0.0f;
	m21 = 0.0f;
	m22 = znzf;
	m23 = 0.0f;

	m30 = 0.0f;
	m31 = 0.0f;
	m32 = nearZ * znzf;
	m33 = 1.0f;
}


//////////////////////////////////////////////////////////////////////////
//	float2 implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE float2::float2( const float* p )
{
	memcpy( e, p, sizeof(float2) );
}

SEGAN_INLINE float2::float2( const float2& v )
{
	memcpy( e, &v, sizeof(float2) );
}

SEGAN_INLINE float2::float2( const float _x, const float _y )
{
	x = _x;
	y = _y;
}

SEGAN_INLINE void float2::Set( const float _x, const float _y )
{
	x = _x;
	y = _y;
}

SEGAN_INLINE void float2::Normalize( const float* v )
{
	const float len = sx_sqrt_fast( v[0] * v[0] + v[1] * v[1] );
	x = v[0] / len;
	y = v[1] / len;
}

SEGAN_INLINE float float2::Length( void ) const
{
	return sx_sqrt_fast( x * x + y * y );
}

SEGAN_INLINE float float2::Distance( const float* v ) const
{
	const float xlen = v[0] - x;
	const float ylen = v[1] - y;
	return sx_sqrt_fast( xlen * xlen + ylen * ylen );
}

SEGAN_INLINE float float2::Dot( const float* v ) const
{
	return ( x * v[0] + y * v[1] );
}

SEGAN_INLINE float float2::Angle( const float* v ) const
{
	const float vlen = sx_sqrt_fast( v[0] * v[0] + v[1] * v[1] );
	const float tlen = sx_sqrt_fast( x * x + y * y );
	const float vdot = ( x * v[0] + y * v[1] );
	return sx_acos( vdot / ( tlen * vlen ) );
}

SEGAN_INLINE void float2::Lerp( const float* v1, const float* v2, const float w )
{
	x = v1[0] + ( v2[0] - v1[0] ) * w;
	y = v1[1] + ( v2[1] - v1[1] ) * w;
}

//////////////////////////////////////////////////////////////////////////
//	float3 implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE float3::float3( const float* p )
{
	memcpy( e, p, sizeof(float3) );
}

SEGAN_INLINE float3::float3( const float3& v )
{
	memcpy( e, &v, sizeof(float3) );
}

SEGAN_INLINE float3::float3( const float _x, const float _y, const float _z )
{
	x = _x;
	y = _y;
	z = _z;
}

SEGAN_INLINE void float3::Set( const float _x, const float _y, const float _z )
{
	x = _x;
	y = _y;
	z = _z;
}

SEGAN_INLINE void float3::Normalize( const float* v )
{
	const float len = sx_sqrt_fast( ( v[0] * v[0] ) + ( v[1] * v[1] ) + ( v[2] * v[2] ) );
	x = v[0] / len;
	y = v[1] / len;
	z = v[2] / len;
}

SEGAN_INLINE float float3::Length( void ) const
{
	return sx_sqrt_fast( ( x * x ) + ( y * y ) + ( z * z ) );
}

SEGAN_INLINE float float3::Distance( const float* v ) const
{
	const float xlen = v[0] - x;
	const float ylen = v[1] - y;
	const float zlen = v[2] - z;
	return sx_sqrt_fast( ( xlen * xlen ) + ( ylen * ylen ) + ( zlen * zlen ) );
}

SEGAN_INLINE float float3::DistanceSqr( const float* v ) const
{
	const float xlen = v[0] - x;
	const float ylen = v[1] - y;
	const float zlen = v[2] - z;
	return ( xlen * xlen ) + ( ylen * ylen ) + ( zlen * zlen );
}

SEGAN_INLINE float float3::Dot( const float* v ) const
{
	return ( x * v[0] ) + ( y * v[1] ) + ( z * v[2] );
}

SEGAN_INLINE void float3::Cross( const float* v1, const float* v2 )
{
	x = ( v1[1] * v2[2] ) - ( v1[2] * v2[1] );
	y = ( v1[2] * v2[0] ) - ( v1[0] * v2[2] );
	z = ( v1[0] * v2[1] ) - ( v1[1] * v2[0] );
}

SEGAN_INLINE void float3::Lerp( const float* v1, const float* v2, const float w )
{
	x = v1[0] + ( v2[0] - v1[0] ) * w;
	y = v1[1] + ( v2[1] - v1[1] ) * w;
	z = v1[2] + ( v2[2] - v1[2] ) * w;
}

SEGAN_INLINE void float3::TransformPoint( const float* v, const float* _matrix )
{
	const matrix* mat = (const matrix*)_matrix;
	mat->TransformPoint( &x, v );
}

SEGAN_INLINE void float3::TransformNorm( const float* v, const float* _matrix )
{
	const matrix* mat = (const matrix*)_matrix;
	mat->TransformNormal( &x, v );
}

SEGAN_INLINE void float3::ProjectToScreen( const float* v, const float* worldViewProjection, const int* viewport )
{
	// prepare transformation matrix
	const matrix* mat = (const matrix*)worldViewProjection;

	// transform vector
	mat->TransformPoint( e, v );

	//  transform to screen space
	const float w = sx_abs_f( v[0] * mat->m03 + v[1] * mat->m13 + v[2] * mat->m23 + mat->m33 );
	x = 1.0f + ( x / w );
	y = 1.0f - ( y / w );

	//  rasterize to screen
	x *= (float)viewport[2] * 0.5f;
	y *= (float)viewport[3] * 0.5f;
	x += (float)viewport[0];
	y += (float)viewport[1];
}


//////////////////////////////////////////////////////////////////////////
//	float4 implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE float4::float4( const float* p )
{
	memcpy( e, p, sizeof(float4) );
}

SEGAN_INLINE float4::float4( const float4& v )
{
	memcpy( e, &v, sizeof(float4) );
}

SEGAN_INLINE float4::float4( const float* xyz, const float _w )
{
	memcpy( e, xyz, sizeof(float3) );
	w = _w;
}

SEGAN_INLINE float4::float4( const float _x, const float _y, const float _z, const float _w )
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

SEGAN_INLINE void float4::Set( const float _x, const float _y, const float _z, const float _w )
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

SEGAN_INLINE void float4::Normalize( const float* v )
{
	const float len = sx_sqrt_fast( ( v[0] * v[0] ) + ( v[1] * v[1] ) + ( v[2] * v[2] ) + ( v[3] * v[3] ) );
	x = v[0] / len;
	y = v[1] / len;
	z = v[2] / len;
	w = v[3] / len;
}

SEGAN_INLINE float float4::Length( void ) const
{
	return sx_sqrt_fast( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
}

SEGAN_INLINE float float4::LengthSqar( void ) const
{
	return ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
}

SEGAN_INLINE float float4::Dot( const float* v ) const
{
	return ( x * v[0] ) + ( y * v[1] ) + ( z * v[2] ) + ( w * v[3] );
}

SEGAN_INLINE void float4::Lerp( const float* v1, const float* v2, const float t )
{
	x = v1[0] + ( v2[0] - v1[0] ) * t;
	y = v1[1] + ( v2[1] - v1[1] ) * t;
	z = v1[2] + ( v2[2] - v1[2] ) * t;
	w = v1[3] + ( v2[3] - v1[3] ) * t;
}

//////////////////////////////////////////////////////////////////////////
//	quaternion implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE quat::quat( const float* p )
{
	memcpy( e, p, sizeof(quat) );
}

SEGAN_INLINE quat::quat( const quat& v )
{
	memcpy( e, &v, sizeof(quat) );
}

SEGAN_INLINE quat::quat( const float* xyz, const float _w )
{
	memcpy( e, xyz, sizeof(float3) );
	w = _w;
}

SEGAN_INLINE quat::quat( const float _x, const float _y, const float _z, const float _w )
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

SEGAN_INLINE void quat::Set( const float _x, const float _y, const float _z, const float _w )
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

SEGAN_INLINE void quat::Normalize( const float* v )
{
	const float len = sx_sqrt_fast( ( v[0] * v[0] ) + ( v[1] * v[1] ) + ( v[2] * v[2] ) + ( v[3] * v[3] ) );
	x = v[0] / len;
	y = v[1] / len;
	z = v[2] / len;
	w = v[3] / len;
}

SEGAN_INLINE float quat::Length( void ) const
{
	return sx_sqrt_fast( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
}

SEGAN_INLINE float quat::LengthSqar( void ) const
{
	return ( x * x ) + ( y * y ) + ( z * z ) + ( w * w );
}

SEGAN_INLINE float quat::Dot( const float* v ) const
{
	return ( x * v[0] ) + ( y * v[1] ) + ( z * v[2] ) + ( w * v[3] );
}

SEGAN_INLINE void quat::Conjugate( const float* q )
{
	x = - q[0];
	y = - q[1];
	z = - q[2];
	w =   q[3];
}

SEGAN_INLINE void quat::Lerp( const float* v1, const float* v2, const float t )
{
	x = v1[0] + ( v2[0] - v1[0] ) * t;
	y = v1[1] + ( v2[1] - v1[1] ) * t;
	z = v1[2] + ( v2[2] - v1[2] ) * t;
	w = v1[3] + ( v2[3] - v1[3] ) * t;
}

SEGAN_INLINE void quat::SLerp( const float* q1, const float* q2, const float t )
{
	quat start;
	float cosAngle = ( q1[0] * q2[0] ) + ( q1[1] * q2[1] ) + ( q1[2] * q2[2] ) + ( q1[3] * q2[3] );
	if ( cosAngle < 0.0f )
	{
		cosAngle = -cosAngle;
		start.x = - q1[0];
		start.y = - q1[1];
		start.z = - q1[2];
		start.w = - q1[3];
	}
	else
	{
		start.x = q1[0];
		start.y = q1[1];
		start.z = q1[2];
		start.w = q1[3];
	}

	float scale0, scale1;
	if ( cosAngle < 0.999f )
	{
		float angle = sx_acos( cosAngle );
		float recipSinAngle = ( 1.0f / sx_sin( angle ) );
		scale0 = ( sx_sin( ( ( 1.0f - t ) * angle ) ) * recipSinAngle );
		scale1 = ( sx_sin( ( t * angle ) ) * recipSinAngle );
	}
	else
	{
		scale0 = ( 1.0f - t );
		scale1 = t;
	}

	x = ( start.x * scale0 ) + ( q2[0] * scale1 );
	y = ( start.y * scale0 ) + ( q2[1] * scale1 );
	z = ( start.z * scale0 ) + ( q2[2] * scale1 );
	w = ( start.w * scale0 ) + ( q2[3] * scale1 );
}

SEGAN_INLINE void quat::SetRotationMatrix( const float* _matrix )
{
	const matrix* mat = (matrix*)_matrix;
	float xx = mat->m00;
	float yx = mat->m01;
	float zx = mat->m02;
	float xy = mat->m10;
	float yy = mat->m11;
	float zy = mat->m12;
	float xz = mat->m20;
	float yz = mat->m21;
	float zz = mat->m22;

	float trace = ( ( xx + yy ) + zz );
	sint negTrace = ( trace < 0.0f );
	sint ZgtX = ( zz > xx );
	sint ZgtY = ( zz > yy );
	sint YgtX = ( yy > xx );

	sint largestXorY = ( !ZgtX || !ZgtY ) && negTrace;
	sint largestYorZ = (  YgtX ||  ZgtX ) && negTrace;
	sint largestZorX = (  ZgtY || !YgtX ) && negTrace;

	if ( largestXorY )
	{
		zz = -zz;
		xy = -xy;
	}
	if ( largestYorZ )
	{
		xx = -xx;
		yz = -yz;
	}
	if ( largestZorX )
	{
		yy = -yy;
		zx = -zx;
	}

	float radicand = ( ( ( xx + yy ) + zz ) + 1.0f );
	float scale = ( 0.5f * ( 1.0f / sx_sqrt_fast( radicand ) ) );
	float tmpx = ( ( zy - yz ) * scale );
	float tmpy = ( ( xz - zx ) * scale );
	float tmpz = ( ( yx - xy ) * scale );
	float tmpw = ( radicand * scale );

	x = tmpx;
	y = tmpy;
	z = tmpz;
	w = tmpw;

	if ( largestXorY )
	{
		x = tmpw;
		y = tmpz;
		z = tmpy;
		w = tmpx;
	}

	if ( largestYorZ )
	{
		tmpx = x;
		tmpz = z;
		x = y;
		y = tmpx;
		z = w;
		w = tmpz;
	}
}

SEGAN_INLINE void quat::GetRotationMatrix( float* OUT _matrix )
{
	float qx2 = ( x + x );
	float qy2 = ( y + y );
	float qz2 = ( z + z );
	float qxqx2 = ( x * qx2 );
	float qxqy2 = ( x * qy2 );
	float qxqz2 = ( x * qz2 );
	float qxqw2 = ( w * qx2 );
	float qyqy2 = ( y * qy2 );
	float qyqz2 = ( y * qz2 );
	float qyqw2 = ( w * qy2 );
	float qzqz2 = ( z * qz2 );
	float qzqw2 = ( w * qz2 );
	matrix* mat = (matrix*)_matrix;
	mat->m00 = ( ( 1.0f - qyqy2 ) - qzqz2 );
	mat->m01 = ( qxqy2 + qzqw2 );
	mat->m02 = ( qxqz2 - qyqw2 );
	mat->m10 = ( qxqy2 - qzqw2 );
	mat->m11 = ( ( 1.0f - qxqx2 ) - qzqz2 );
	mat->m12 = ( qyqz2 + qxqw2 );
	mat->m20 = ( qxqz2 + qyqw2 );
	mat->m21 = ( qyqz2 - qxqw2 );
	mat->m22 = ( ( 1.0f - qxqx2 ) - qyqy2 );
}

SEGAN_INLINE void quat::RotationPitchYawRoll( const float yaw, const float pitch, const float roll )
{
	float sp, cp, sy, cy, sr, cr;
	sx_sin_cos( pitch * 0.5f, sp, cp );
	sx_sin_cos( yaw * 0.5f,	  sy, cy );
	sx_sin_cos( roll * 0.5f,  sr, cr );

	quat r1( cy * sp, sy * cp, - sy * sp, cy * cp );

	x = cr * r1.x - sr * r1.y;
	y = cr * r1.y + sr * r1.x;
	z = cr * r1.z + sr * r1.w;
	w = cr * r1.w - sr * r1.z;
}

SEGAN_INLINE void quat::Multiply( const float* q1, const float* q2 )
{
	const quat* quat1 = (quat*)q1;
	const quat* quat2 = (quat*)q2;
#if SEGAN_MATH_SIMD
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_quat_mul( this, quat1, quat2 );		break;
	case MM_SSE:		gen_quat_mul( this, quat1, quat2 );		break;
	case MM_SSE2:		sse_quat_mul( this, quat1, quat2 );		break;
	}
#else
	gen_quat_mul( this, mat1, mat2 );
#endif
}

SEGAN_INLINE void quat::Inverse( const float* q )
{
	sx_assert( 0 || "not implemented" );
}
