/********************************************************************
	created:	2013/03/07
	filename: 	Math_tools.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some functions and helpers for math
*********************************************************************/
#ifndef GUARD_Math_tools_HEADER_FILE
#define GUARD_Math_tools_HEADER_FILE


//////////////////////////////////////////////////////////////////////////
//	VECTOR 2D
//////////////////////////////////////////////////////////////////////////

//! return the length of vector
SEGAN_INLINE float sx_length( const float2& v )
{
	return sx_sqrt_fast( v.x * v.x + v.y * v.y );
}

//! return normalized vector
SEGAN_INLINE float2 sx_normalize( const float2& v )
{
	float len = sx_length( v );
	if ( len )	len = 1.0f / len;
	return float2( v.x * len, v.y * len );
}

//! return the value of dot product of two vectors
SEGAN_INLINE float sx_dot( const float2& v1, const float2& v2 )
{
	return v1.x * v2.x + v1.y * v2.y;
}

//! return the angle between of two vectors
SEGAN_INLINE float sx_angle( const float2& v1, const float2& v2 )
{
	return sx_acos( sx_dot( v1, v2 ) / ( sx_length( v1 ) * sx_length( v2 ) ) );
}

//! return interpolated vector of v1 and v2 by weight of t
SEGAN_INLINE float2 sx_lerp( const float2& v1, const float2& v2, const float t )
{
	return float2( 
		v1.x + ( v2.x - v1.x ) * t,
		v1.y + ( v2.y - v1.y ) * t
		);
}

//! return interpolated vector of v1 and v2 by weight of t
SEGAN_INLINE void sx_lerp( float2& res, const float2& v1, const float2& v2, const float t )
{
	res.x = v1.x + ( v2.x - v1.x ) * t;
	res.y = v1.y + ( v2.y - v1.y ) * t;
}

//! return distance between two vectors
SEGAN_INLINE float sx_distance( const float2& v1, const float2& v2 )
{
	const float xlen = v1.x - v2.x;
	const float ylen = v1.y - v2.y;
	return sx_sqrt_fast( xlen * xlen + ylen * ylen );
}

//! return distance between two vectors
SEGAN_INLINE float sx_distance_sqr( const float2& v1, const float2& v2 )
{
	const float xlen = v1.x - v2.x;
	const float ylen = v1.y - v2.y;
	return ( xlen * xlen + ylen * ylen );
}

//////////////////////////////////////////////////////////////////////////
//	VECTOR 3D
//////////////////////////////////////////////////////////////////////////

//! return the length of vector
SEGAN_INLINE float sx_length( const float3& v )
{
	return sx_sqrt_fast( v.x * v.x + v.y * v.y + v.z * v.z );
}

//! return normalized vector
SEGAN_INLINE float3 sx_normalize( const float3& v )
{
	float len = sx_length( v );
	if ( len )	len = 1.0f / len;
	return float3( v.x * len, v.y * len, v.z * len );
}

//! return cross product of v1 and v2
SEGAN_INLINE float3 sx_cross( const float3& v1, const float3& v2 )
{
	return float3( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x );
}

//! cross product of v1 and v2 into res
SEGAN_INLINE void sx_cross( float3& res, const float3& v1, const float3& v2 )
{
	res.x = v1.y * v2.z - v1.z * v2.y;
	res.y = v1.z * v2.x - v1.x * v2.z;
	res.z = v1.x * v2.y - v1.y * v2.x;
}

//! return the value of dot product of two vectors
SEGAN_INLINE float sx_dot( const float3& v1, const float3& v2 )
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

//! return the angle between of two vectors
SEGAN_INLINE float sx_angle( const float3& v1, const float3& v2 )
{
	return sx_acos( sx_dot( v1, v2 ) / ( sx_length( v1 ) * sx_length( v2 ) ) );
}

//! return distance between two vectors
SEGAN_INLINE float sx_distance( const float3& v1, const float3& v2 )
{
	const float xlen = v1.x - v2.x;
	const float ylen = v1.y - v2.y;
	const float zlen = v1.z - v2.z;
	return sx_sqrt_fast( xlen * xlen + ylen * ylen + zlen * zlen );
}


