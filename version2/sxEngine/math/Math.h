/********************************************************************
	created:	2013/09/19
	filename: 	Math.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain headers for advanced math library
*********************************************************************/
#ifndef GUARD_Math_HEADER_FILE
#define GUARD_Math_HEADER_FILE

#include "../../sxLib/Lib.h"

//////////////////////////////////////////////////////////////////////////
//	INTEGER POINT 2D
//////////////////////////////////////////////////////////////////////////
class int2
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
class float2
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
		m30 = x; m31 = y; m32 = z;
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



//////////////////////////////////////////////////////////////////////////
//	COLOR
//////////////////////////////////////////////////////////////////////////
class Color
{
public:
	SEGAN_INLINE Color() {}
	SEGAN_INLINE Color( const dword c ): code(c) {}
	SEGAN_INLINE Color( const Color& c ): code(c.code) {}
	SEGAN_INLINE Color( const float* p ) { Set( p[0], p[1], p[2], p[3] ); }
	SEGAN_INLINE Color( const byte _r, const byte _g, const byte _b, const byte _a ): r(_r), g(_g), b(_b), a(_a) { /*return ( ca << 24 ) | ( cr << 16 ) | ( cg << 8 ) | cb;*/ }
	SEGAN_INLINE Color( const float _r, const float _g, const float _b, const float _a ) { Set( _r, _g, _b, _a ); }

	// assignment operators
	SEGAN_INLINE Color& operator += ( const Color& c )	{ code = sx_clamp_u( code + c.code, 0, 0xffffffff ); return *this; }
	SEGAN_INLINE Color& operator -= ( const Color& c )	{ code = sx_clamp_u( code - c.code, 0, 0xffffffff ); return *this; }

	// binary operators
	SEGAN_INLINE Color operator + ( const Color& c ) const	{ return sx_clamp_u( code + c.code, 0, 0xffffffff ); }
	SEGAN_INLINE Color operator - ( const Color& c ) const	{ return sx_clamp_u( code - c.code, 0, 0xffffffff ); }
	SEGAN_INLINE bool operator == ( const Color& c ) const	{ return ( code == c.code ); }
	SEGAN_INLINE bool operator != ( const Color& c ) const	{ return ( code != c.code ); }

	//! conventional operators
	SEGAN_INLINE operator const dword ( void ) const { return code; }

	//! set new value for this color
	SEGAN_INLINE Color& Set( const float _r, const float _g, const float _b, const float _a )
	{
		r = _r >= 1.0f ? 0xff : ( _r <= 0.0f ? 0x00 : byte( _r * 255.0f + 0.5f ) );
		g = _g >= 1.0f ? 0xff : ( _g <= 0.0f ? 0x00 : byte( _g * 255.0f + 0.5f ) );
		b = _b >= 1.0f ? 0xff : ( _b <= 0.0f ? 0x00 : byte( _b * 255.0f + 0.5f ) );
		a = _a >= 1.0f ? 0xff : ( _a <= 0.0f ? 0x00 : byte( _a * 255.0f + 0.5f ) );
		return *this;
	}

	//! return interpolated color of c1 and c2 by weight of t to this
	SEGAN_INLINE Color& Lerp( const Color& c1, const Color& c2, const float t )
	{
		r = c1.r + byte( float( c2.r - c1.r ) * t );
		g = c1.g + byte( float( c2.g - c1.g ) * t );
		b = c1.b + byte( float( c2.b - c1.b ) * t );
		a = c1.a + byte( float( c2.a - c1.a ) * t );
		return *this;
	}


public:

	union {
		struct {
			byte r;
			byte g;
			byte b;
			byte a;
		};
		byte	e[4];	//	elements of color
		dword	code;	//	32bit color code in ARGB8 mode
	};
};

//////////////////////////////////////////////////////////////////////////
//	COLOR VECTOR 2
//////////////////////////////////////////////////////////////////////////
struct Color2
{
	Color c0;
	Color c1;
};

