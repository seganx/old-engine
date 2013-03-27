#include "../Math.h"
#include "../Memory.h"
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
//	SPHERE
//////////////////////////////////////////////////////////////////////////

SEGAN_INLINE void sx_cover( Sphere& res, const Sphere& s1, const Sphere& s2 )
{
	float3 v( s1.x - s2.x, s1.y - s2.y, s1.z - s2.z );
	float  len = sx_length( v );

	//////////////////////////////////////////////////////////////////////////
	//  verify that spheres in not completely inside together

	//  check to see if s1 is inside of s2
	if ( len + s1.r < s2.r )
	{
		res = s2;
		return;
	}
	//  check to see if s2 is inside of s1
	else if ( len + s2.r < s1.r )
	{
		res = s1;
		return;
	}
	else
	{
		v.Normalize();
		res.center = ( s1.center + s2.center ) * 0.5f;
		res.center += v * ( s1.r * 0.5f );
		res.center -= v * ( s2.r * 0.5f );
		res.r = ( len + s1.r + s2.r ) * 0.5f;
	}	
}

//////////////////////////////////////////////////////////////////////////
//	AXES ALIGNED BOX
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE AABox sx_cover( const AABox& b1, const OBBox& b2 )
{
	float3 v[8];
	sx_get_points( v, b2 );

	AABox res( MAXIMUM, MAXIMUM, MAXIMUM, -MAXIMUM, -MAXIMUM, -MAXIMUM );
	for( int i = 0; i < 8; ++i )
	{
		if ( res.x1 > v[i].x ) res.x1 = v[i].x;
		if ( res.y1 > v[i].y ) res.y1 = v[i].y;
		if ( res.z1 > v[i].z ) res.z1 = v[i].z;

		if ( res.x2 < v[i].x ) res.x2 = v[i].x;
		if ( res.y2 < v[i].y ) res.y2 = v[i].y;
		if ( res.z2 < v[i].z ) res.z2 = v[i].z;
	}

	return AABox(
		sx_min_f( b1.x1, res.x1 ),
		sx_min_f( b1.y1, res.y1 ),
		sx_min_f( b1.z1, res.z1 ),
		sx_max_f( b1.x2, res.x2 ),
		sx_max_f( b1.y2, res.y2 ),
		sx_max_f( b1.z2, res.z2 ) );
}


//////////////////////////////////////////////////////////////////////////
//	RAY
//////////////////////////////////////////////////////////////////////////

SEGAN_INLINE Ray sx_ray( const float x, const float y, const float width, const float height, const matrix& view, const matrix& proj )
{
	/*	code from "Keith Ditchburn" (www.toymaker.info/Games/html/picking.html)	*/
	float3 v;
	v.x =  ( ( ( 2.0f * x ) / width  ) - 1.0f ) / proj.m00;
	v.y = -( ( ( 2.0f * y ) / height ) - 1.0f ) / proj.m11;
	v.z =  1.0f;

	matrix m;
	sx_inverse( m, view );

	// Transform the screen space pick ray into 3D space
	float3 dir;
	sx_transform_normal( dir, v, m );
	dir.Normalize();
	return Ray( float3( m.m30, m.m31, m.m32 ), dir );
}

SEGAN_INLINE Ray sx_transform( const Ray& ray, const matrix& mat )
{
	matrix m = sx_inverse( mat );
	return Ray( sx_transform_point( ray.pos, m ), sx_transform_normal( ray.dir, m ) );
}

SEGAN_INLINE bool sx_intersect( const Ray& ray, const Plane& plane, float3* outPoint /*= null*/, float3* outNormal /*= null*/ )
{
	// code from : www.bandedsoftware.com/hexgear/concepts/tutorials/ray/ray.html

	float3 n( plane.a, plane.b, plane.c );
	float d = sx_dot( n, ray.dir );

	if ( d >= EPSILON )
	{
		return false;
	}
	else
	{
		if ( outPoint )
		{
			float amount = - sx_distance( plane, ray.pos ) / d;
			*outPoint = ray.pos + ray.dir * amount;
		}

		if ( outNormal )
		{
			*outNormal = n;
		}

		return true;		
	}	
}

