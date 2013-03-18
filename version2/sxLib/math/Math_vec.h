/********************************************************************
	created:	2013/03/07
	filename: 	Math_vec.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain basic math vector classes
*********************************************************************/
#ifndef GUARD_Math_vec_HEADER_FILE
#define GUARD_Math_vec_HEADER_FILE


//////////////////////////////////////////////////////////////////////////
//	INTEGER POINT 2D
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class int2
{
public:
	SEGAN_INLINE int2() {}
	SEGAN_INLINE int2( const int2& i ): x(i.x), y(i.y) {}
	SEGAN_INLINE int2( const sint* p ): x(p[0]), y(p[1]) {}
	SEGAN_INLINE int2( const sint _x, const sint _y ): x(_x), y(_y) {}

	SEGAN_INLINE bool operator== (const int2& i ) { return ( x == i.x ) && ( y == i.y ); }
	SEGAN_INLINE bool operator!= (const int2& i ) { return ( x != i.x ) || ( y != i.y ); }

public:
	sint x, y;
};

//////////////////////////////////////////////////////////////////////////
//	INTEGER RECT 4D
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class int4
{
public:
	SEGAN_INLINE int4() {}
	SEGAN_INLINE int4( const int4& i ): x(i.x), y(i.y), w(i.w), h(i.h) {}
	SEGAN_INLINE int4( const sint* p ): x(p[0]), y(p[1]), w(p[2]), h(p[3]) {}
	SEGAN_INLINE int4( const sint _x, const sint _y, const sint _w, const sint _h ): x(_x), y(_y), w(_w), h(_h) {}

	SEGAN_INLINE bool operator== (const int4& i ) { return ( x == i.x ) && ( y == i.y ) && ( w == i.w ) && ( h == i.h ); }
	SEGAN_INLINE bool operator!= (const int4& i ) { return ( x != i.x ) || ( y != i.y ) || ( w != i.w ) || ( h != i.h ); }

public:
	sint x, y, w, h;
};

//////////////////////////////////////////////////////////////////////////
//	VECTOR 2D
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class float2
{
public:
	//! constructors
	SEGAN_INLINE float2() {}
	SEGAN_INLINE float2( const float2& v ): x(v.x), y(v.y) {}
	SEGAN_INLINE float2( const float* p ): x(p[0]), y(p[1]) {}
	SEGAN_INLINE float2( const float _x, const float _y ): x(_x), y(_y) {}

	// assignment operators
	SEGAN_INLINE float2& operator += ( const float2& v ) { x += v.x; y += v.y; return *this; }
	SEGAN_INLINE float2& operator -= ( const float2& v ) { x -= v.x; y -= v.y; return *this; }
	SEGAN_INLINE float2& operator *= ( const float f )	 { x *= f; y *= f; return *this; }
	SEGAN_INLINE float2& operator /= ( const float f )	 { x /= f; y /= f; return *this; }

	// binary operators
	SEGAN_INLINE float2 operator + ( const float2& v ) const	{ return float2( x + v.x, y + v.y ); }
	SEGAN_INLINE float2 operator - ( const float2& v ) const	{ return float2( x - v.x, y - v.y ); }
	SEGAN_INLINE float2 operator * ( const float f ) const		{ return float2( x * f, y * f ); }
	SEGAN_INLINE float2 operator / ( const float f ) const		{ return float2( x / f, y / f ); }
	SEGAN_INLINE bool operator == ( const float2& v ) const		{ return ( x == v.x ) && ( y == v.y ); }
	SEGAN_INLINE bool operator != ( const float2& v ) const		{ return ( x != v.x ) || ( y != v.y ); }

	// unary operators
	SEGAN_INLINE float2 operator - () const	{ return float2( -x, -y ); }

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

	//! set new value for this vector
	SEGAN_INLINE float2& Set( const float _x, const float _y )
	{
		x = _x;
		y = _y;
		return *this;
	}

	//! return interpolated vector of v1 and v2 by weight of w to this
	SEGAN_INLINE float2& Lerp( const float2& v1, const float2& v2, const float w )
	{
		x = v1.x + ( v2.x - v1.x ) * w;
		y = v1.y + ( v2.y - v1.y ) * w;
		return *this;
	}

	//! normalize this vector
	SEGAN_INLINE float2& Normalize( void )
	{
		float len = sx_sqrt_fast( x * x + y * y );
		if ( len ) len = 1.0f / len;
		x *= len;
		y *= len;
		return *this;
	}

	//! normalize the vector v to this. this = normalize(v)
	SEGAN_INLINE float2& Normalize( const float2& v )
	{
		float len = sx_sqrt_fast( v.x * v.x + v.y * v.y );
		if ( len ) len = 1.0f / len;
		x = v.x * len;
		y = v.y * len;
		return *this;
	}


public:

	union {
		struct {
			float x;
			float y;
		};

		float e[2];	//	element of vector
	};
};