//! return distance between two vectors
SEGAN_INLINE float sx_distance_sqr( const float3& v1, const float3& v2 )
{
	const float xlen = v1.x - v2.x;
	const float ylen = v1.y - v2.y;
	const float zlen = v1.z - v2.z;
	return ( xlen * xlen + ylen * ylen + zlen * zlen );
}

//! return interpolated vector of v1 and v2 by weight of t
SEGAN_INLINE float3 sx_lerp( const float3& v1, const float3& v2, const float t )
{
	return float3( v1.x + ( v2.x - v1.x ) * t, v1.y + ( v2.y - v1.y ) * t, v1.z + ( v2.z - v1.z ) * t );
}

//! return interpolated vector of v1 and v2 by weight of t
SEGAN_INLINE void sx_lerp( float3& res, const float3& v1, const float3& v2, const float t )
{
	res.x = v1.x + ( v2.x - v1.x ) * t;
	res.y = v1.y + ( v2.y - v1.y ) * t;
	res.z = v1.z + ( v2.z - v1.z ) * t;
}


//////////////////////////////////////////////////////////////////////////
//	VECTOR 4D
//////////////////////////////////////////////////////////////////////////

//! return the length of vector
SEGAN_INLINE float sx_length( const float4& v )
{
	return sx_sqrt_fast( v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w );
}

//! return normalized vector
SEGAN_INLINE float4 sx_normalize( const float4& v )
{
	float len = sx_length( v );
	if ( len )	len = 1.0f / len;
	return float4( v.x * len, v.y * len, v.z * len, v.w * len );
}

//! return the value of dot product of two vectors
SEGAN_INLINE float sx_dot( const float4& v1, const float4& v2 )
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

//! return the angle between of two vectors
SEGAN_INLINE float sx_angle( const float4& v1, const float4& v2 )
{
	return sx_acos( sx_dot( v1, v2 ) / ( sx_length( v1 ) * sx_length( v2 ) ) );
}

//! return interpolated vector of v1 and v2 by weight of t
SEGAN_INLINE float4 sx_lerp( const float4& v1, const float4& v2, const float t )
{
	return float4( v1.x + ( v2.x - v1.x ) * t, v1.y + ( v2.y - v1.y ) * t, v1.z + ( v2.z - v1.z ) * t, v1.w + ( v2.w - v1.w ) * t );
}

//! return interpolated vector of v1 and v2 by weight of t
SEGAN_INLINE void sx_lerp( float4& res, const float4& v1, const float4& v2, const float t )
{
	res.x = v1.x + ( v2.x - v1.x ) * t;
	res.y = v1.y + ( v2.y - v1.y ) * t;
	res.z = v1.z + ( v2.z - v1.z ) * t;
	res.w = v1.w + ( v2.w - v1.w ) * t;
}

//////////////////////////////////////////////////////////////////////////
//	QUATERNION
//////////////////////////////////////////////////////////////////////////

//! return the value of dot product of two quaternion
SEGAN_INLINE float sx_dot( const quat& q1, const quat& q2 )
{
	return ( q1.x * q2.x ) + ( q1.y * q2.y ) + ( q1.z * q2.z ) + ( q1.w * q2.w );
}

//! return normalized quaternion
SEGAN_INLINE quat sx_normalize( const quat& q )
{
	float len = sx_sqrt_fast( q.x * q.x + q.y * q.y + q.z * q.z );
	if ( len )	len = 1.0f / len;
	return quat( q.x * len, q.y * len, q.z * len, q.w * len );
}

//! return interpolated quaternion of q1 and q2 by weight of t
SEGAN_INLINE quat sx_lerp( const quat& q1, const quat& q2, const float t )
{
	return quat( q1.x + ( q2.x - q1.x ) * t, q1.y + ( q2.y - q1.y ) * t, q1.z + ( q2.z - q1.z ) * t, q1.w + ( q2.w - q1.w ) * t );
}

//! return interpolated quaternion of q1 and q2 by weight of t
SEGAN_INLINE void sx_lerp( quat& res, const quat& q1, const quat& q2, const float t )
{
	res.x = q1.x + ( q2.x - q1.x ) * t;
	res.y = q1.y + ( q2.y - q1.y ) * t;
	res.z = q1.z + ( q2.z - q1.z ) * t;
	res.w = q1.w + ( q2.w - q1.w ) * t;
}