SEGAN_INLINE bool sx_intersect( const Ray& ray, const Sphere& sphere, float3* outPoint /*= null*/, float3* outNormal /*= null*/ )
{
	/*	code from "Tuomas Tonteri" ( www.sci.tuomastonteri.fi/programming/sse/example3 )	*/

	float a = sx_length_sqr( ray.dir );
 	float b = sx_dot( ray.dir, ray.pos - sphere.center ) * 2.0f;
 	float c = sx_length_sqr( sphere.center ) + sx_length_sqr( ray.pos ) - 2.0f * sx_dot( ray.pos, sphere.center ) - sphere.r * sphere.r;
 	float D = b * b - 4.0f * a * c;

	// If ray can not intersect then stop
	if ( D < 0 ) return false;
	D = sx_sqrt_fast( D );

	// Ray can intersect the sphere, solve the closer hit point
	float t = - 0.5f * ( b + D ) / a;
	if ( t > 0.0f )
	{
		float3 p = ray.pos + ray.dir * t;
		if (outPoint)	*outPoint = p;
		if (outNormal)	*outNormal = ( p - sphere.center ) / sphere.r;		
	}
	else
	{
		if (outPoint)	*outPoint = ray.pos;
		if (outNormal)	*outNormal = - ray.dir;
	}

	return true;
}

SEGAN_INLINE bool sx_intersect( const Ray& ray, const AABox& aabox, float3* outPoint, float3* outNormal )
{
	/*	code from "Sepehr Taghdissian" ( www.hmrEngine.com - sep.tagh@gmail.com )	*/

	float3 pos = ray.pos;
	float3 dir = ray.dir;
	AABox  box = aabox;

	// Check for point inside box, trivial reject, and determine parametric
	// distance to each front face

	bool inside = true;

	float xt, xn;
	if ( pos.x < box.min.x ) {
		xt = box.min.x - pos.x;
		xt /= dir.x;
		inside = false;
		xn = -1.0f;
	} else if ( pos.x > box.max.x ) {
		xt = box.max.x - pos.x;
		xt /= dir.x;
		inside = false;
		xn = 1.0f;
	} else {
		xt = -1.0f;
	}

	float yt, yn;
	if ( pos.y < box.min.y ) {
		yt = box.min.y - pos.y;
		yt /= dir.y;
		inside = false;
		yn = -1.0f;
	} else if ( pos.y > box.max.y ) {
		yt = box.max.y - pos.y;
		yt /= dir.y;
		inside = false;
		yn = 1.0f;
	} else {
		yt = -1.0f;
	}

	float zt, zn;
	if ( pos.z < box.min.z ) {
		zt = box.min.z - pos.z;
		zt /= dir.z;
		inside = false;
		zn = -1.0f;
	} else if ( pos.z > box.max.z ) {
		zt = box.max.z - pos.z;
		zt /= dir.z;
		inside = false;
		zn = 1.0f;
	} else {
		zt = -1.0f;
	}

	// Inside box?
	if ( inside ) 
	{
		if ( outPoint )		*outPoint = pos;
		if ( outNormal )	*outNormal = - dir;
		return true;
	}

	// Select farthest Plane - this is
	// the Plane of intersection.
	sint which = 0;
	float t = xt;
	if ( yt > t ) {
		which = 1;
		t = yt;
	}
	if ( zt > t ) {
		which = 2;
		t = zt;
	}

	switch ( which )
	{
		case 0: // intersect with yz Plane
			{
				float y = pos.y + dir.y*t;
				if ( y < box.min.y || y > box.max.y ) return false;
				float z = pos.z + dir.z*t;
				if ( z < box.min.z || z > box.max.z ) return false;

				if ( outNormal ) outNormal->Set( xn, 0.0f, 0.0f );
			} break;

		case 1: // intersect with xz Plane
			{
				float x = pos.x + dir.x*t;
				if ( x < box.min.x || x > box.max.x ) return false;
				float z = pos.z + dir.z*t;
				if ( z < box.min.z || z > box.max.z ) return false;

				if ( outNormal ) outNormal->Set( 0.0f, yn, 0.0f );
			} break;

		case 2: // intersect with xy Plane
			{
				float x = pos.x + dir.x*t;
				if ( x < box.min.x || x > box.max.x ) return false;
				float y = pos.y + dir.y*t;
				if ( y < box.min.y || y > box.max.y ) return false;

				if ( outNormal ) outNormal->Set( 0.0f, 0.0f, zn );
			} break;
	}

	if ( outPoint ) *outPoint = pos + dir * t;

	return true;
}