//////////////////////////////////////////////////////////////////////////
//	VECTOR 3D
//////////////////////////////////////////////////////////////////////////
class float3
{
public:
	//! constructors
	SEGAN_INLINE float3() {}
	SEGAN_INLINE float3( const float3& v ): x(v.x), y(v.y), z(v.z) {}
	SEGAN_INLINE float3( const float* p ): x(p[0]), y(p[1]), z(p[2]) {}
	SEGAN_INLINE float3( const float _x, const float _y, const float _z ): x(_x), y(_y), z(_z) {}

	// assignment operators
	SEGAN_INLINE float3& operator += ( const float3& v ) { x += v.x; y += v.y; z += v.z; return *this; }
	SEGAN_INLINE float3& operator -= ( const float3& v ) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	SEGAN_INLINE float3& operator *= ( const float f )	 { x *= f; y *= f; z *= f; return *this; }
	SEGAN_INLINE float3& operator /= ( const float f )	 { x /= f; y /= f; z /= f; return *this; }

	// binary operators
	SEGAN_INLINE float3 operator + ( const float3& v ) const	{ return float3( x + v.x, y + v.y, z + v.z ); }
	SEGAN_INLINE float3 operator - ( const float3& v ) const	{ return float3( x - v.x, y - v.y, z - v.z ); }
	SEGAN_INLINE float3 operator * ( const float f ) const		{ return float3( x * f, y * f, z * f ); }
	SEGAN_INLINE float3 operator / ( const float f ) const		{ return float3( x / f, y / f, z / f ); }
	SEGAN_INLINE bool operator == ( const float3& v ) const		{ return ( x == v.x ) && ( y == v.y ) && ( z == v.z ); }
	SEGAN_INLINE bool operator != ( const float3& v ) const		{ return ( x != v.x ) || ( y != v.y ) || ( z != v.z ); }

	// unary operators
	SEGAN_INLINE float3 operator - () const	{ return float3( -x, -y, -z ); }

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

	//! set new value for this vector
	SEGAN_INLINE float3& Set( const float _x, const float _y, const float _z )
	{
		x = _x;
		y = _y;
		z = _z;
		return *this;
	}

	//! return interpolated vector of v1 and v2 by weight of w to this
	SEGAN_INLINE float3& Lerp( const float3& v1, const float3& v2, const float w )
	{
		x = v1.x + ( v2.x - v1.x ) * w;
		y = v1.y + ( v2.y - v1.y ) * w;
		z = v1.z + ( v2.z - v1.z ) * w;
		return *this;
	}

	//! normalize this vector
	SEGAN_INLINE float3& Normalize( void )
	{
		float len = sx_sqrt_fast( x * x + y * y + z * z );
		if ( len ) len = 1.0f / len;
		x *= len;
		y *= len;
		z *= len;
		return *this;
	}

	//! normalize the vector v to this. this = normalize(v)
	SEGAN_INLINE float3& Normalize( const float3& v )
	{
		float len = sx_sqrt_fast( v.x * v.x + v.y * v.y + v.z * v.z );
		if ( len ) len = 1.0f / len;
		x = v.x * len;
		y = v.y * len;
		z = v.z * len;
		return *this;
	}

	//! return the vector of cross product of v1 and v2 to this. this = (v2 x v2)
	SEGAN_INLINE float3& Cross( const float3& v1, const float3& v2 )
	{
		x = v1.y * v2.z - v1.z * v2.y;
		y = v1.z * v2.x - v1.x * v2.z;
		z = v1.x * v2.y - v1.y * v2.x;
		return *this;
	}

public:

	union {
		struct {
			float x;
			float y;
			float z;
		};

		float e[3];	//	element of vector
	};
};


