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
SEGAN_INLINE Matrix::Matrix( const float * p )
{
	memcpy( &m00, p, sizeof(Matrix) );
}

SEGAN_INLINE Matrix::Matrix( const Matrix& m )
{
	memcpy( &m00, &m.m00, sizeof(Matrix) );
}

SEGAN_INLINE Matrix::Matrix( float f11, float f12, float f13, float f14, float f21, float f22, float f23, float f24, float f31, float f32, float f33, float f34, float f41, float f42, float f43, float f44 )
{
	m00 = f11; m01 = f12; m02 = f13; m03 = f14;
	m10 = f21; m11 = f22; m12 = f23; m13 = f24;
	m20 = f31; m21 = f32; m22 = f33; m23 = f34;
	m30 = f41; m31 = f42; m32 = f43; m33 = f44;
}

SEGAN_INLINE void Matrix::Empty( void )
{
	sx_mem_set( this, 0, sizeof(Matrix) );
}

SEGAN_INLINE void Matrix::Identity( void )
{
	m00=1; m01=0; m02=0; m03=0;
	m10=0; m11=1; m12=0; m13=0;
	m20=0; m21=0; m22=1; m23=0;
	m30=0; m31=0; m32=0; m33=1;
}

SEGAN_INLINE bool Matrix::EqualTo( const float* p )
{
	const Matrix* mat = (const Matrix*)p;
	for ( int i=0; i<4; i++ )
	{
		for ( int j=0; i<4; i++ )
		{
			if ( sx_abs_f( m[i][j] - mat->m[i][j] ) > EPSILON) return false;
		}
	}
	return true;
}

SEGAN_INLINE float Matrix::Determinant( void )
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

SEGAN_INLINE void Matrix::Transpose( const float* p )
{
	const Matrix* mat = (const Matrix*)p;
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

SEGAN_INLINE void Matrix::Inverse( const float* p )
{
	const Matrix* mat = (const Matrix*)p;
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

SEGAN_INLINE void Matrix::Add( const float* p1, const float* p2 )
{
	const Matrix* mat1 = (const Matrix*)p1;
	const Matrix* mat2 = (const Matrix*)p2;
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

SEGAN_INLINE void Matrix::Subtract( const float* p1, const float* p2 )
{
	const Matrix* mat1 = (const Matrix*)p1;
	const Matrix* mat2 = (const Matrix*)p2;
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

SEGAN_INLINE void Matrix::Multiply( const float* p1, const float* p2 )
{
	const Matrix* mat1 = (const Matrix*)p1;
	const Matrix* mat2 = (const Matrix*)p2;
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

SEGAN_INLINE void Matrix::Divide( const float* m1, const float* m2 )
{
	Matrix mat2Inv;
	mat2Inv.Inverse( m2 );
	Multiply( m1, m2 );
}

SEGAN_INLINE void Matrix::SetRotationPitchYawRoll( const float pitch, const float yaw, const float roll )
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

SEGAN_INLINE void Matrix::GetRotationPitchYawRoll( float& OUT pitch, float& OUT yaw, float& OUT roll )
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

SEGAN_INLINE void Matrix::SetDirection( const float* dir, const float* up )
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

SEGAN_INLINE void Matrix::GetDirection( float* OUT dir, float* OUT up /*= null */ ) const
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

SEGAN_INLINE void Matrix::SetTranslation( const float x, const float y, const float z )
{
	m30 = x;
	m31 = y;
	m32 = z;
	m33 = 1.0f;
}

SEGAN_INLINE void Matrix::TransformNormal( float* OUT dest, const float* src ) const
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

SEGAN_INLINE void Matrix::TransformPoint( float* OUT dest, const float* src ) const
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

SEGAN_INLINE void Matrix::Scale( const float x, const float y, const float z )
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

SEGAN_INLINE void Matrix::LookAt( const float* eye, const float* at, const float* up )
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

SEGAN_INLINE void Matrix::PerspectiveFov( float fov, float aspect, float nearZ, float farZ )
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

SEGAN_INLINE void Matrix::Orthographic( float width, float height, float nearZ, float farZ )
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
	memcpy( &e, p, sizeof(float2) );
}

SEGAN_INLINE float2::float2( const float2& v )
{
	memcpy( &e, &v, sizeof(float2) );
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
//	float2 implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE float3::float3( const float* p )
{
	memcpy( &e, p, sizeof(float2) );
}

SEGAN_INLINE float3::float3( const float3& v )
{
	memcpy( &e, &v, sizeof(float2) );
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

SEGAN_INLINE void float3::TransformPoint( const float* v, const float* matrix )
{
	const Matrix* mat = (const Matrix*)matrix;
	mat->TransformPoint( &x, v );
}

SEGAN_INLINE void float3::TransformNorm( const float* v, const float* matrix )
{
	const Matrix* mat = (const Matrix*)matrix;
	mat->TransformNormal( &x, v );
}

SEGAN_INLINE void float3::ProjectToScreen( const float* v, const float* worldViewProjection, const int* viewport )
{
	// prepare transformation matrix
	const Matrix* mat = (const Matrix*)worldViewProjection;

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


