/********************************************************************
	created:	2012/05/01
	filename: 	math.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain all math functions and classes
*********************************************************************/
#ifndef GUARD_math_HEADER_FILE
#define GUARD_math_HEADER_FILE

#include "../../sxLib/Lib.h"


//! initialize math system to choose instruction sets. pass true to force using generic math library
SEGAN_ENG_API void sx_math_initialize( bool useGenericMath = false );

//! finalize math system
SEGAN_ENG_API void sx_math_finalize( void );


//////////////////////////////////////////////////////////////////////////
//	standard matrix 4x4
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API matrix
{
public:
	matrix() {};
	matrix( const float * p );
	matrix( const matrix& m );
	matrix( 
		float f11, float f12, float f13, float f14,
		float f21, float f22, float f23, float f24,
		float f31, float f32, float f33, float f34,
		float f41, float f42, float f43, float f44
		);

	//! zero all elements of this matrix
	void Empty( void );

	//! make this matrix identity
	void Identity( void );

	//! compare the matrix and return true if all elements are the same
	bool EqualTo( const float* m );

 	//! calculate the determinant of matrix
 	float Determinant( void );
 
 	//! calculate the transpose of matrix m into this. this = transpose(m)
 	void Transpose( const float* m );
 
 	//! calculate the inverse of matrix m to this. this = inverse(m)
 	void Inverse( const float* m );

	//! add two matrix and put the result to this. this = m1 + m2
	void Add( const float* m1, const float* m2 );

	//! subtract two matrix and put the result to this. this = m1 - m2
	void Subtract( const float* m1, const float* m2 );

	//! multiply two matrix and put the result to this. this = m1 * m2
	void Multiply( const float* m1, const float* m2 );

	//! divide two matrix and put the result to this. this = m1 / m2
	void Divide( const float* m1, const float* m2 );
 
	//! make this as a rotation matrix in pitch/yaw/roll system
	void SetRotationPitchYawRoll( const float pitch, const float yaw, const float roll );

	//! get rotation params in pitch/yaw/roll system
	void GetRotationPitchYawRoll( float& OUT pitch, float& OUT yaw, float& OUT roll );
 
	//! make this as a direction matrix
	void SetDirection( const float* dir, const float* up );

	//! get direction of this matrix. dir and/or up vector can be null
	void GetDirection( float* OUT dir, float* OUT up = null ) const; 

 	//! set translation of this matrix without change of rotation
 	void SetTranslation( const float x, const float y, const float z );

	//! transform src vector by this matrix to the dest vector
	void TransformNormal( float* OUT dest, const float* src ) const;

	//! transform src point by this matrix to the dest point
	void TransformPoint( float* OUT dest, const float* src ) const;
 
 	//! make this to an scaling matrix. Use this with care. some objects in 3d space have no absolute scale. like rigid bodies, triggers, etc
 	void Scale( const float x, const float y, const float z );
 
 	//! Build a look at matrix
 	void LookAt( const float* eye, const float* at, const float* up );
 
 	//! Build a perspective projection matrix
 	void PerspectiveFov( float fov, float aspect, float nearZ, float farZ );
 
 	//! Build an orthographic projection matrix
 	void Orthographic( float width, float height, float nearZ, float farZ );

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return &m00; }

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
//	VECTOR 2D
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API float2
{
public:
	float2() {};
	float2( const float* p );
	float2( const float2& v );
	float2( const float x, const float y );

	//! set new value for this vector
	void  Set( const float x, const float y );

	//! normalize the vector v to this. this = normalize(v)
	void  Normalize( const float* v );

	//! return the length of vector
	float Length( void ) const;

	//! return distance between this and v
	float Distance( const float* v ) const;

	//! return the value of dot product of this and v. (this.v)
	float Dot( const float* v ) const;

	//! return the angle between this and v
	float Angle( const float* v ) const;

	//! return interpolated vector of v1 and v2 by weight of w to this
	void  Lerp( const float* v1, const float* v2, const float w );

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

	// assignment operators
	SEGAN_INLINE float2& operator += ( const float2& v ) { x += v.x; y += v.y; return *this; }
	SEGAN_INLINE float2& operator -= ( const float2& v ) { x -= v.x; y -= v.y; return *this; }
	SEGAN_INLINE float2& operator *= ( const float f )	 { x *= f; y *= f; return *this; }
	SEGAN_INLINE float2& operator /= ( const float f )	 { x /= f; y /= f; return *this; }

	// unary operators
	SEGAN_INLINE float2 operator - () const	{ return float2( -x, -y ); }

	// binary operators
	SEGAN_INLINE float2 operator + ( const float2& v ) const	{ return float2( x + v.x, y + v.y ); }
	SEGAN_INLINE float2 operator - ( const float2& v ) const	{ return float2( x - v.x, y - v.y ); }
	SEGAN_INLINE float2 operator * ( const float f ) const		{ return float2( x * f, y * f ); }
	SEGAN_INLINE float2 operator / ( const float f ) const		{ return float2( x / f, y / f ); }
	SEGAN_INLINE bool operator == ( const float2& v ) const		{ return ( x == v.x ) && ( y == v.y ); }
	SEGAN_INLINE bool operator != ( const float2& v ) const		{ return ( x != v.x ) || ( y != v.y ); }

public:

	union {
		struct {
			float x;
			float y;
		};

		float e[2];	//	element vector
	};
};


