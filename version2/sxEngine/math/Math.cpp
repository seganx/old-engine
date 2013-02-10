#include "Math.h"
#include "Math_SSE.h"
#include "Math_Generic.h"

#include "../System/Log.h"

enum MathModule
{
	MM_NULL	= 0,
	MM_GENERIC,
	MM_SSE,
	MM_SSE2,
	MM_SSE3,
} s_math_module = MM_NULL;

#define SX_MATH_GENERIC		0
#define SX_MATH_SSE			1


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


SEGAN_API void sx_math_initialize( bool useGenericMath /*= false*/ )
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
				s_math_module = MM_SSE;
			if ( HasSSE2() )
				s_math_module = MM_SSE2;
			// 		if ( HasSSE3() )
			// 			s_math_module = MM_SSE3;
		}
		else
		{
			g_logger->Log( L"Warning: CPUID not found for math initialization! using generic math functions." );
		}
	}
}

SEGAN_API void sx_math_finalize( void )
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
	memcpy(&_11, p, sizeof(Matrix));
}

SEGAN_INLINE Matrix::Matrix( const Matrix& m )
{
	memcpy(&_11, &m._11, sizeof(Matrix));
}

SEGAN_INLINE Matrix::Matrix( float f11, float f12, float f13, float f14, float f21, float f22, float f23, float f24, float f31, float f32, float f33, float f34, float f41, float f42, float f43, float f44 )
{
	_11 = f11; _12 = f12; _13 = f13; _14 = f14;
	_21 = f21; _22 = f22; _23 = f23; _24 = f24;
	_31 = f31; _32 = f32; _33 = f33; _34 = f34;
	_41 = f41; _42 = f42; _43 = f43; _44 = f44;
}

SEGAN_INLINE void Matrix::Empty( void )
{
	memset( this, 0, sizeof(Matrix) );
}

SEGAN_INLINE void Matrix::Identity( void )
{
	_11=1; _12=0; _13=0; _14=0;
	_21=0; _22=1; _23=0; _24=0;
	_31=0; _32=0; _33=1; _34=0;
	_41=0; _42=0; _43=0; _44=1;
}

SEGAN_INLINE bool Matrix::EqualTo( const Matrix& mat )
{
	for ( int i=0; i<4; i++ )
	{
		for ( int j=0; i<4; i++ )
		{
			if ( sx_abs( m[i][j] - mat.m[i][j] ) > EPSILON) return false;
		}
	}
	return true;
}

SEGAN_INLINE float Matrix::Determinant( void )
{
	switch ( s_math_module )
	{
	case MM_GENERIC:	return gen_matrix_det( *this );
	case MM_SSE:
	case MM_SSE2:		return 0; //sse_matrix_det( *this );
	}
	return 0;
}

SEGAN_INLINE void Matrix::Transpose( const Matrix& m )
{
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_Transpose( this, &m );		break;
	case MM_SSE:
	case MM_SSE2:		break;//sse_matrix_Tanspose( *this );		break;
	}
}

SEGAN_INLINE void Matrix::Inverse( const Matrix& m )
{
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_Inverse( this, &m );		break;
	case MM_SSE:
	case MM_SSE2:		break;//sse_matrix_Inverse( *this );		break;
	}
}

SEGAN_INLINE void Matrix::Add( const Matrix& m1, const Matrix& m2 )
{
 	switch ( s_math_module )
 	{
 	case MM_GENERIC:	gen_matrix_add( *this, m1, m2 );		break;
 	case MM_SSE:
	case MM_SSE2:		sse_matrix_add( *this, m1, m2 );		break;
 	}
}

SEGAN_INLINE void Matrix::Subtract( const Matrix& m1, const Matrix& m2 )
{
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_sub( *this, m1, m2 );		break;
	case MM_SSE:
	case MM_SSE2:		sse_matrix_sub( *this, m1, m2 );		break;
	}
}

SEGAN_INLINE void Matrix::Multiply( const Matrix& m1, const Matrix& m2 )
{
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_mul( *this, m1, m2 );		break;
	case MM_SSE:
	case MM_SSE2:		sse_matrix_mul( *this, m1, m2 );		break;
	}
}

SEGAN_INLINE void Matrix::Divide( const Matrix& m1, const Matrix& m2 )
{
	Matrix mat2Inv;
	mat2Inv.Inverse( m2 );
	Multiply( m1, m2 );
}