//! return spherical interpolated quaternion of q1 and q2 by weight of t
SEGAN_ENG_API quat SLerp( const quat& q1, const quat& q2, const float t );

//! return spherical interpolated quaternion of q1 and q2 by weight of t
SEGAN_ENG_API void SLerp( quat& res, const quat& q1, const quat& q2, const float t );

//! build the quaternion from a rotation matrix
SEGAN_ENG_API void sx_set_rotation( quat& res, const matrix& mat );

//! get rotation matrix from this quaternion
SEGAN_ENG_API void sx_get_rotation( matrix& res, const quat& q );

//! build this quaternion as rotation around X, Y and Z axis
SEGAN_ENG_API void sx_set_rotation_xyz( quat& res, const float pitch, const float yaw, const float roll );

//! add two quaternion and return the result
SEGAN_ENG_API quat sx_add( const quat& q1, const quat& q2 );

//! add two quaternion and put the result to res
SEGAN_ENG_API void sx_add( quat& res, const quat& q1, const quat& q2 );

//! quaternion multiplication. this will represents the rotation q2 followed by the rotation q1.  (return q2 * q1)
SEGAN_ENG_API quat sx_mul( const quat& q1, const quat& q2 );

//! quaternion multiplication. this will represents the rotation q2 followed by the rotation q1.  (res = q2 * q1)
SEGAN_ENG_API void sx_mul( quat& res, const quat& q1, const quat& q2 );


//////////////////////////////////////////////////////////////////////////
//	standard matrix 4x4
//////////////////////////////////////////////////////////////////////////

//! compare the matrix and return true if all elements are the same
SEGAN_INLINE bool sx_isequal( const matrix& m1, const matrix& m2 )
{
	for ( int i=0; i<4; i++ )
	{
		for ( int j=0; i<4; i++ )
		{
			if ( sx_abs_f( m1.m[i][j] - m2.m[i][j] ) > EPSILON) return false;
		}
	}
	return true;
}

//! calculate the determinant of matrix
SEGAN_ENG_API float sx_determinant( const matrix& mat );

//! calculate the transpose of matrix m into res. res = transpose(m)
SEGAN_ENG_API void sx_transpose( matrix& res, const matrix& m );

//! calculate the transpose of matrix
SEGAN_ENG_API matrix sx_transpose( const matrix& mat );

//! return the inverse of matrix
SEGAN_ENG_API matrix sx_inverse( const matrix& mat );

//! calculate the inverse of matrix m to res. res = inverse(m)
SEGAN_ENG_API void sx_inverse( matrix& res, const matrix& m );

//! add two matrix and return the result
SEGAN_ENG_API matrix sx_add( const matrix& mat1, const matrix& mat2 );

//! add two matrix and put the result to res. res = m1 + m2
SEGAN_ENG_API void sx_add( matrix& res, const matrix& mat1, const matrix& mat2 );

//! subtract two matrix and return the result
SEGAN_ENG_API matrix sx_sub( const matrix& mat1, const matrix& mat2 );

//! subtract two matrix and put the result to res. res = m1 - m2
SEGAN_ENG_API void sx_sub( matrix& res, const matrix& mat1, const matrix& mat2 );

//! multiply two matrix and return the result
SEGAN_ENG_API matrix sx_mul( const matrix& mat1, const matrix& mat2 );

//! multiply two matrix and put the result to res. res = m1 * m2
SEGAN_ENG_API void sx_mul( matrix& res, const matrix& mat1, const matrix& mat2 );

//! divide two matrix and put the result to res. res = m1 / m2
SEGAN_ENG_API matrix sx_divide( const matrix& mat1, const matrix& mat2 );

//! divide two matrix and put the result to res. res = m1 / m2
SEGAN_ENG_API void sx_divide( matrix& res, const matrix& mat1, const matrix& mat2 );

//! return a rotation matrix in pitch/yaw/roll system
SEGAN_ENG_API matrix sx_set_rotation_xyz( const float pitch, const float yaw, const float roll );

//! make res as a rotation matrix in pitch/yaw/roll system
SEGAN_ENG_API void sx_set_rotation_xyz( matrix& res, const float pitch, const float yaw, const float roll );

//! return rotation params in pitch/yaw/roll system
SEGAN_ENG_API float3 sx_get_rotation_xyz( const matrix& mat );

