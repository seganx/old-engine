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

#include <xmmintrin.h>
#include <emmintrin.h>
#include <assert.h>

#define USE_SSE2_LDDQU
#ifdef USE_SSE2_LDDQU
#include <intrin.h>  //used for _mm_lddqu_si128
#endif //USE_SSE2_LDDQU

//! initialize math system to choose instruction sets. pass true to force using generic math library
SEGAN_API void sx_math_initialize( bool useGenericMath = false );

//! finalize math system
SEGAN_API void sx_math_finalize( void );


//////////////////////////////////////////////////////////////////////////
//	standard matrix 4x4
//////////////////////////////////////////////////////////////////////////
class SEGAN_API Matrix
{
public:
	Matrix() {};
	Matrix( const float * p );
	Matrix( const Matrix& m );
	Matrix( 
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
	bool EqualTo( const Matrix& m );

 	//! calculate the determinant of matrix
 	float Determinant( void );
 
 	//! calculate the transpose of matrix m into this. this = transpose(m)
 	void Transpose( const Matrix& m );
 
 	//! calculate the inverse of matrix m to this. this = inverse(m)
 	void Inverse( const Matrix& m );

	//! add two matrix and put the result to this. this = m1 + m2
	void Add( const Matrix& m1, const Matrix& m2 );

	//! subtract two matrix and put the result to this. this = m1 - m2
	void Subtract( const Matrix& m1, const Matrix& m2 );

	//! multiply two matrix and put the result to this. this = m1 * m2
	void Multiply( const Matrix& m1, const Matrix& m2 );

	//! divide two matrix and put the result to this. this = m1 / m2
	void Divide( const Matrix& m1, const Matrix& m2 );
 
	//! set rotation of this matrix in pitch/yaw/roll system
	void SetRotationPitchYawRoll( const float pitch, const float yaw, const float roll );

	//! get rotation params in pitch/yaw/roll system
	void GetRotationPitchYawRoll( float& OUT pitch, float& OUT yaw, float& OUT roll );
 
 	//! set translation of this matrix without change of rotation
 	void SetTranslation( const float x, const float y, const float z );
 
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
			float	_11, _12, _13, _14;
			float   _21, _22, _23, _24;
			float   _31, _32, _33, _34;
			float   _41, _42, _43, _44;
		};

		float	m[4][4];

		__m128	m128[4];
	};
};
typedef Matrix *PMatrix;


#if 0

//////////////////////////////////////////////////////////////////////////
//	VECTOR 2D
//////////////////////////////////////////////////////////////////////////
class SEGAN_API Vector2 : public D3DXVECTOR2
{
public:
	Vector2( void );
	Vector2( const float * );
	Vector2( const Vector2& v );
	Vector2( float x, float y );

	//! set new value for this vector
	void  Set(float x, float y);

	//! normalize the vector v to this. this = normalize(v)
	void  Normalize(Vector2& v);

	//! return the length of vector
	float Length(void);

	//! return distance between this and v
	float Distance(Vector2& v);

	//! return the value of dot product of this and v. (this.v)
	float Dot(Vector2& v);

	//! return the angle between this and v
	float Angle(Vector2& v);

	//! return interpolated vector of v1 and v2 by weight of w to this
	void  Lerp(Vector2& v1, Vector2& v2, float w);

};
typedef Vector2	*PVector2, float2, *pfloat2;

//////////////////////////////////////////////////////////////////////////
//	VECTOR 3D
//////////////////////////////////////////////////////////////////////////
class SEGAN_API Vector3 : public D3DXVECTOR3
{
public:
	Vector3( void );
	Vector3( const float * );
	Vector3( const D3DVECTOR& v );
	Vector3( float x, float y, float z );

	//! set new value for this vector
	void Set(float x, float y, float z);

	//! normalize the vector v to this. this = normalize(v)
	void  Normalize(const Vector3& v);

	//! return the length of vector
	float Length(void);

	//! return distance between this and v
	float Distance(const Vector3& v) const;

	//! return square of distance between this and v
	float Distance_sqr(const Vector3& v) const;

	//! return the value of dot product of this and v. (this.v)
	float Dot(const Vector3& v);

	//! return the vector of cross product of v1 and v2 to this. this = (v2 x v2)
	void Cross(const Vector3& v1, const Vector3& v2);

	//! return interpolated vector of v1 and v2 by weight of w to this
	void  Lerp(Vector3& v1, Vector3& v2, float w);

	//! transform vector v to this by matrix m WITH interference of translation
	void  Transform(const Vector3& v, const Matrix& m);

	//! transform vector v to this by matrix m WITHOUT interference of translation
	void  Transform_Norm(const Vector3& vIn, const Matrix& m);

	//! project vector to the screen space
	void  ProjectToScreen(const Vector3& vIn, const Matrix& matWorld, const Matrix& matView, const Matrix& matProj, const D3DVIEWPORT9& vp);
};
typedef Vector3 *PVector3, Vector, *PVector, float3, *pfloat3;

//////////////////////////////////////////////////////////////////////////
//	VECTOR 4D
//////////////////////////////////////////////////////////////////////////
class SEGAN_API Vector4 : public D3DXVECTOR4
{
public:
	Vector4();
	Vector4( const float* );
	Vector4( const D3DVECTOR& xyz, float w );
	Vector4( float x, float y, float z, float w );