//////////////////////////////////////////////////////////////////////////
//	VECTOR 4D
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class float4
{
public:
	//! constructors
	SEGAN_INLINE float4() {}
	SEGAN_INLINE float4( const float4& v ): x(v.x), y(v.y), z(v.z), w(v.w) {}
	SEGAN_INLINE float4( const float* p ): x(p[0]), y(p[1]), z(p[2]), w(p[3]) {}
	SEGAN_INLINE float4( const float _x, const float _y, const float _z, const float _w ): x(_x), y(_y), z(_z), w(_w) {}

	// assignment operators
	SEGAN_INLINE float4& operator += ( const float4& v ) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	SEGAN_INLINE float4& operator -= ( const float4& v ) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	SEGAN_INLINE float4& operator *= ( const float f )	 { x *= f; y *= f; z *= f; w *= f; return *this; }
	SEGAN_INLINE float4& operator /= ( const float f )	 { x /= f; y /= f; z /= f; w /= f; return *this; }

	// binary operators
	SEGAN_INLINE float4 operator + ( const float4& v ) const	{ return float4( x + v.x, y + v.y, z + v.z, w + v.w ); }
	SEGAN_INLINE float4 operator - ( const float4& v ) const	{ return float4( x - v.x, y - v.y, z - v.z, w - v.w ); }
	SEGAN_INLINE float4 operator * ( const float f ) const		{ return float4( x * f, y * f, z * f, w * f ); }
	SEGAN_INLINE float4 operator / ( const float f ) const		{ return float4( x / f, y / f, z / f, w / f ); }
	SEGAN_INLINE bool operator == ( const float4& v ) const		{ return ( x == v.x ) && ( y == v.y ) && ( z == v.z ) && ( w == v.w ); }
	SEGAN_INLINE bool operator != ( const float4& v ) const		{ return ( x != v.x ) || ( y != v.y ) || ( z != v.z ) || ( w == v.w ); }

	// unary operators
	SEGAN_INLINE float4 operator - () const	{ return float4( -x, -y, -z, -w ); }

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

	//! set new value for this vector
	SEGAN_INLINE float4& Set( const float _x, const float _y, const float _z, const float _w )
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
		return *this;
	}

	//! return interpolated vector of v1 and v2 by weight of w to this
	SEGAN_INLINE float4& Lerp( const float4& v1, const float4& v2, const float _w )
	{
		x = v1.x + ( v2.x - v1.x ) * _w;
		y = v1.y + ( v2.y - v1.y ) * _w;
		z = v1.z + ( v2.z - v1.z ) * _w;
		w = v1.w + ( v2.w - v1.w ) * _w;
		return *this;
	}

	//! normalize this vector
	SEGAN_INLINE float4& Normalize( void )
	{
		float len = sx_sqrt_fast( x * x + y * y + z * z + w * w );
		if ( len ) len = 1.0f / len;
		x *= len;
		y *= len;
		z *= len;
		w *= len;
		return *this;
	}

	//! normalize the vector v to this. this = normalize(v)
	SEGAN_INLINE float4& Normalize( const float4& v )
	{
		float len = sx_sqrt_fast( v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w );
		if ( len ) len = 1.0f / len;
		x = v.x * len;
		y = v.y * len;
		z = v.z * len;
		w = v.w * len;
		return *this;
	}

public:

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};

		float e[4];	//	element of vector
	};
};


//////////////////////////////////////////////////////////////////////////
//	QUATERNION
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class quat
{
public:
	//! constructors
	SEGAN_INLINE quat() {}
	SEGAN_INLINE quat( const quat& v ): x(v.x), y(v.y), z(v.z), w(v.w) {}
	SEGAN_INLINE quat( const float* p ): x(p[0]), y(p[1]), z(p[2]), w(p[3]) {}
	SEGAN_INLINE quat( const float _x, const float _y, const float _z, const float _w ): x(_x), y(_y), z(_z), w(_w) {}

	SEGAN_INLINE operator const float* ( void ) const { return e; }

	//! set new value for this quaternion
	SEGAN_INLINE quat& Set( const float _x, const float _y, const float _z, const float _w )
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
		return *this;
	}

	//! return interpolated quaternion of q1 and q2 by weight of t to this
	SEGAN_INLINE quat& Lerp( const quat& q1, const quat& q2, const float t )
	{
		x = q1.x + ( q2.x - q1.x ) * t;
		y = q1.y + ( q2.y - q1.y ) * t;
		z = q1.z + ( q2.z - q1.z ) * t;
		w = q1.w + ( q2.w - q1.w ) * t;
		return *this;
	}

	//! normalize this quaternion
	SEGAN_INLINE quat& Normalize( void )
	{
		float len = sx_sqrt_fast( x * x + y * y + z * z );
		if ( len ) len = 1.0f / len;
		x *= len;
		y *= len;
		z *= len;
		w *= len;
		return *this;
	}

	//! normalize the quaternion q to this. this = normalize(v)
	SEGAN_INLINE quat& Normalize( const quat& v )
	{
		float len = sx_sqrt_fast( v.x * v.x + v.y * v.y + v.z * v.z );
		if ( len ) len = 1.0f / len;
		x = v.x * len;
		y = v.y * len;
		z = v.z * len;
		w = v.w * len;
		return *this;
	}

	//! (-x, -y, -z, w)
	SEGAN_INLINE quat& Conjugate( void )
	{
		x = - x;
		y = - y;
		z = - z;
		return *this;
	}

	//! (-x, -y, -z, w)
	SEGAN_INLINE quat& Conjugate( const quat& q )
	{
		x = - q.x;
		y = - q.y;
		z = - q.z;
		w =   q.w;
		return *this;
	}