//! make this as a direction matrix
SEGAN_ENG_API void sx_set_direction( matrix& res, const float3& dir, const float3& up );

//! get direction of this matrix. dir and/or up vector can be null
SEGAN_ENG_API void sx_get_direction( const matrix& mat, float3* OUT dir, float3* OUT up /*= null */ );

//! transform src vector by matrix
SEGAN_ENG_API float3 sx_transform_normal( const float3& src, const matrix& mat );

//! transform src vector by matrix to the dest vector
SEGAN_ENG_API void sx_transform_normal( float3& dest, const float3& src, const matrix& mat );

//! transform src point by this matrix
SEGAN_ENG_API float3 sx_transform_point( const float3& src, const matrix& mat );

//! transform src point by this matrix to the dest point
SEGAN_ENG_API void sx_transform_point( float3& dest, const float3& src, const matrix& mat );

//! return an scaling matrix. Use this with care. some objects in 3d space have no absolute scale. like rigid bodies, triggers, etc
SEGAN_ENG_API matrix sx_scale( const float x, const float y, const float z );

//! make matrix res as an scaling matrix. Use this with care. some objects in 3d space have no absolute scale. like rigid bodies, triggers, etc
SEGAN_ENG_API void sx_scale( matrix& res, const float x, const float y, const float z );

//! return a look at matrix
SEGAN_ENG_API matrix sx_lookat( const float3& eye, const float3& at, const float3& up );

//! build a look at matrix on res
SEGAN_ENG_API void sx_lookat( matrix& res, const float3& eye, const float3& at, const float3& up );

//! return a perspective projection matrix
SEGAN_ENG_API matrix sx_perspective_fov( float fov, float aspect, float nearZ, float farZ );

//! Build an orthographic projection matrix
SEGAN_ENG_API matrix sx_orthographic( float width, float height, float nearZ, float farZ );

//////////////////////////////////////////////////////////////////////////
//	PLANE
//////////////////////////////////////////////////////////////////////////

//! return normalized plane 
SEGAN_INLINE Plane sx_normalize( const Plane& p )
{
	float len = sx_sqrt_fast( ( p.a * p.a ) + ( p.b * p.b ) + ( p.c * p.c ) );
	if ( len ) len = 1.0f / len;
	return Plane( p.a * len, p.b * len, p.c * len, p.d * len );
}

//! return distance of a point from the plane
SEGAN_INLINE float sx_distance( const Plane& plane, const float3& point )
{
	return ( plane.a * point.x + plane.b * point.y + plane.c * point.z + plane.d );
}


//////////////////////////////////////////////////////////////////////////
//	FRUSTUM
//////////////////////////////////////////////////////////////////////////

//! normalize the given frustum
SEGAN_INLINE void sx_normalize( Frustum& fr )
{
	fr.p0.Normalize();
	fr.p1.Normalize();
	fr.p2.Normalize();
	fr.p3.Normalize();
	fr.p4.Normalize();
	fr.p5.Normalize();
}


//! compute and return frustum from given matrix
SEGAN_ENG_API Frustum sx_get_frustum( const matrix& mat );

//! compute frustum from given matrix
SEGAN_ENG_API void sx_get_frustum( Frustum& res, const matrix& mat );


//////////////////////////////////////////////////////////////////////////
//	SPHERE
//////////////////////////////////////////////////////////////////////////

//! return distance between center of two sphere
SEGAN_INLINE float sx_distance_center( const Sphere& s1, const Sphere& s2 )
{
	return sx_distance( s1.center, s2.center );
}

//! return distance between edge of two sphere
SEGAN_INLINE float sx_distance_edge( const Sphere& s1, const Sphere& s2 )
{
	return sx_distance( s1.center, s2.center ) - s1.radius - s2.radius;
}

//! return distance between center of two sphere
SEGAN_INLINE float sx_distance_center_sqr( const Sphere& s1, const Sphere& s2 )
{
	return sx_distance_sqr( s1.center, s2.center );
}

//! return distance between edge of two sphere
SEGAN_INLINE float sx_distance_edge_sqr( const Sphere& s1, const Sphere& s2 )
{
	return sx_distance_sqr( s1.center, s2.center ) - ( s1.radius * s1.radius ) - ( s2.radius * s2.radius );
}