//////////////////////////////////////////////////////////////////////////
//	VECTOR 3D
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API float3
{
public:
	float3() {};
	float3( const float* p );
	float3( const float3& v );
	float3( const float x, const float y, const float z );

	//! set new value for this vector
	void Set( const float x, const float y, const float z );

	//! normalize the vector v to this. this = normalize(v)
	void Normalize( const float* v );

	//! return the length of vector
	float Length( void ) const;

	//! return distance between this and v
	float Distance( const float* v ) const;

	//! return square of distance between this and v
	float DistanceSqr( const float* v ) const;

	//! return the value of dot product of this and v. (this.v)
	float Dot( const float* v ) const;

	//! return the vector of cross product of v1 and v2 to this. this = (v2 x v2)
	void Cross( const float* v1, const float* v2 );

	//! return interpolated vector of v1 and v2 by weight of w to this
	void Lerp( const float* v1, const float* v2, const float w );

	//! transform vector v to this by matrix m WITH interference of translation
	void TransformPoint( const float* v, const float* matrix );

	//! transform vector v to this by matrix m WITHOUT interference of translation
	void TransformNorm( const float* v, const float* matrix );

	//! project vector to the screen space. view port must contain x, y, width, height
	void ProjectToScreen( const float* v, const float* worldViewProjection, const int* viewport );

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

	// assignment operators
	SEGAN_INLINE float3& operator += ( const float3& v ) { x += v.x; y += v.y; z += v.z; return *this; }
	SEGAN_INLINE float3& operator -= ( const float3& v ) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	SEGAN_INLINE float3& operator *= ( const float f )	 { x *= f; y *= f; z *= f; return *this; }
	SEGAN_INLINE float3& operator /= ( const float f )	 { x /= f; y /= f; z /= f; return *this; }

	// unary operators
	SEGAN_INLINE float3 operator - () const	{ return float3( -x, -y, -z ); }

	// binary operators
	SEGAN_INLINE float3 operator + ( const float3& v ) const	{ return float3( x + v.x, y + v.y, z + v.z ); }
	SEGAN_INLINE float3 operator - ( const float3& v ) const	{ return float3( x - v.x, y - v.y, z + v.z ); }
	SEGAN_INLINE float3 operator * ( const float f ) const		{ return float3( x * f, y * f, z * f ); }
	SEGAN_INLINE float3 operator / ( const float f ) const		{ return float3( x / f, y / f, z / f ); }
	SEGAN_INLINE bool operator == ( const float3& v ) const		{ return ( x == v.x ) && ( y == v.y ) && ( z == v.z ); }
	SEGAN_INLINE bool operator != ( const float3& v ) const		{ return ( x != v.x ) || ( y != v.y ) || ( z != v.z ); }

public:

	union {
		struct {
			float x;
			float y;
			float z;
		};

		float e[3];	//	element vector
	};
};

