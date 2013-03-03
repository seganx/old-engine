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

	SEGAN_INLINE operator const float* ( void ) const { return &m00; }

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

	SEGAN_INLINE operator const float* ( void ) const { return e; }

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

	SEGAN_INLINE operator const float* ( void ) const { return e; }

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

	SEGAN_INLINE operator const float* ( void ) const { return e; }

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


#if 0

//////////////////////////////////////////////////////////////////////////
//	PLANE
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API Plane : public D3DXPLANE
{
public:
	//! setup a plane from 3 vectors in space
	void Set(const Vector3& a, const Vector3& b, const Vector3 c);

	//! Construct a plane from a point and a normal
	void Make(const Vector3& v, const Vector3& n);

	//! return distance of a vector v from this plane
	float Distance(const Vector3& v) const;

	//! normalize plane p to this
	void Normalize(const Plane& p);
};
typedef Plane *PPlane;

//////////////////////////////////////////////////////////////////////////
//	FRUSTUM
//////////////////////////////////////////////////////////////////////////
struct SEGAN_ENG_API Frustum
{
	union{
		struct{
			Plane p[6];
		};

		struct{
			Plane p0;
			Plane p1;
			Plane p2;
			Plane p3;
			Plane p4;
			Plane p5;
		};
	};

	//! normalize frustum f to this
	void Normalize( const Frustum& f );

	//! compute this frustum from given matrix
	void ComputeByMatrix(Matrix& mat);
};
typedef Frustum *PFrustum;


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
//	AXIS ALIGNED BOX
//////////////////////////////////////////////////////////////////////////
struct OBBox;
struct SEGAN_ENG_API AABox
{
	Vector Min;
	Vector Max;

	AABox(void);
	AABox(const float3& _min, const float3& _max);

	//! resize the box to the zero
	void Zero(void);

	//! resize the current box to cover the entry box
	void CoverAA(const AABox& box);

	//! resize the current box to cover the entry box
	void CoverOB(const OBBox& box);

	//! return volume of the box
	float GetVolume(void) const;

	//!  operator ==
	bool operator == (const AABox& box) const;
};
typedef AABox *PAABox;

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
//	SPHERE
//////////////////////////////////////////////////////////////////////////
struct SEGAN_ENG_API Sphere
{
	union{
		struct
		{
			Vector center;
			float radius;
		};

		struct
		{
			float x;
			float y;
			float z;
			float r;
		};
	};

	Sphere(void);
	Sphere(const float3& cen, const float rad);

	//! set new properties
	void Set(const float3& cen, const float rad);

	//! compute center and radius depend of the box
	void ComputeByAABox(const AABox& box);

	//! resize the sphere to the zero
	void Zero(void);

	//! resize the current sphere to cover the entry sphere
	void Cover(const Sphere& sphere);

	//! return true if the sphere intersect to this and also fill the distance parameter 
	bool Intersect(const Sphere& sphere, float* distance) const;

	//! transform Sphere to this by matrix m
	void Transform(const Sphere& sphere, const Matrix& m);

	//!  operator ==
	bool operator == (const Sphere& sphere) const;
};
typedef Sphere *PSphere;

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
//	FLOAT RECTANGLE
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