//! compute center and radius depend on the box
SEGAN_INLINE Sphere sx_get_sphere( const AABox& box )
{
	Sphere res;
	res.center = box.min + box.max;
	res.center *= 0.5f;
	float3 a = box.max - box.min;
	res.radius = sx_length( a ) / 2.0f;
	return res;
}

//! make res sphere cover the entry spheres
SEGAN_INLINE void sx_cover( Sphere& res, const Sphere& s1, const Sphere& s2 )
{
	float3 ver( s1.x - s2.x, s1.y - s2.y, s1.z - s2.z );
	float  len = sx_length( ver );

	//////////////////////////////////////////////////////////////////////////
	//  verify that spheres in not completely inside together

	//  check to see if s1 is inside of s2
	if ( len + s1.r < s2.r )
	{
		return s2;
	}
	//  check to see if s2 is inside of s1
	else if ( len + s2.r < s1.r )
	{
		return s1;
	}
	else
	{
		if ( len < EPSILON ) len = EPSILON;
		ver.x /= len;
		ver.y /= len;
		ver.z /= len;

		float3 c1 = s1.center + ( ver * s1.r );
		res.r = ( len + s1.r + s2.r ) * 0.5f;
		ver *= res.r;
		res.center = c1 - ver;
	}	
}

//! return a new sphere which covers the entry spheres
SEGAN_INLINE Sphere sx_cover( const Sphere& s1, const Sphere& s2 )
{
	Sphere res;
	sx_cover( res, s1, s2 );
	return res;
}

//! return true if the spheres intersect to each other and fill out the distance parameter 
SEGAN_INLINE bool sx_intersect( const Sphere& s1, const Sphere& s2, float* OUT distance = null )
{
	const float3 d( s2.x - s1.x, s2.y - s1.y, s2.z - s1.z );
	if ( distance )
	{
		*distance = sx_sqrt_fast( d.x * d.x + d.y * d.y + d.z * d.z );
		return ( *distance < ( s2.r + s1.r ) );
	}
	else
	{
		const float dis = ( d.x * d.x ) + ( d.y * d.y ) + ( d.z * d.z );
		const float rad = s2.r + s1.r;
		return ( dis < ( rad * rad ) );
	}
}

//! return transformed sphere by matrix
SEGAN_INLINE Sphere sx_transform( const Sphere& sphere, const matrix& mat )
{
	return Sphere( sx_transform_point( sphere.center, mat ), sphere.radius );
}



//////////////////////////////////////////////////////////////////////////
//	AXIS ALIGNED BOX
//////////////////////////////////////////////////////////////////////////

//! resize the res box to cover the entry box
SEGAN_INLINE void sx_cover( AABox& res, const AABox& b1, const AABox& b2 )
{
	res.x1 = sx_min_f( b1.x1, b2.x1 );
	res.y1 = sx_min_f( b1.y1, b2.y1 );
	res.z1 = sx_min_f( b1.z1, b2.z1 );
	res.x2 = sx_min_f( b1.x2, b2.x2 );
	res.y2 = sx_min_f( b1.y2, b2.y2 );
	res.z2 = sx_min_f( b1.z2, b2.z2 );
}

//! return a box which covers the entry box
SEGAN_INLINE AABox sx_cover( const AABox& b1, const AABox& b2 )
{
	return AABox(
		sx_min_f( b1.x1, b2.x1 ),
		sx_min_f( b1.y1, b2.y1 ),
		sx_min_f( b1.z1, b2.z1 ),
		sx_min_f( b1.x2, b2.x2 ),
		sx_min_f( b1.y2, b2.y2 ),
		sx_min_f( b1.z2, b2.z2 ) );
}


//! return volume of the box
SEGAN_INLINE float sx_get_volume( const AABox& box )
{
	return ( box.x2 - box.x1 ) * ( box.y2 - box.y1 ) * ( box.z2 - box.z1 );
}


//////////////////////////////////////////////////////////////////////////
//	utility functions
//////////////////////////////////////////////////////////////////////////

SEGAN_ENG_API float3 sx_project_to_screen( const float3& v, const matrix& worldViewProjection, const int x, const int y, const int width, const int height );


//! resize the current box to cover the entry box
SEGAN_INLINE void sx_Cover( AABox& res, const OBBox& box );


#endif	//	GUARD_Math_tools_HEADER_FILE