SEGAN_INLINE void Matrix::SetRotationPitchYawRoll( const float pitch, const float yaw, const float roll )
{
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_setrotate_pyr( this, pitch, yaw, roll );		break;
	case MM_SSE:
	case MM_SSE2:		break;//sse_matrix_setrotate_pyr( this, pitch, yaw, roll );		break;
	}
}

SEGAN_INLINE void Matrix::GetRotationPitchYawRoll( float& OUT pitch, float& OUT yaw, float& OUT roll )
{
	pitch = asinf( - _32 ); 

	if ( cosf( pitch ) > 0.001f )
	{ 
		yaw = atan2f( _12, _22 ); 
		roll = atan2f( _31, _33 ); 
	}
	else 
	{ 
		yaw = atan2f( -_21, _11 ); 
		roll = 0.0f; 
	} 
}

void Matrix::SetTranslation( const float x, const float y, const float z )
{
	_41 = x;
	_42 = y;
	_43 = z;
	_44 = 1;
}

void Matrix::Scale( const float x, const float y, const float z )
{
	switch ( s_math_module )
	{
	case MM_GENERIC:	gen_matrix_scale( this, x, y, z );		break;
	case MM_SSE:
	case MM_SSE2:		break;//sse_matrix_scale( this, pitch, yaw, roll );		break;
	}
}

void Matrix::LookAt( const float* eye, const float* at, const float* up )
{
	float dir[3] = { at[0] - eye[0], at[1] - eye[1], at[2] - eye[2] };
	float dirlen = sx_sqrt_fast( dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2] );
	dir[0] /= dirlen;
	dir[1] /= dirlen;
	dir[2] /= dirlen;

	float side[3] = { dir[1] * up[2] - dir[2] * up[1], dir[2] * up[0] - dir[0] * up[2], dir[0] * up[1] - dir[1] * up[0] };
	float sidelen = sx_sqrt_fast( side[0] * side[0] + side[1] * side[1] + side[2] * side[2] );
	side[0] /= sidelen;
	side[1] /= sidelen;
	side[2] /= sidelen;

	float upd[3] = { side[1] * dir[2] - side[2] * dir[1], side[2] * dir[0] - side[0] * dir[2], side[0] * dir[1] - side[1] * dir[0] };
	float uplen = sx_sqrt_fast( upd[0] * upd[0] + upd[1] * upd[1] + upd[2] * upd[2] );
	upd[0] /= uplen;
	upd[1] /= uplen;
	upd[2] /= uplen;

	_11 = side[0];
	_12 = upd[0];
	_13 = dir[0];
	_14 = 0;

	_21 = side[1];
	_22 = upd[1];
	_23 = dir[1];
	_24 = 0;

	_31 = side[2];
	_32 = upd[2];
	_33 = dir[2];
	_34 = 0;

	_41 = - ( side[0] * eye[0] + side[1] * eye[1] + side[2] * eye[2] );
	_42 = -	( upd[0] * eye[0] + upd[1] * eye[1] + upd[2] * eye[2] );
	_43 = -	( dir[0] * eye[0] + dir[1] * eye[1] + dir[2] * eye[2] );
	_44 = 1.0f;

}

void Matrix::PerspectiveFov( float fov, float aspect, float nearZ, float farZ )
{
	float yscale = 1 / sx_tan( fov * 0.5f );
	float zfzn = farZ / ( farZ - nearZ );

	_11 = aspect * yscale;
	_12 = 0.0f;
	_13 = 0.0f;
	_14 = 0.0f;

	_21 = 0.0f;
	_22 = yscale;
	_23 = 0.0f;
	_24 = 0.0f;

	_31 = 0.0f;
	_32 = 0.0f;
	_33 = zfzn;
	_34 = 1.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = -nearZ * zfzn;
	_44 = 0.0f;

}

void Matrix::Orthographic( float width, float height, float nearZ, float farZ )
{
	float znzf = 1 / ( nearZ - farZ );

	_11 = 2 / width;
	_12 = 0.0f;
	_13 = 0.0f;
	_14 = 0.0f;

	_21 = 0.0f;
	_22 = 2 / height;
	_23 = 0.0f;
	_24 = 0.0f;

	_31 = 0.0f;
	_32 = 0.0f;
	_33 = znzf;
	_34 = 0.0f;

	_41 = 0.0f;
	_42 = 0.0f;
	_43 = nearZ * znzf;
	_44 = 1.0f;
}