SEGAN_ENG_API bool sx_intersect( const Ray& ray, const AABox& box, const float far /*= MAXIMUM */ )
{
#if SEGAN_MATH_SIMD
	return sse_aabox_intersect( ray, box, far );
#else
	//	improved Smits’ method : http://cag.csail.mit.edu/~amy/papers/box-jgt.pdf
	//	improved by SeganX to use in BVH scene manager
	bool res = false;
	float tmin  = ( box.bounds[ray.sign[0]].x		- ray.pos.x ) * ray.dirInv.x;
	float tmax  = ( box.bounds[1 - ray.sign[0]].x	- ray.pos.x ) * ray.dirInv.x;
	float tymin = ( box.bounds[ray.sign[1]].y		- ray.pos.y ) * ray.dirInv.y;
	float tymax = ( box.bounds[1 - ray.sign[1]].y	- ray.pos.y ) * ray.dirInv.y;
	if ( tmin < tymax && tymin < tmax )
	{
		if ( tymin > tmin ) tmin = tymin;
		if ( tymax < tmax )	tmax = tymax;
		float tzmin = ( box.bounds[ray.sign[2]].z		- ray.pos.z ) * ray.dirInv.z;
		float tzmax = ( box.bounds[1 - ray.sign[2]].z	- ray.pos.z ) * ray.dirInv.z;
		if ( tmin < tzmax && tzmin < tmax )
		{
			if ( tzmin > tmin ) tmin = tzmin;
			if ( tzmax < tmax ) tmax = tzmax;
			res = ( tmax > 0 && tmin < far );
		}
	}
	return res;
#endif
}

SEGAN_INLINE bool sx_intersect( const Ray& ray, const OBBox& box, float3* outPoint /*= null*/, float3* outNormal /*= null*/ )
{
	Ray r = sx_transform( ray, box.world );
	bool res = sx_intersect( r, box.aabox, outPoint, outNormal );
	if ( outPoint ) sx_transform_point( *outPoint, *outPoint, box.world );
	if ( outNormal ) sx_transform_normal( *outNormal, *outNormal, box.world );
	return res;
}

SEGAN_INLINE bool sx_intersect( const Ray& ray, const float3& v0, const float3& v1, const float3& v2, float3* outPoint /*= null*/, float3* outNormal /*= null*/ )
{
	float3 edge1 = v1 - v0;
	float3 edge2 = v2 - v0;
	float3 pvec;

	sx_cross( pvec, ray.dir, edge2 ); 
	float det = sx_dot( edge1, pvec );

	if( det < EPSILON )
		return false;

	float3 tvec = ray.pos - v0;
	float u = sx_dot( tvec, pvec );
	if( u < 0.0f || u > det )
		return false;

	float3 qvec;
	sx_cross( qvec, tvec, edge1 );
	float v = sx_dot( ray.dir, qvec );
	if( v < 0.0f || u + v > det )
		return false;

	float t = sx_dot( edge2, qvec ) / det;
	if ( t < 0 ) return false;

	if ( outPoint )
	{
		*outPoint = ray.pos + ray.dir * t;
	}

	if ( outNormal )
	{
		qvec.Cross(edge1, edge2);
		outNormal->Normalize( qvec );
	}

	return true;
}


