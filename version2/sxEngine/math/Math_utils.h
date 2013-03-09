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


//#define _XM_NO_INTRINSICS_
//#include "D:\sajad\Engines\API\_DirectX\Include\xnamath.h"
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
	SEGAN_INLINE Frustum( const float* f ) { memcpy( p, f, sizeof(Frustum) ); }
	SEGAN_INLINE Frustum( const Frustum& f ) { memcpy( p, f.p, sizeof(Frustum) ); }
	SEGAN_INLINE Frustum( const float* _p0, const float* _p1, const float* _p2, const float* _p3, const float* _p4, const float* _p5 )
	{
		memcpy( &p0, _p0, sizeof(Plane) );
		memcpy( &p1, _p1, sizeof(Plane) );
		memcpy( &p2, _p2, sizeof(Plane) );
		memcpy( &p3, _p3, sizeof(Plane) );
		memcpy( &p4, _p4, sizeof(Plane) );
		memcpy( &p5, _p5, sizeof(Plane) );
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
	SEGAN_INLINE Sphere( const float* s ): x(s[0]), y(s[1]), z(s[2]), r(s[3]) {}
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
SEGAN_ALIGN_16 class AABox
{
public:
	SEGAN_INLINE AABox() {}
	SEGAN_INLINE AABox( const AABox& a ): min(a.min), max(a.max) {}
	SEGAN_INLINE AABox( const float3& _min, const float3& _max ): min(_min), max(_max) {}
	SEGAN_INLINE AABox( const float* a ): x1(a[0]), y1(a[1]), z1(a[2]), x2(a[3]), y2(a[4]), z2(a[5]) {}
	SEGAN_INLINE AABox( const float _x1, const float _y1, const float _z1, const float _x2, const float _y2, const float _z2 )
		: x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2),	z2(_z2) {}

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
	};
};


//////////////////////////////////////////////////////////////////////////
//	ORIENTED BOX
//////////////////////////////////////////////////////////////////////////
SEGAN_ALIGN_16 class OBBox
{
public:
	SEGAN_INLINE OBBox() {}
	SEGAN_INLINE OBBox( const float* p)
	{
		sx_mem_copy( v, p, sizeof(OBBox) );
	}
	SEGAN_INLINE OBBox( const OBBox& box )
	{
		memcpy( v, &box, sizeof(OBBox) );
	}
	SEGAN_INLINE OBBox( const AABox& box )
	{
		SetAABox( box );
	}

	SEGAN_INLINE OBBox& Set( const float3& min, const float3& max )
	{
		v[0] = max;
		v[1].Set( min.x, max.y, max.z );
		v[2].Set( min.x, min.y, max.z );
		v[3].Set( max.x, min.y, max.z );
		v[4].Set( max.x, min.y, min.z );
		v[5].Set( max.x, max.y, min.z );
		v[6].Set( min.x, max.y, min.z );
		v[7] = min;
		return *this;
	}

	SEGAN_INLINE OBBox& SetAABox( const AABox& box )
	{
		Set( box.min, box.max );
		return *this;
	}

public:

	float3 v[8];

};

#if 0



//////////////////////////////////////////////////////////////////////////
//	RAY
//////////////////////////////////////////////////////////////////////////
struct SEGAN_ENG_API Ray
{
	float3 pos;			//  origin of the ray
	float3 dir;			//  direction of the ray

	Ray(const float3& _pos, const float3& _dir);
	Ray(float mouse_abs_x, float mouse_abs_y, float vpWidth, float vpHeight, Matrix& matView, Matrix& matProj);

	//! set new value for this ray
	void Set(const float3& _pos, const float3& _dir);

	//! compute ray
	void Compute(float mouse_abs_x, float mouse_abs_y, float vpWidth, float vpHeight, Matrix& matView, Matrix& matProj);

	/*! 
	return true if this ray intersect with plan and fill out outPoint if outPonit be exist and 
	fill out outNormal with normal vector of intersection if outNormal be exist
	*/
	bool Intersect_Plane(const Plane& plane, pfloat3 outPoint = NULL, pfloat3 outNormal = NULL);

	/*! 
	return true if this ray intersect with Rectangle and fill out outPoint if outPonit be exist and 
	fill out outNormal with normal vector of intersection if outNormal be exist
	*/
	bool Intersect_Rect3D(const Rect3D& rect3d, pfloat3 outPoint = NULL, pfloat3 outNormal = NULL);

	/*! 
	return true if this ray intersect with Sphere and fill out outPoint if outPonit be exist and 
	fill out outNormal with normal vector of intersection if outNormal be exist
	NOTE: return true if ray be inside of the shape with outPoint=Ray.pos and outNormal=-Ray.dir
	*/
	bool Intersect_Sphere(const Sphere& sphere, pfloat3 outPoint = NULL, pfloat3 outNormal = NULL);

	/*!
	return true if this ray intersect with AABox and fill out outPoint if outPonit be exist and 
	fill out outNormal with normal vector of intersection if outNormal be exist
	NOTE: return true if ray be inside of the shape with outPoint=Ray.pos and outNormal=-Ray.dir
	*/
	bool Intersect_AABox(const AABox& box, pfloat3 outPoint = NULL, pfloat3 outNormal = NULL);

	/*! 
	return true if this ray intersect with OBBox and fill out outPoint if outPonit be exist and 
	fill out outNormal with normal vector of intersection if outNormal be exist
	NOTE: return true if ray be inside of the shape with outPoint=Ray.pos and outNormal=-Ray.dir
	*/
	bool Intersect_OBBox(const OBBox& box, pfloat3 outPoint = NULL, pfloat3 outNormal = NULL);

	/*! 
	return true if this ray intersect with Triangle and fill out outPoint if outPonit be exist and 
	fill out outNormal with normal vector of intersection if outNormal be exist
	*/
	bool Intersect_Triangle(const float3& v0, const float3& v1, const float3& v2, pfloat3 outPoint = NULL, pfloat3 outNormal = NULL);
};
typedef Ray *PRay;


#endif

#endif	//	GUARD_Math_utils_HEADER_FILE