//////////////////////////////////////////////////////////////////////////
//	VECTOR 4D
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API float4
{
public:
	float4() {};
	float4( const float* p );
	float4( const float4& v );
	float4( const float* xyz, const float w );
	float4( const float x, const float y, const float z, const float w );

	//! set new value for this vector
	void Set( const float x, const float y, const float z, const float w );

	//! normalize the vector v to this. this = normalize(v)
	void Normalize( const float* v );

	//! return the length of vector
	float Length( void ) const;

	//! Length squared, or "norm"
	float LengthSqar( void ) const;

	//! dot product tow quaternions and return to this
	float Dot( const float* v ) const;

	//! return interpolated vector of v1 and v2 by weight of w to this
	void Lerp( const float* v1, const float* v2, const float w );

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

	// assignment operators
	SEGAN_INLINE float4& operator += ( const float4& v ) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	SEGAN_INLINE float4& operator -= ( const float4& v ) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	SEGAN_INLINE float4& operator *= ( const float f )	 { x *= f; y *= f; z *= f; w *= f; return *this; }
	SEGAN_INLINE float4& operator /= ( const float f )	 { x /= f; y /= f; z /= f; w /= f; return *this; }

	// unary operators
	SEGAN_INLINE float4 operator - () const	{ return float4( -x, -y, -z, -w ); }

	// binary operators
	SEGAN_INLINE float4 operator + ( const float4& v ) const	{ return float4( x + v.x, y + v.y, z + v.z, w + v.w ); }
	SEGAN_INLINE float4 operator - ( const float4& v ) const	{ return float4( x - v.x, y - v.y, z + v.z, w - v.w ); }
	SEGAN_INLINE float4 operator * ( const float f ) const		{ return float4( x * f, y * f, z * f, w * f ); }
	SEGAN_INLINE float4 operator / ( const float f ) const		{ return float4( x / f, y / f, z / f, w / f ); }
	SEGAN_INLINE bool operator == ( const float4& v ) const		{ return ( x == v.x ) && ( y == v.y ) && ( z == v.z ) && ( w == v.w ); }
	SEGAN_INLINE bool operator != ( const float4& v ) const		{ return ( x != v.x ) || ( y != v.y ) || ( z != v.z ) || ( w == v.w ); }

public:

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};

		float e[4];	//	element vector
	};
};


//////////////////////////////////////////////////////////////////////////
//	VECTOR 4D
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API quat
{
public:
	quat() {};
	quat( const float* p );
	quat( const quat& v );
	quat( const float* xyz, const float w );
	quat( const float x, const float y, const float z, const float w );

	SEGAN_INLINE operator const float* ( void ) const { return e; }

	//! set new value for this quaternion
	void Set( const float x, const float y, const float z, const float w );

	//! normalize the quaternion q to this. this = normalize(v)
	void Normalize( const float* v );

	//! return the length of quaternion
	float Length( void ) const;

	//! Length squared, or "norm"
	float LengthSqar( void ) const;

	//! dot product tow quaternions and return to this
	float Dot( const float* v ) const;

	//! return interpolated quaternion of q1 and q2 by weight of w to this
	void Lerp( const float* v1, const float* v2, const float w );

	//! return spherical interpolated quaternion of q1 and q2 by weight of w to this
	void SLerp( const float* v1, const float* v2, const float w );

	//! (-x, -y, -z, w)
	void Conjugate( const float* q );

	//! build this quaternion from a rotation matrix
	void SetRotationMatrix( const float* _matrix );

	//! get rotation matrix from this quaternion
	void GetRotationMatrix( float* OUT _matrix );

	//! build this quaternion from Yaw around the Y axis, Pitch around the X axis, Roll around the Z axis.
	void RotationPitchYawRoll( const float yaw, const float pitch, const float roll );

	//! quaternion multiplication. this will represents the rotation q2 followed by the rotation q1.  (this = q2 * q1)
	void Multiply( const float* q1, const float* q2 );

	//! add two quaternion and put the result to this. this = q1 + q2
	void Add( const float* q1, const float* q2 );

	// conjugate and re-norm
	void Inverse( const float* q );

public:

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};

		float e[4];	//	element vector
	};
};