SEGAN_INLINE bool sx_intersect( const Ray& ray, const float3& v0, const float3& v1, const float3& v2, float2& outuv, const bool twoside /*= false */ )
{
	float3 edge1 = v1 - v0;
	float3 edge2 = v2 - v0;
	float3 pvec, tvec;

	sx_cross( pvec, ray.dir, edge2 ); 
	float det = sx_dot( edge1, pvec );
	if ( det > 0 )
	{
		tvec = ray.pos - v0;
	}
	else
	{
		if ( !twoside ) // for 2side detection remove this line 
			return false;		

		tvec = v0 - ray.pos; 
		det = - det;
	}

	if( det < EPSILON )
		return false;

	float u = sx_dot( tvec, pvec ); 
	if ( u < 0.0f || u > det )
		return false;

	float3 qvec;
	sx_cross( qvec, tvec, edge1 ); 
	float v = sx_dot( qvec, ray.dir );
	if ( v < 0.0f || (u + v) > det )
		return false;

	outuv.x = ( u / det ) - 0.5f;
	outuv.y = 0.5f - ( v / det );

	return true;
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


SEGAN_INLINE bool sx_intersect( const AABox& box, const Frustum& fr )
{
	float3 vmin, vmax;
	for ( int i = 0; i < 6; ++i )
	{ 
		if ( fr.p[i].a > 0 ) 
			vmin.x = box.x1, vmax.x = box.x2;
		else 
			vmin.x = box.x2, vmax.x = box.x1;

		if ( fr.p[i].b > 0 )
			vmin.y = box.y1, vmax.y = box.y2;
		else
			vmin.y = box.y2, vmax.y = box.y1;

		if ( fr.p[i].c > 0 )
			vmin.z = box.z1, vmax.z = box.z2;
		else
			vmin.z = box.z2, vmax.z = box.z1;

		if( sx_dot( (float3)fr.p[i], vmin ) + fr.p[i].d > 0 ) 
			return false;
	} 
	return true;
}

SEGAN_INLINE bool sx_intersect( const OBBox& box, const Frustum& fr )
{
	float3 v[8];
	sx_get_points( v, box );

	// check the visibility via intersect between box and frustum
	bool result = true;
	for (int i=0; i<6 && result; i++)
	{
		result = false;
		for (int j=0; j<8; j++)
		{
			if ( sx_distance( fr.p[i], v[j] ) >= 0 )
			{
				result = true;
				break;
			}
		}
	}

	return result;
}

SEGAN_INLINE void sx_convert_quat_triangle( float3* dest, const float3* src )
{
	sx_mem_copy( &dest[0], &src[0], sizeof(float3) * 3);
	sx_mem_copy( &dest[3], &src[0], sizeof(float3) );
	sx_mem_copy( &dest[4], &src[2], sizeof(float3) * 2);
}

SEGAN_INLINE void sx_convert_quat_triangle( float2* dest, const float2* src )
{
	sx_mem_copy( &dest[0], &src[0], sizeof(float2) * 3);
	sx_mem_copy( &dest[3], &src[0], sizeof(float2) );
	sx_mem_copy( &dest[4], &src[2], sizeof(float2) * 2);
}

SEGAN_INLINE void sx_convert_quat_triangle( Color* dest, const Color* src )
{
	sx_mem_copy( &dest[0], &src[0], sizeof(Color) * 3);
	sx_mem_copy( &dest[3], &src[0], sizeof(Color) );
	sx_mem_copy( &dest[4], &src[2], sizeof(Color) * 2);
}