public:

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};

		float e[4];	//	element of quaternion
	};
};

//////////////////////////////////////////////////////////////////////////
//	standard matrix 4x4
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class matrix
{
public:
	SEGAN_INLINE matrix() {}

	SEGAN_INLINE matrix( const matrix& mat )
		: m00(mat.m00), m01(mat.m01), m02(mat.m02), m03(mat.m03),
		  m10(mat.m10), m11(mat.m11), m12(mat.m12), m13(mat.m13), 
		  m20(mat.m20), m21(mat.m21), m22(mat.m22), m23(mat.m23), 
		  m30(mat.m30), m31(mat.m31), m32(mat.m32), m33(mat.m33) {}

	SEGAN_INLINE matrix( const float* p )
		: m00(p[ 0]), m01(p[ 1]), m02(p[ 2]), m03(p[ 3]),
		  m10(p[ 4]), m11(p[ 5]), m12(p[ 6]), m13(p[ 7]), 
		  m20(p[ 8]), m21(p[ 9]), m22(p[10]), m23(p[11]), 
		  m30(p[12]), m31(p[13]), m32(p[14]), m33(p[15]) {}

	SEGAN_INLINE matrix( 
		const float f11, const float f12, const float f13, const float f14,
		const float f21, const float f22, const float f23, const float f24,
		const float f31, const float f32, const float f33, const float f34,
		const float f41, const float f42, const float f43, const float f44 )
		: m00(f11), m01(f12), m02(f13), m03(f14),
		  m10(f21), m11(f22), m12(f23), m13(f24), 
		  m20(f31), m21(f32), m22(f33), m23(f34), 
		  m30(f41), m31(f42), m32(f43), m33(f44) {}

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return &m00; }

	//! zero all elements of this matrix
	SEGAN_INLINE matrix& Zero( void )
	{
		m00 = m01 = m02 = m03 = 0;
		m10 = m11 = m12 = m13 = 0;
		m20 = m21 = m22 = m23 = 0;
		m30 = m31 = m32 = m33 = 0;
		return *this;
	}

	//! make this matrix identity
	SEGAN_INLINE matrix& Identity( void )
	{
		m01 = m02 = m03 = 0;
		m10 = m12 = m13 = 0;
		m20 = m21 = m23 = 0;
		m30 = m31 = m32 = 0;
		m00 = m11 = m22 = m33 = 1.0f;
		return *this;
	}

 
 	//! set translation of this matrix without change of rotation
 	SEGAN_INLINE matrix& SetTranslation( const float x, const float y, const float z )
	{
		m30 = x; m30 = y; m32 = z;
		return *this;
	}

	//! make this as an scaling matrix. Use this with care. some objects in 3d space have no absolute scale. like rigid bodies, triggers, etc
	SEGAN_INLINE matrix& MakeScale( const float x, const float y, const float z )
	{
		m01 = m02 = m03 = 0;
		m10 = m12 = m13 = 0;
		m20 = m21 = m23 = 0;
		m30 = m31 = m32 = 0;
		m00 = x; m11 = y; m22 = z; m33 = 1.0f;
		return *this;
	}

	//! set scale of this matrix. Use this with care. some objects in 3d space have no absolute scale. like rigid bodies, triggers, etc
	SEGAN_INLINE matrix& SetScale( const float x, const float y, const float z )
	{
		m00 *= x; m11 *= y; m22 *= z;
		return *this;
	}

public:

	union {
		struct {
			float	m00, m01, m02, m03;
			float   m10, m11, m12, m13;
			float   m20, m21, m22, m23;
			float   m30, m31, m32, m33;
		};

		float	m[4][4];
	};
};
typedef matrix float4x4;


#endif	//	GUARD_Math_vec_HEADER_FILE