//#define _XM_NO_INTRINSICS_
//#include "D:\sajad\Engines\API\_DirectX\Include\xnamath.h"
//////////////////////////////////////////////////////////////////////////
//	PLANE
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API Plane
{
public:
	Plane() {};
	Plane( const float* p );
	Plane( const Plane& p );
	Plane( const float a, const float b, const float c, const float d );

	//! set new value for this plane
	void Set( const float a, const float b, const float c, const float d );

	//! construct a plane from 3 points in space
	void MakeFromPoints( const float* p1, const float* p2, const float* p3 );

	//! construct a plane from a point and a normal
	void MakeFromNormal( const float* p, const float* n );

	//! return distance of a point from this plane
	float Distance( const float* p ) const;

	//! normalize plane p to this
	void Normalize( const float* p );

	//! make this plane by transform plane p by matrix m
	void Transform( const float* p, const float* m );

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return e; }

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
class SEGAN_ENG_API Frustum
{
public:
	Frustum() {};
	Frustum( const float* p );
	Frustum( const Frustum& p );
	Frustum(
		const float* p0, const float* p1, const float* p2,
		const float* p3, const float* p4, const float* p5
		);

	//! normalize frustum f to this
	void Normalize( const float* _frustum );

	//! compute this frustum from given matrix
	void ComputeByMatrix( const float* _matrix );

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return p0.e; }

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
class SEGAN_ENG_API Sphere
{
public:
	Sphere() {};
	Sphere( const float* s );
	Sphere( const Sphere& s );
	Sphere( const float* cen, const float rad );
	Sphere( const float x, const float y, const float z, const float r );

	//! resize the sphere to the zero
	void Zero( void );

	//! set new properties
	void Set( const float* cen, const float rad );

	//! compute center and radius depend of the box
	void ComputeByAABox( const float* box );

	//! resize the current sphere to cover the entry sphere
	void Cover( const float* sphere );

	//! return true if the sphere intersect to this and also fill the distance parameter 
	bool Intersect( const float* sphere, float* OUT distance ) const;

	//! transform sphere to this by matrix m
	void Transform( const float* sphere, const float* mat );

	//! conventional operators
	SEGAN_INLINE operator const float* ( void ) const { return &x; }

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
class SEGAN_ENG_API AABox
{
public:
	AABox() {};
	AABox( const float* a );
	AABox( const AABox& a );
	AABox( const float* min, const float* max );
	AABox( const float x1, const float y1, const float z1, const float x2, const float y2, const float z2 );

	//! resize the box to the zero
	void Zero( void );

	//! resize the current box to cover the entry box
	void CoverAA( const float* aabox );

	//! resize the current box to cover the entry box
	void CoverOB( const float* obbox );

	//! return volume of the box
	float GetVolume( void ) const;

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

#if 0

//////////////////////////////////////////////////////////////////////////
//	3D RECTANGLE
//////////////////////////////////////////////////////////////////////////
struct SEGAN_ENG_API Rect3D
{
	union{
		struct{
			float3 v0;
			float3 v1;
			float3 v2;
			float3 v3;
		};

		struct{
			float3 v[4];
		};
	};


	Rect3D(void);
	Rect3D(const float3& _v0, const float3& _v1, const float3& _v2, const float3& _v3);

	//! setup 3d rectangle
	void Set(const float3& _v0, const float3& _v1, const float3 _v2, const float3 _v3);

	//! transform rect3d to this by matrix m
	void  Transform(const Rect3D& rect3d, const Matrix& m);
};

//////////////////////////////////////////////////////////////////////////
//	ORIENTED BOX
//////////////////////////////////////////////////////////////////////////
struct SEGAN_ENG_API OBBox
{
	Vector v[8];

	OBBox(void);
	OBBox(const AABox& aaBox);
	OBBox(const float3& _min, const float3& _max);

	//! transform AABox to this by matrix m
	void  Transform(const AABox& aaBox, const Matrix& m);

	//! return true if box intersect with frustum
	bool IntersectFrustum(Frustum& fr) const;

};
typedef OBBox *POBBox;


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

//////////////////////////////////////////////////////////////////////////
//	float RECTANGLE
//////////////////////////////////////////////////////////////////////////
typedef struct RectF
{
	float x1, y1, x2, y2;

	RectF(float _x1, float _y1, float _x2, float _y2): x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
} *PRectF;

//////////////////////////////////////////////////////////////////////////
//	INTEGER POINT
//////////////////////////////////////////////////////////////////////////
typedef struct PointI
{
	int x, y;

	PointI(int _x, int _y): x(_x), y(_y) {}

	bool operator== (PointI& p) {return x==p.x && y==p.y;}
	bool operator!= (PointI& p) {return x!=p.x || y!=p.y;}
} *PPointI, int2, *pint2;

#endif



#endif	//	GUARD_math_HEADER_FILE