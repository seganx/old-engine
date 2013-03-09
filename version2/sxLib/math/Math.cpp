#include "../Math.h"
#include "Math_Generic.h"

#if SEGAN_MATH_SIMD
#include "Math_SSE.h"
#endif


//////////////////////////////////////////////////////////////////////////
//	quaternion implementation
//////////////////////////////////////////////////////////////////////////

SEGAN_INLINE quat SLerp( const quat& q1, const quat& q2, const float t )
{
	quat start;
	float cosAngle = ( q1.x * q2.x ) + ( q1.y * q2.y ) + ( q1.z * q2.z ) + ( q1.w * q2.w );
	if ( cosAngle < 0.0f )
	{
		cosAngle = -cosAngle;
		start.x = - q1.x;
		start.y = - q1.y;
		start.z = - q1.z;
		start.w = - q1.w;
	}
	else
	{
		start.x = q1.x;
		start.y = q1.y;
		start.z = q1.z;
		start.w = q1.w;
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

	return quat(
		( start.x * scale0 ) + ( q2.x * scale1 ),
		( start.y * scale0 ) + ( q2.y * scale1 ),
		( start.z * scale0 ) + ( q2.z * scale1 ),
		( start.w * scale0 ) + ( q2.w * scale1 ) );
}

SEGAN_INLINE void SLerp( quat& res, const quat& q1, const quat& q2, const float t )
{
	quat start;
	float cosAngle = ( q1.x * q2.x ) + ( q1.y * q2.y ) + ( q1.z * q2.z ) + ( q1.w * q2.w );
	if ( cosAngle < 0.0f )
	{
		cosAngle = -cosAngle;
		start.x = - q1.x;
		start.y = - q1.y;
		start.z = - q1.z;
		start.w = - q1.w;
	}
	else
	{
		start.x = q1.x;
		start.y = q1.y;
		start.z = q1.z;
		start.w = q1.w;
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

	res. x = ( start.x * scale0 ) + ( q2.x * scale1 );
	res. y = ( start.y * scale0 ) + ( q2.y * scale1 );
	res. z = ( start.z * scale0 ) + ( q2.z * scale1 );
	res. w = ( start.w * scale0 ) + ( q2.w * scale1 );
}

SEGAN_INLINE void sx_set_rotation( quat& res, const matrix& mat )
{
	float xx = mat.m00;
	float yx = mat.m01;
	float zx = mat.m02;
	float xy = mat.m10;
	float yy = mat.m11;
	float zy = mat.m12;
	float xz = mat.m20;
	float yz = mat.m21;
	float zz = mat.m22;

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

	res.x = tmpx;
	res.y = tmpy;
	res.z = tmpz;
	res.w = tmpw;

	if ( largestXorY )
	{
		res.x = tmpw;
		res.y = tmpz;
		res.z = tmpy;
		res.w = tmpx;
	}

	if ( largestYorZ )
	{
		tmpx = res.x;
		tmpz = res.z;
		res.x = res.y;
		res.y = tmpx;
		res.z = res.w;
		res.w = tmpz;
	}
}

SEGAN_INLINE void sx_get_rotation( matrix& res, const quat& q )
{
	float qx2 = ( q.x + q.x );
	float qy2 = ( q.y + q.y );
	float qz2 = ( q.z + q.z );
	float qxqx2 = ( q.x * qx2 );
	float qxqy2 = ( q.x * qy2 );
	float qxqz2 = ( q.x * qz2 );
	float qxqw2 = ( q.w * qx2 );
	float qyqy2 = ( q.y * qy2 );
	float qyqz2 = ( q.y * qz2 );
	float qyqw2 = ( q.w * qy2 );
	float qzqz2 = ( q.z * qz2 );
	float qzqw2 = ( q.w * qz2 );

	res.m00 = ( ( 1.0f - qyqy2 ) - qzqz2 );
	res.m01 = ( qxqy2 + qzqw2 );
	res.m02 = ( qxqz2 - qyqw2 );
	res.m03 = 0.0f;
	res.m10 = ( qxqy2 - qzqw2 );
	res.m11 = ( ( 1.0f - qxqx2 ) - qzqz2 );
	res.m12 = ( qyqz2 + qxqw2 );
	res.m13 = 0.0f;
	res.m20 = ( qxqz2 + qyqw2 );
	res.m21 = ( qyqz2 - qxqw2 );
	res.m22 = ( ( 1.0f - qxqx2 ) - qyqy2 );
	res.m23 = 0.0f;
	res.m30 = 0.0f;
	res.m31 = 0.0f;
	res.m32 = 0.0f;
	res.m33 = 0.0f;
}

SEGAN_INLINE void sx_set_rotation_xyz( quat& res, const float pitch, const float yaw, const float roll )
{
	float sp, cp, sy, cy, sr, cr;
	sx_sin_cos( pitch * 0.5f, sp, cp );
	sx_sin_cos( yaw * 0.5f,	  sy, cy );
	sx_sin_cos( roll * 0.5f,  sr, cr );

	quat r1( cy * sp, sy * cp, - sy * sp, cy * cp );

	res.x = cr * r1.x - sr * r1.y;
	res.y = cr * r1.y + sr * r1.x;
	res.z = cr * r1.z + sr * r1.w;
	res.w = cr * r1.w - sr * r1.z;
}


SEGAN_INLINE quat sx_add( const quat& q1, const quat& q2 )
{
	return quat( q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w );
}

SEGAN_INLINE void sx_add( quat& res, const quat& q1, const quat& q2 )
{
	res.x = q1.x + q2.x;
	res.y = q1.y + q2.y;
	res.z = q1.z + q2.z;
	res.w = q1.w + q2.w;
}

SEGAN_INLINE quat sx_mul( const quat& q1, const quat& q2 )
{
	quat res;
#if SEGAN_MATH_SIMD
	sse_quat_mul( &res, &q1, &q2 );
#else
	gen_quat_mul( &res, &q1, &q2 );
#endif
	return res;
}

SEGAN_INLINE void sx_mul( quat& res, const quat& q1, const quat& q2 )
{
#if SEGAN_MATH_SIMD
	sse_quat_mul( &res, &q1, &q2 );
#else
	gen_quat_mul( &res, &q1, &q2 );
#endif
}


//////////////////////////////////////////////////////////////////////////
//	matrix implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE float sx_determinant( const matrix& mat )
{
#if SEGAN_MATH_SIMD
	return sse_matrix_det( &mat );
#else
	return gen_matrix_det( &mat );
#endif
}

SEGAN_INLINE matrix sx_transpose( const matrix& mat )
{
	matrix res;
#if SEGAN_MATH_SIMD
	sse_matrix_transpose( &res, &mat );
#else
	gen_matrix_transpose( &res, &mat );
#endif
	return res;
}

SEGAN_INLINE void sx_transpose( matrix& res, const matrix& mat )
{
#if SEGAN_MATH_SIMD
	sse_matrix_transpose( &res, &mat );
#else
	gen_matrix_transpose( &res, &mat );
#endif
}

SEGAN_INLINE matrix sx_inverse( const matrix& mat )
{
	matrix res;
#if SEGAN_MATH_SIMD
	sse_matrix_inv( &res, &mat );
#else
	gen_matrix_inv( &res, &mat );
#endif
	return res;
}

SEGAN_INLINE void sx_inverse( matrix& res, const matrix& mat )
{
#if SEGAN_MATH_SIMD
	sse_matrix_inv( &res, &mat );
#else
	gen_matrix_inv( &res, &mat );
#endif
}

SEGAN_INLINE matrix sx_add( const matrix& mat1, const matrix& mat2 )
{
	matrix res;
#if SEGAN_MATH_SIMD
	sse_matrix_add( &res, &mat1, &mat2 );
#else
	gen_matrix_add( &res, &mat1, &mat2 );
#endif
	return res;
}

SEGAN_INLINE void sx_add( matrix& res, const matrix& mat1, const matrix& mat2 )
{
#if SEGAN_MATH_SIMD
	sse_matrix_add( &res, &mat1, &mat2 );
#else
	gen_matrix_add( &res, &mat1, &mat2 );
#endif
}

SEGAN_INLINE matrix sx_sub( const matrix& mat1, const matrix& mat2 )
{
	matrix res;
#if SEGAN_MATH_SIMD
	sse_matrix_sub( &res, &mat1, &mat2 );
#else
	gen_matrix_sub( &res, &mat1, &mat2 );
#endif
	return res;
}

SEGAN_INLINE void sx_sub( matrix& res, const matrix& mat1, const matrix& mat2 )
{
#if SEGAN_MATH_SIMD
	sse_matrix_sub( &res, &mat1, &mat2 );
#else
	gen_matrix_sub( &res, &mat1, &mat2 );
#endif
}

SEGAN_INLINE matrix sx_mul( const matrix& mat1, const matrix& mat2 )
{
	matrix res;
#if SEGAN_MATH_SIMD
	sse_matrix_mul( &res, &mat1, &mat2 );
#else
	gen_matrix_mul( &res, &mat1, &mat2 );
#endif
	return res;
}

SEGAN_INLINE void sx_mul( matrix& res, const matrix& mat1, const matrix& mat2 )
{
#if SEGAN_MATH_SIMD
	sse_matrix_mul( &res, &mat1, &mat2 );
#else
	gen_matrix_mul( &res, &mat1, &mat2 );
#endif
}

SEGAN_INLINE matrix sx_divide( const matrix& mat1, const matrix& mat2 )
{
	matrix mat2Inv, res;
	sx_inverse( mat2Inv, mat2 );
	sx_mul( res, mat1, mat2 );
	return res;
}

SEGAN_INLINE void sx_divide( matrix& res, const matrix& mat1, const matrix& mat2 )
{
	matrix mat2Inv;
	sx_inverse( mat2Inv, mat2 );
	sx_mul( res, mat1, mat2 );
}

SEGAN_INLINE matrix sx_set_rotation_xyz( const float pitch, const float yaw, const float roll )
{
	matrix res;
#if SEGAN_MATH_SIMD
	sse_matrix_setrotate_pyr( &res, pitch, yaw, roll );
#else
	gen_matrix_setrotate_pyr( &res, pitch, yaw, roll );
#endif
	return res;
}

SEGAN_INLINE void sx_set_rotation_xyz( matrix& res, const float pitch, const float yaw, const float roll )
{
#if SEGAN_MATH_SIMD
	sse_matrix_setrotate_pyr( &res, pitch, yaw, roll );
#else
	gen_matrix_setrotate_pyr( &res, pitch, yaw, roll );
#endif
}

SEGAN_INLINE float3 sx_get_rotation_xyz( const matrix& mat )
{
	float3 res;
	res.x = asinf( - mat.m21 ); 
	if ( cosf( res.x ) > 0.001f )
	{ 
		res.y = atan2f( mat.m01, mat.m11 );
		res.z = atan2f( mat.m20, mat.m22 );
	}
	else 
	{ 
		res.y = atan2f( - mat.m10, mat.m00 );
		res.z = 0.0f;
	}
	return res;
}

SEGAN_INLINE void sx_set_direction( matrix& res, const float3& dir, const float3& up )
{
	gen_matrix_direction( &res, dir, up );
}

SEGAN_INLINE void sx_get_direction( const matrix& mat, float3* OUT dir, float3* OUT up /*= null */ )
{
	if ( dir )
	{
		dir->x = mat.m20;
		dir->y = mat.m21;
		dir->z = mat.m22;
	}

	if ( up )
	{
		up->x = mat.m10;
		up->y = mat.m11;
		up->z = mat.m12;
	}
}

SEGAN_INLINE float3 sx_transform_normal( const float3& src, const matrix& mat )
{
	float3 res;
#if SEGAN_MATH_SIMD
	sse_matrix_transform_norm( &res.x, &src.x, &mat );
#else
	gen_matrix_transform_norm( &res.x, &src.x, &mat );
#endif
	return res;
}

SEGAN_INLINE void sx_transform_normal( float3& dest, const float3& src, const matrix& mat )
{
#if SEGAN_MATH_SIMD
	sse_matrix_transform_norm( &dest.x, &src.x, &mat );
#else
	gen_matrix_transform_norm( &dest.x, &src.x, &mat );
#endif
}

SEGAN_INLINE float3 sx_transform_point( const float3& src, const matrix& mat )
{
	float3 res;
#if SEGAN_MATH_SIMD
	sse_matrix_transform_point( &res.x, &src.x, &mat );
#else
	gen_matrix_transform_point( &res.x, &src.x, &mat );
#endif
	return res;
}

SEGAN_INLINE void sx_transform_point( float3& dest, const float3& src, const matrix& mat )
{
#if SEGAN_MATH_SIMD
	sse_matrix_transform_point( &dest.x, &src.x, &mat );
#else
	gen_matrix_transform_point( &dest.x, &src.x, &mat );
#endif
}

SEGAN_INLINE matrix sx_scale( const float x, const float y, const float z )
{
	matrix res;
#if 0//SEGAN_MATH_SIMD
	sse_matrix_scale( &res, x, y, z );
#else
	gen_matrix_scale( &res, x, y, z );
#endif
	return res;
}

SEGAN_INLINE void sx_scale( matrix& res, const float x, const float y, const float z )
{
#if 0//SEGAN_MATH_SIMD
	sse_matrix_scale( &res, x, y, z );
#else
	gen_matrix_scale( &res, x, y, z );
#endif
}

SEGAN_INLINE matrix sx_lookat( const float3& eye, const float3& at, const float3& up )
{
	matrix res;
#if 0
	sse_matrix_lookat( &res, eye, at, up );
#else
	gen_matrix_lookat( &res, eye, at, up );
#endif
	return res;
}

SEGAN_INLINE void sx_lookat( matrix& res, const float3& eye, const float3& at, const float3& up )
{
#if 0
	sse_matrix_lookat( &res, eye, at, up );
#else
	gen_matrix_lookat( &res, eye, at, up );
#endif
}

SEGAN_INLINE matrix sx_perspective_fov( float fov, float aspect, float nearZ, float farZ )
{
	const float yscale = 1.0f / sx_tan( fov * 0.5f );
	const float zfzn = farZ / ( farZ - nearZ );

	return matrix(
		aspect * yscale,	0.0f,			0.0f,			0.0f,
		0.0f,				yscale,			0.0f,			0.0f,
		0.0f,				0.0f,			zfzn,			1.0f,
		0.0f,				0.0f,			-nearZ * zfzn,	0.0f
		);

}

SEGAN_INLINE matrix sx_orthographic( float width, float height, float nearZ, float farZ )
{
	const float znzf = 1.0f / ( nearZ - farZ );
	return matrix(
		2.0f / width,	0.0f,			0.0f,			0.0f,
		0.0f,			2.0f / height,	0.0f,			0.0f,
		0.0f,			0.0f,			znzf,			0.0f,
		0.0f,			0.0f,			nearZ * znzf,	1.0f
		);
}

//////////////////////////////////////////////////////////////////////////
//	frustum implementation
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE Frustum sx_get_frustum( const matrix& mat )
{
	Frustum res;

	// Near clipping plane
	res.p0.a = mat.m02;
	res.p0.b = mat.m12;
	res.p0.c = mat.m22;
	res.p0.d = mat.m32;

	// Left clipping plane
	res.p1.a = mat.m03 + mat.m00;
	res.p1.b = mat.m13 + mat.m10;
	res.p1.c = mat.m23 + mat.m20;
	res.p1.d = mat.m33 + mat.m30;

	// Right clipping plane
	res.p2.a = mat.m03 - mat.m00;
	res.p2.b = mat.m13 - mat.m10;
	res.p2.c = mat.m23 - mat.m20;
	res.p2.d = mat.m33 - mat.m30;

	// Top clipping plane
	res.p3.a = mat.m03 - mat.m01;
	res.p3.b = mat.m13 - mat.m11;
	res.p3.c = mat.m23 - mat.m21;
	res.p3.d = mat.m33 - mat.m31;

	// Bottom clipping plane
	res.p4.a = mat.m03 + mat.m01;
	res.p4.b = mat.m13 + mat.m11;
	res.p4.c = mat.m23 + mat.m21;
	res.p4.d = mat.m33 + mat.m31;

	// Far clipping plane
	res.p5.a = mat.m03 - mat.m02;
	res.p5.b = mat.m13 - mat.m12;
	res.p5.c = mat.m23 - mat.m22;
	res.p5.d = mat.m33 - mat.m32;

	return res;
}

SEGAN_INLINE void sx_get_frustum( Frustum& res, const matrix& mat )
{
	// Near clipping plane
	res.p0.a = mat.m02;
	res.p0.b = mat.m12;
	res.p0.c = mat.m22;
	res.p0.d = mat.m32;

	// Left clipping plane
	res.p1.a = mat.m03 + mat.m00;
	res.p1.b = mat.m13 + mat.m10;
	res.p1.c = mat.m23 + mat.m20;
	res.p1.d = mat.m33 + mat.m30;

	// Right clipping plane
	res.p2.a = mat.m03 - mat.m00;
	res.p2.b = mat.m13 - mat.m10;
	res.p2.c = mat.m23 - mat.m20;
	res.p2.d = mat.m33 - mat.m30;

	// Top clipping plane
	res.p3.a = mat.m03 - mat.m01;
	res.p3.b = mat.m13 - mat.m11;
	res.p3.c = mat.m23 - mat.m21;
	res.p3.d = mat.m33 - mat.m31;

	// Bottom clipping plane
	res.p4.a = mat.m03 + mat.m01;
	res.p4.b = mat.m13 + mat.m11;
	res.p4.c = mat.m23 + mat.m21;
	res.p4.d = mat.m33 + mat.m31;

	// Far clipping plane
	res.p5.a = mat.m03 - mat.m02;
	res.p5.b = mat.m13 - mat.m12;
	res.p5.c = mat.m23 - mat.m22;
	res.p5.d = mat.m33 - mat.m32;
}


//////////////////////////////////////////////////////////////////////////
//	utility functions
//////////////////////////////////////////////////////////////////////////

SEGAN_INLINE float3 sx_project_to_screen( const float3& v, const matrix& worldViewProjection, const int x, const int y, const int width, const int height )
{
	float3 res;

	// transform vector
	sx_transform_point( res, v, worldViewProjection );

	//  transform to screen space
	const float w = sx_abs_f( v[0] * worldViewProjection.m03 + v[1] * worldViewProjection.m13 + v[2] * worldViewProjection.m23 + worldViewProjection.m33 );
	res.x = 1.0f + ( res.x / w );
	res.y = 1.0f - ( res.y / w );

	//  rasterize to screen
	res.x *= (float)width * 0.5f;
	res.y *= (float)height * 0.5f;
	res.x += (float)x;
	res.y += (float)y;

	return res;
}
