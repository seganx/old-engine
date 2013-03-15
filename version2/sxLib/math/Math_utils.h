/********************************************************************
	created:	2013/03/07
	filename: 	Math_utils.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some math object designed on
				top of math vectors
*********************************************************************/
#ifndef GUARD_Math_utils_HEADER_FILE
#define GUARD_Math_utils_HEADER_FILE


//////////////////////////////////////////////////////////////////////////
//	COLOR
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class Color
{
public:
	SEGAN_INLINE Color() {}
	SEGAN_INLINE Color( const dword c ) { Set( c ); }
	SEGAN_INLINE Color( const Color& c ): r(c.r), g(c.g), b(c.b), a(c.a) {}
	SEGAN_INLINE Color( const float* p ): r(p[0]), g(p[1]), b(p[2]), a(p[3]) {}
	SEGAN_INLINE Color( const float _r, const float _g, const float _b, const float _a ): r(_r), g(_g), b(_b), a(_a) {}

	// assignment operators
	SEGAN_INLINE Color& operator += ( const Color& c )	{ r += c.r; g += c.g; b += c.b; a += c.a; return *this; }
	SEGAN_INLINE Color& operator -= ( const Color& c )	{ r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
	SEGAN_INLINE Color& operator *= ( const float f )	{ r *= f; g *= f; b *= f; a *= f; return *this; }
	SEGAN_INLINE Color& operator /= ( const float f )	{ r /= f; g /= f; b /= f; a /= f; return *this; }

	// binary operators
	SEGAN_INLINE Color operator + ( const Color& c ) const	{ return Color( r + c.r, g + c.g, b + c.b, a + c.a ); }
	SEGAN_INLINE Color operator - ( const Color& c ) const	{ return Color( r - c.r, g - c.g, b + c.b, a - c.a ); }
	SEGAN_INLINE Color operator * ( const float f ) const	{ return Color( r * f, g * f, b * f, a * f ); }
	SEGAN_INLINE Color operator / ( const float f ) const	{ return Color( r / f, g / f, b / f, a / f ); }
	SEGAN_INLINE bool operator == ( const Color& c ) const	{ return ( r == c.r ) && ( g == c.g ) && ( b == c.b ) && ( a == c.a ); }
	SEGAN_INLINE bool operator != ( const Color& c ) const	{ return ( r != c.r ) || ( g != c.g ) || ( b != c.b ) || ( a == c.a ); }

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }
	SEGAN_INLINE operator const dword ( void ) const
	{
		dword cr = r >= 1.0f ? 0xff : ( r <= 0.0f ? 0x00 : dword( r * 255.0f + 0.5f ) );
		dword cg = g >= 1.0f ? 0xff : ( g <= 0.0f ? 0x00 : dword( g * 255.0f + 0.5f ) );
		dword cb = b >= 1.0f ? 0xff : ( b <= 0.0f ? 0x00 : dword( b * 255.0f + 0.5f ) );
		dword ca = a >= 1.0f ? 0xff : ( a <= 0.0f ? 0x00 : dword( a * 255.0f + 0.5f ) );
		return ( ca << 24 ) | ( cr << 16 ) | ( cg << 8 ) | cb;
	}

	//! set new value for this color
	SEGAN_INLINE Color& Set( const float _r, const float _g, const float _b, const float _a )
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
		return *this;
	}

	//! set new value for this color
	SEGAN_INLINE Color& Set( const dword c )
	{
		const float f = 1.0f / 255.0f;
		r = f * float( (byte)( c >> 16 ) );
		g = f * float( (byte)( c >>  8 ) );
		b = f * float( (byte)( c >>  0 ) );
		a = f * float( (byte)( c >> 24 ) );
		return *this;
	}

	//! return interpolated vector of c1 and c2 by weight of t to this
	SEGAN_INLINE Color& Lerp( const Color& c1, const Color& c2, const float t )
	{
		x = c1.x + ( c2.x - c1.x ) * t;
		y = c1.y + ( c2.y - c1.y ) * t;
		z = c1.z + ( c2.z - c1.z ) * t;
		w = c1.w + ( c2.w - c1.w ) * t;
		return *this;
	}


public:

	union {
		struct {
			float r;
			float g;
			float b;
			float a;
		};

		struct {
			float x;
			float y;
			float z;
			float w;
		};

		float e[4];	//	elements of color
	};
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


#endif	//	GUARD_Math_utils_HEADER_FILE