//////////////////////////////////////////////////////////////////////////
//	PLANE
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class Plane
{
public:
	SEGAN_INLINE Plane() {}
	SEGAN_INLINE Plane( const float* p ): a(p[0]), b(p[1]), c(p[2]), d(p[3]) {}
	SEGAN_INLINE Plane( const Plane& p ): a(p.a), b(p.b), c(p.c), d(p.d) {}
	SEGAN_INLINE Plane( const float _a, const float _b, const float _c, const float _d ): a(_a), b(_b), c(_c), d(_d) {}

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

	//! set new value for this plane
	SEGAN_INLINE Plane& Set( const float _a, const float _b, const float _c, const float _d )
	{
		a = _a; b = _b; c = _c; d = _d;
		return *this;
	}

	//! construct a plane from 3 points in space
	SEGAN_INLINE Plane& MakeFromPoints( const float3& p1, const float3& p2, const float3& p3 )
	{
		float3 v1( p2.x - p1.x, p2.y - p1.y, p2.z - p1.z );
		float3 v2( p3.x - p1.x, p3.y - p1.y, p3.z - p1.z );
		float3 nr( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x );
		return MakeFromNormal( p1, nr );
	}

	//! construct a plane from a point and a normal
	SEGAN_INLINE Plane& MakeFromNormal( const float3& p, const float3& n )
	{
		a = n.x; b = n.y; c = n.z; d = - ( p.x * n.x ) - ( p.y * n.y ) - ( p.z * n.z );
		return *this;
	}

	//! normalize plane p to this
	SEGAN_INLINE Plane& Normalize( const Plane& p )
	{
		float len = sx_sqrt_fast( ( p.a * p.a ) + ( p.b * p.b ) + ( p.c * p.c ) );
		if ( len ) len = 1.0f / len;
		a = p.a * len;
		b = p.b * len;
		c = p.c * len;
		d = p.d * len;
		return *this;
	}

	//! normalize itself
	SEGAN_INLINE Plane& Normalize( void )
	{
		float len = sx_sqrt_fast( ( a * a ) + ( b * b ) + ( c * c ) );
		if ( len ) len = 1.0f / len;
		a *= len;
		b *= len;
		c *= len;
		d *= len;
		return *this;
	}

public:

	union {
		struct {
			float a;
			float b;
			float c;
			float d;
		};

		float e[4];	//	elements of plane
	};
};


//////////////////////////////////////////////////////////////////////////
//	FRUSTUM
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class Frustum
{
public:
	SEGAN_INLINE Frustum() {};
	SEGAN_INLINE Frustum( const float* f ) { *this = *( (Frustum*)f ); }
	SEGAN_INLINE Frustum( const Frustum& f ) { *this = f; }
	SEGAN_INLINE Frustum( const float* _p0, const float* _p1, const float* _p2, const float* _p3, const float* _p4, const float* _p5 )
	{
		p0 = *( (Plane*)_p0 );
		p1 = *( (Plane*)_p1 );
		p2 = *( (Plane*)_p2 );
		p3 = *( (Plane*)_p3 );
		p4 = *( (Plane*)_p4 );
		p5 = *( (Plane*)_p5 );
	}

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return p0.e; }

	//! normalize frustum f to this
	SEGAN_INLINE Frustum& Normalize( const Frustum& fr )
	{
		p0.Normalize( fr.p0 );
		p1.Normalize( fr.p1 );
		p2.Normalize( fr.p2 );
		p3.Normalize( fr.p3 );
		p4.Normalize( fr.p4 );
		p5.Normalize( fr.p5 );
		return *this;
	}

public:

	union {
		struct {
			Plane p0;
			Plane p1;
			Plane p2;
			Plane p3;
			Plane p4;
			Plane p5;
		};

		struct {
			Plane p[6];	//	planes of the frustum
		};
	};

};

//////////////////////////////////////////////////////////////////////////
//	SPHERE
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class Sphere
{
public:
	SEGAN_INLINE Sphere() {}
	SEGAN_INLINE Sphere( const Sphere& s ): x(s.x), y(s.y), z(s.z), r(s.r) {}
	SEGAN_INLINE Sphere( const float3& c, const float _r ): x(c.x), y(c.y), z(c.z), r(_r) {}
	SEGAN_INLINE Sphere( const float _x, const float _y, const float _z, const float _r ): x(_x), y(_y), z(_z), r(_r) {}

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return &x; }

	//! resize the sphere to the zero
	SEGAN_INLINE Sphere& Zero( void )
	{
		x = 0; y = 0; z = 0; r = 0;
		return *this;
	}

	//! set new properties
	SEGAN_INLINE Sphere& Set( const float3& cen, const float rad )
	{
		center = cen;
		radius = rad;
		return *this;
	}