	//! set new value for this vector
	void Set(float x, float y, float z, float w);

	//! normalize the vector v to this. this = normalize(v)
	void Normalize(const Vector4& v);

	//! return the length of vector
	float Length(void);

	//! Length squared, or "norm"
	float LengthSq(void);

	//! dot product tow quaternions and return to this
	float Dot(const Vector4 v);

	//! (-x, -y, -z, w)
	void Conjugate(const Vector4 q);

	//! return interpolated vector of v1 and v2 by weight of w to this
	void Lerp(const Vector4& v1, const Vector4& v2, const float w);

	//! return spherical interpolated quaternion of q1 and q2 by weight of w to this
	void SLerp(const Vector4& q1, const Vector4 q2, const float w);

	//! build this quaternion from a rotation matrix
	void RotationMatrix(const Matrix& mat);

	//! get rotation matrix from this quaternion
	void GetRotationMatrix(OUT Matrix& mat);

	//! build this quaternion about arbitrary axis.
	void RotationAxis(const float3 v, const float a);

	//! compute a quaternin's axis and angle of rotation. Expects unit quaternions.
	void GetAxisAngle(OUT float3& oVec, OUT float& oAng);

	//! build this quaternion from Yaw around the Y axis, Pitch around the X axis, Roll around the Z axis.
	void RotationYawPitchRoll(const float Yaw, const float Pitch, const float Roll);

	//! quaternion multiplication. this will represents the rotation q2 followed by the rotation q1.  (this = q2 * q1)
	void Multiply(const Vector4& q1, const Vector4 q2);

	// conjugate and re-norm
	void Inverse(const Vector4 q);

	//! transform vector v to this by matrix m
	void  Transform(const Vector4& vIn, const Matrix& m);

	void operator =(const D3DXCOLOR colorRGBA);
};
typedef Vector4 *PVector4, float4, *pfloat4;

//////////////////////////////////////////////////////////////////////////
//	QUATERNION
//////////////////////////////////////////////////////////////////////////
class SEGAN_API Quaternion : public D3DXQUATERNION
{
public:
	Quaternion();
	Quaternion( const float* );
	Quaternion( const D3DVECTOR& xyz, float w );
	Quaternion( float x, float y, float z, float w );

	//! set new value for this quaternion
	void Set(float x, float y, float z, float w);

	//! normalize the quaternion q to this. this = normalize(q)
	void Normalize(const Quaternion& q);

	//! return the length of quaternion
	float Length(void);

	//! Length squared, or "norm"
	float LengthSq(void);

	//! dot product tow quaternions and return to this
	float Dot(const Quaternion& q);

	//! (-x, -y, -z, w)
	void Conjugate(const Quaternion& q);

	//! return interpolated quaternion of q1 and q2 by weight of w to this
	void Lerp(const Quaternion& q1, const Quaternion& q2, const float w);

	//! return spherical interpolated quaternion of q1 and q2 by weight of w to this
	void SLerp(const Quaternion& q1, const Quaternion q2, const float w);

	//! build this quaternion from a rotation matrix
	void SetRotationMatrix(const Matrix& mat);

	//! get rotation matrix from this quaternion
	void GetRotationMatrix(OUT Matrix& mat) const;

	//! build this quaternion about X, Y and Z angle
	void SetRotationXYZ(const float rx, const float ry, const float rz);

	//! compute X, Y and Z angles from this quaternion
	void GetRotationXYZ(OUT float& rx, OUT float& ry, OUT float& rz) const;

	//! build this quaternion about arbitrary axis
	void SetRotationAxis(const float3& v, const float a);

	//! compute a quaternin's axis and angle of rotation. Expects unit quaternions
	void GetRotationAxis(OUT float3& oVec, OUT float& oAng) const;

	//! build this quaternion from Yaw around the Y axis, Pitch around the X axis, Roll around the Z axis
	void SetRotationPYR(const float Pitch, const float Yaw, const float Roll);

	////! compute Yaw around the Y axis, Pitch around the X axis and Roll around the Z axis from this quaternion
	//void GetRotationYPR(float& Yaw, float& Pitch, float& Roll) const;

	////! build this quaternion from spherical rotation angles
	//void RotationSpherical(const float angle, const float latitude, const float longitude);

	////! compute spherical rotation angles from this quaternion
	//void GetSpherical(OUT float& angle, OUT float& latitude, OUT float& longitude) const;

	//! quaternion multiplication. this will represents the rotation q2 followed by the rotation q1  (this = q2 * q1)
	void Multiply(const Quaternion& q1, const Quaternion& q2);

	// conjugate and re-norm
	void Inverse(const Quaternion& q);

	//! transform quaternion q to this by matrix m
	void  Transform(const Quaternion& q, const Matrix& m);
};
typedef Quaternion *PQuaternion, floatQ, *pfloatQ;

//////////////////////////////////////////////////////////////////////////
//	PLANE
//////////////////////////////////////////////////////////////////////////
class SEGAN_API Plane : public D3DXPLANE
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
struct SEGAN_API Frustum
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
struct SEGAN_API Rect3D
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
struct SEGAN_API AABox
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
struct SEGAN_API OBBox
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
struct SEGAN_API Sphere
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
struct SEGAN_API Ray
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