public:

	union{
		struct
		{
			float3	center;
			float	radius;
		};

		struct
		{
			float x;
			float y;
			float z;
			float r;
		};
	};

};

//////////////////////////////////////////////////////////////////////////
//	AXIS ALIGNED BOX
//////////////////////////////////////////////////////////////////////////
class AABox
{
public:
	SEGAN_INLINE AABox() {}
	SEGAN_INLINE AABox( const AABox& a ): min(a.min), max(a.max) {}
	SEGAN_INLINE AABox( const float3& _min, const float3& _max ): min(_min), max(_max) {}
	SEGAN_INLINE AABox( const float _x1, const float _y1, const float _z1, const float _x2, const float _y2, const float _z2 )
		: x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2),	z2(_z2) {}

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return &x1; }

	SEGAN_INLINE AABox& Set( const float3& _min, const float3& _max )
	{
		min = _min;
		max = _max;
		return *this;
	}

	//! resize the box to the zero
	SEGAN_INLINE AABox& Zero( void )
	{
		x1 = 0; y1 = 0; z1 = 0;
		x2 = 0; y2 = 0; z2 = 0;
		return *this;
	}

	//! resize the current box to cover the entry box
	SEGAN_INLINE AABox& Cover( const AABox& box )
	{	   
		if ( x1 > box.x1 ) x1 = box.x1;
		if ( y1 > box.y1 ) y1 = box.y1;
		if ( z1 > box.z1 ) z1 = box.z1;
		if ( x2 < box.x2 ) x2 = box.x2;
		if ( y2 < box.y2 ) y2 = box.y2;
		if ( z2 < box.z2 ) z2 = box.z2;
		return *this;
	}

public:

	union{
		struct {
			float x1;
			float y1;
			float z1;
			float x2;
			float y2;
			float z2;
		};

		struct {
			float3 min;
			float3 max;
		};

		struct {
			float3 bounds[2];
		};
	};
};


//////////////////////////////////////////////////////////////////////////
//	ORIENTED BOX
//////////////////////////////////////////////////////////////////////////
class OBBox
{
public:
	SEGAN_INLINE OBBox() { world.Identity(); }
	SEGAN_INLINE OBBox( const OBBox& box ) { *this = box; }

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return &aabox.x1; }

	SEGAN_INLINE OBBox& Set( const float3& min, const float3& max, const matrix& matworld )
	{
		// 		v[0] = max;
		// 		v[1].Set( min.x, max.y, max.z );
		// 		v[2].Set( min.x, min.y, max.z );
		// 		v[3].Set( max.x, min.y, max.z );
		// 		v[4].Set( max.x, min.y, min.z );
		// 		v[5].Set( max.x, max.y, min.z );
		// 		v[6].Set( min.x, max.y, min.z );
		// 		v[7] = min;
		world = matworld;
		aabox.Set( min, max );
		return *this;
	}

	SEGAN_INLINE OBBox& Set( const AABox& box, const matrix& matworld )
	{
		world = matworld;
		aabox = box;
		return *this;
	}

	SEGAN_INLINE OBBox& SetAABox( const AABox& box )
	{
		aabox.Set( box.min, box.max );
		return *this;
	}

public:
	AABox	aabox;
	matrix	world;	//	transformation matrix
};

//////////////////////////////////////////////////////////////////////////
//	RAY
//////////////////////////////////////////////////////////////////////////
class Ray
{
public:
	SEGAN_INLINE Ray(){}
	SEGAN_INLINE Ray( const float3& _pos, const float3& _dir ){ Set( _pos, _dir ); };

	//! set new value for this ray
	SEGAN_INLINE void Set( const float3& _pos, const float3& _dir )
	{
		pos = _pos;
		dir = _dir;
		dirInv.Set( 1 / _dir.x, 1 / _dir.y, 1 / _dir.z );
		sign[0] = ( dirInv.x < 0 );
		sign[1] = ( dirInv.y < 0 );
		sign[2] = ( dirInv.z < 0 );
	}

public:

	float3	pos;		//  origin of the ray
	float3	dir;		//  direction of the ray
	float3	dirInv;		//	inverse of direction to use Smits’ method
	sint	sign[3];	//	use of http://cag.csail.mit.edu/~amy/papers/box-jgt.pdf

};

#include "Math_tools.h"

#endif	//	GUARD_Math_HEADER_FILE