#include "sxMath.h"

#define DBL_PI	3.14159265358979

FORCEINLINE double ArcTan2(const double Y, const double X)
{
	_asm
	{
		FLD     Y;
		FLD     X;
		FPATAN;
		FWAIT;
	}
}


FORCEINLINE double ArcSin(const double X)
{
	return ArcTan2(X, sqrt(1 - X * X));
}


namespace sx { namespace math {

//////////////////////////////////////////////////////////////////////////
//	MATRIX
//////////////////////////////////////////////////////////////////////////
Matrix::Matrix(): D3DXMATRIX() {}
Matrix::Matrix( const float * f): D3DXMATRIX(f) {}
Matrix::Matrix( const D3DMATRIX& m): D3DXMATRIX(m) {}
Matrix::Matrix( float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44 )
: D3DXMATRIX(_11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44) {}

FORCEINLINE void Matrix::Empty( void )
{
	int i, j;
	for (i=0; i<4; i++)
	{
		for (j=0; i<4; i++)
		{
			m[i][j] = 0;
		}
	}
}

FORCEINLINE void Matrix::Identity( void )
{
	_11=1; _12=0; _13=0; _14=0;
	_21=0; _22=1; _23=0; _24=0;
	_31=0; _32=0; _33=1; _34=0;
	_41=0; _42=0; _43=0; _44=1;
}

FORCEINLINE bool Matrix::EqualTo( const Matrix& mat )
{
	int i, j;
	for (i=0; i<4; i++)
	{
		for (j=0; i<4; i++)
		{
			if(abs(m[i][j] - mat.m[i][j]) > EPSILON) return false;
		}
	}

	return true;
}

FORCEINLINE void Matrix::Add( const Matrix& m1, const Matrix& m2 )
{
	*this = m1 + m2;
}

FORCEINLINE void Matrix::Subtract( const Matrix& m1, const Matrix& m2 )
{
	*this = m1 - m2;
}

FORCEINLINE void Matrix::Multiply( const Matrix& m1, const Matrix& m2 )
{
	D3DXMatrixMultiply(this, &m1, &m2);
}

FORCEINLINE void Matrix::Divide( const Matrix& m1, const Matrix& m2 )
{
	D3DXMatrixInverse(this, NULL, &m2);
	D3DXMatrixMultiply(this, &m1, this);
}

FORCEINLINE float Matrix::Determinant( void )
{
	return D3DXMatrixDeterminant(this);
}

FORCEINLINE void Matrix::Transpose( const Matrix& m )
{
	D3DXMatrixTranspose(this, &m);
}

FORCEINLINE void Matrix::Inverse( const Matrix& m )
{
	D3DXMatrixInverse(this, NULL, &m);
}

FORCEINLINE void Matrix::RotationX( const float Angle )
{
	D3DXMatrixRotationX(this, Angle);
}

FORCEINLINE void Matrix::RotationY( const float Angle )
{
	D3DXMatrixRotationY(this, Angle);
}

FORCEINLINE void Matrix::RotationZ( const float Angle )
{
	D3DXMatrixRotationZ(this, Angle);
}

FORCEINLINE void Matrix::RotationXYZ( const float x, const float y, const float z )
{
	Matrix mZ, mY, mX;
	
	D3DXMatrixRotationZ( &mZ, z );

	D3DXMatrixRotationY( &mX, y );
	
	D3DXMatrixRotationX( &mY, x );

	this->Multiply( mZ, mY );

	this->Multiply( *this, mX );

	//D3DXMatrixRotationYawPitchRoll(this, y, x, z);
}

FORCEINLINE void Matrix::GetRoationXYZ( float& outX, float& outY, float& outZ ) const
{
	outX = (float)asin( - _32 ); 

	float threshold = 0.001f; 

	float test = (float)cos( outX ); 

	if (test > threshold) 
	{ 

		outZ = (float)atan2( _12, _22 ); 

		outY = (float)atan2( _31, _33 ); 

	}
	else 
	{ 

		outZ = (float)atan2( -_21, _11 ); 

		outY = 0.0f; 

	} 
}

FORCEINLINE void Matrix::RotationPYR( const float pitch, const float yaw, const float roll )
{
	D3DXMatrixRotationYawPitchRoll(this, yaw, pitch, roll);
}

FORCEINLINE void Matrix::RotationQ( const float qx, const float qy, const float qz, const float qw )
{
	D3DXQUATERNION q(qx, qy, qz, qw);
	D3DXMatrixRotationQuaternion(this, &q);
}

FORCEINLINE void Matrix::GetRotationQ( float& outQx, float& outQy, float& outQz, float& outQw )
{
	D3DXQUATERNION q; D3DXVECTOR3 scl, trns;
	D3DXMatrixDecompose(&scl, &q, &trns, this);
	outQx = q.x; outQy = q.y; outQz = q.z; outQw = q.w;
}

FORCEINLINE void Matrix::Translation( const float tx, const float ty, const float tz )
{
	D3DXMatrixTranslation(this, tx, ty, tz);
}

FORCEINLINE void Matrix::SetTranslation( const float tx, const float ty, const float tz )
{
	_41=tx; _42=ty; _43=tz;
}

FORCEINLINE void Matrix::TranslateIt( const float tx, const float ty, const float tz )
{
	_41+=tx; _42+=ty; _43+=tz;
}

FORCEINLINE void Matrix::GetTranslation( float& txOut, float& tyOut, float& tzOut )
{
	txOut=_41; tyOut=_42; tzOut=_43;
}

FORCEINLINE void Matrix::Scale( const float sx, const float sy, const float sz )
{
	D3DXMatrixScaling(this, sx, sy, sz);
}

FORCEINLINE Matrix&  Matrix::LookAtLH( const Vector3& Eye, const Vector3& At, const Vector3& Up )
{
	D3DXMatrixLookAtLH(this, &Eye, &At, &Up);
	return *this;
}

FORCEINLINE Matrix&  Matrix::LookAtRH( const Vector3& Eye, const Vector3& At, const Vector3& Up )
{
	D3DXMatrixLookAtRH(this, &Eye, &At, &Up);
	return *this;
}

FORCEINLINE Matrix& Matrix::PerspectiveFovLH( float FOV, float Aspect, float zNear, float zFar )
{
	D3DXMatrixPerspectiveFovLH(this, FOV, Aspect, zNear, zFar);
	return *this;
}

FORCEINLINE Matrix&  Matrix::PerspectiveFovRH( float FOV, float Aspect, float zNear, float zFar )
{
	D3DXMatrixPerspectiveFovRH(this, FOV, Aspect, zNear, zFar);
	return *this;
}

FORCEINLINE Matrix&  Matrix::OrthoLH( float width, float height, float zNear, float zFar )
{
	D3DXMatrixOrthoLH(this, width, height, zNear, zFar);
	return *this;
}

FORCEINLINE Matrix&  Matrix::OrthoRH( float width, float height, float zNear, float zFar )
{
	D3DXMatrixOrthoRH(this, width, height, zNear, zFar);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//	VECTOR 2D
//////////////////////////////////////////////////////////////////////////
Vector2::Vector2(): D3DXVECTOR2() {}
Vector2::Vector2( const float * f): D3DXVECTOR2(f) {}
Vector2::Vector2( const Vector2& v ): D3DXVECTOR2(v.x, v.y) {}
Vector2::Vector2( float x, float y ): D3DXVECTOR2(x, y) {}


FORCEINLINE void Vector2::Set( float _x, float _y )
{
	x = _x;
	y = _y;
}

FORCEINLINE void Vector2::Normalize( Vector2& v )
{
	D3DXVec2Normalize(this, &v);
}

FORCEINLINE float Vector2::Length( void )
{
	return D3DXVec2Length(this);
}

FORCEINLINE float Vector2::Distance( Vector2& v )
{
	float lx = v.x - x;
	float ly = v.y - y;
	return sqrt(lx*lx + ly*ly);
}

FORCEINLINE float Vector2::Dot( Vector2& v )
{
	return D3DXVec2Dot(this, &v);
}

FORCEINLINE float Vector2::Angle( Vector2& v )
{
	return acos( Dot(v)/(Length()*v.Length()) );
}

FORCEINLINE void Vector2::Lerp( Vector2& v1, Vector2& v2, float w )
{
	D3DXVec2Lerp(this, &v1, &v2, w);
}

//////////////////////////////////////////////////////////////////////////
//	VECTOR 3D
//////////////////////////////////////////////////////////////////////////
Vector3::Vector3(): D3DXVECTOR3() {}
Vector3::Vector3( const float * f): D3DXVECTOR3(f) {}
Vector3::Vector3( const D3DVECTOR& v ): D3DXVECTOR3(v) {}
Vector3::Vector3( float x, float y, float z ): D3DXVECTOR3(x, y, z) {}

FORCEINLINE void Vector3::Set( float _x, float _y, float _z )
{
	x = _x;
	y = _y;
	z = _z;
}

FORCEINLINE void Vector3::Normalize( const Vector3& v )
{
	D3DXVec3Normalize(this, &v);
}

FORCEINLINE float Vector3::Length( void )
{
	return D3DXVec3Length(this);
}

FORCEINLINE float Vector3::Distance( const Vector3& v ) const
{
	float lx = v.x - x;
	float ly = v.y - y;
	float lz = v.z - z;
	return sqrt(lx*lx + ly*ly + lz*lz);
}

float Vector3::Distance_sqr( const Vector3& v ) const
{
	float lx = v.x - x;
	float ly = v.y - y;
	float lz = v.z - z;
	return (lx*lx + ly*ly + lz*lz);
}

FORCEINLINE float Vector3::Dot( const Vector3& v )
{
	return D3DXVec3Dot(this, &v);
}

FORCEINLINE void Vector3::Cross( const Vector3& v1, const Vector3& v2 )
{
	x = v1.y * v2.z - v1.z * v2.y;
	y = v1.z * v2.x - v1.x * v2.z;
	z = v1.x * v2.y - v1.y * v2.x;
}

FORCEINLINE void Vector3::Lerp( Vector3& v1, Vector3& v2, float w )
{
	D3DXVec3Lerp(this, &v1, &v2, w);
}

FORCEINLINE void Vector3::Transform( const Vector3& v, const Matrix& m )
{
	float _x = v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
	float _y = v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
	float _z = v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;
	x = _x;
	y = _y;
	z = _z;
}

FORCEINLINE void Vector3::Transform_Norm( const Vector3& v, const Matrix& m )
{
	D3DXVec3TransformNormal(this, &v, &m);
}

FORCEINLINE void Vector3::ProjectToScreen( const float3& vIn, const Matrix& matWorld, const Matrix& matView, const Matrix& matProj, const D3DVIEWPORT9& vp )
{
	D3DXVec3Project(this, &vIn, &vp, &matProj, &matView, &matWorld);
	x = x - vp.Width * 0.5f;
	y = - y + vp.Height * 0.5f;
	z = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
//	VECTOR 4D
//////////////////////////////////////////////////////////////////////////
Vector4::Vector4(): D3DXVECTOR4() {}
Vector4::Vector4( const float* f): D3DXVECTOR4(f) {}
Vector4::Vector4( const D3DVECTOR& xyz, float w ): D3DXVECTOR4(xyz, w) {}
Vector4::Vector4( float x, float y, float z, float w ): D3DXVECTOR4(x, y, z, w) {}

FORCEINLINE void Vector4::Set( float _x, float _y, float _z, float _w )
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

FORCEINLINE void Vector4::Normalize( const Vector4& v )
{
	D3DXVec4Normalize(this, &v);
}

FORCEINLINE float Vector4::Length( void )
{
	return D3DXVec4Length(this);
}

FORCEINLINE float Vector4::LengthSq( void )
{
	return D3DXVec4LengthSq(this);
}

FORCEINLINE float Vector4::Dot( const Vector4 v )
{
	return D3DXVec4Dot(this, &v);
}

FORCEINLINE void Vector4::Lerp( const Vector4& v1, const Vector4& v2, float w )
{
	D3DXVec4Lerp(this, &v1, &v2, w);
}

FORCEINLINE void Vector4::Transform( const Vector4& v, const Matrix& m )
{
	D3DXVec4Transform(this, &v, &m);
}

FORCEINLINE void Vector4::operator=( const D3DXCOLOR colorRGBA )
{
	x = colorRGBA.r;
	y = colorRGBA.g;
	z = colorRGBA.b;
	w = colorRGBA.a;
}


//////////////////////////////////////////////////////////////////////////
//	QUATERNION
//////////////////////////////////////////////////////////////////////////
Quaternion::Quaternion(): D3DXQUATERNION() {}
Quaternion::Quaternion( const float* f): D3DXQUATERNION(f) {}
Quaternion::Quaternion( const D3DVECTOR& xyz, float w ): D3DXQUATERNION(x, y, z, w) {}
Quaternion::Quaternion( float x, float y, float z, float w ): D3DXQUATERNION(x, y, z, w) {}

FORCEINLINE void Quaternion::Set( float _x, float _y, float _z, float _w )
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

FORCEINLINE void Quaternion::Normalize( const Quaternion& q )
{
	D3DXQuaternionNormalize(this, &q);
}

FORCEINLINE float Quaternion::Length( void )
{
	return D3DXQuaternionLength(this);
}

FORCEINLINE float Quaternion::LengthSq( void )
{
	return D3DXQuaternionLengthSq(this);
}

FORCEINLINE float Quaternion::Dot( const Quaternion& q )
{
	return D3DXQuaternionDot(this, &q);
}

FORCEINLINE void Quaternion::Conjugate( const Quaternion& q )
{
	D3DXQuaternionConjugate(this, &q);
}

FORCEINLINE void Quaternion::Lerp( const Quaternion& q1, const Quaternion& q2, const float w )
{
	D3DXVec4Lerp( reinterpret_cast<D3DXVECTOR4*>(this), (D3DXVECTOR4*)&q1, (D3DXVECTOR4*)&q2, w );
}

FORCEINLINE void Quaternion::SLerp( const Quaternion& q1, const Quaternion q2, const float w )
{
	D3DXQuaternionSlerp(this, &q1, &q2, w);
}

FORCEINLINE void Quaternion::SetRotationMatrix( const Matrix& mat )
{
 	D3DXQuaternionRotationMatrix(this, &mat);
}

FORCEINLINE void Quaternion::GetRotationMatrix( OUT Matrix& mat ) const
{
	D3DXMatrixRotationQuaternion(&mat, this);
}

FORCEINLINE void Quaternion::SetRotationXYZ( const float rx, const float ry, const float rz )
{
  	Matrix m;
  	m.RotationXYZ( rx, ry, rz );
  	SetRotationMatrix( m );
}

FORCEINLINE void Quaternion::GetRotationXYZ( OUT float& rx, OUT float& ry, OUT float& rz ) const
{
	Matrix m;
	GetRotationMatrix( m );
	m.GetRoationXYZ( rx, ry, rz );
}

FORCEINLINE void Quaternion::SetRotationAxis( const float3& v, const float a )
{
	w = cosf(a/2);
	x = v.x * sinf(a/2);
	y = v.y * sinf(a/2);
	z = v.z * sinf(a/2);
}

FORCEINLINE void Quaternion::GetRotationAxis( OUT float3& oVec, OUT float& oAng ) const 
{
	oAng= acosf(w) * 2;
	float s = sinf(oAng/2);
	if (s<EPSILON) s = EPSILON;
	oVec.x = x / s;
	oVec.y = y / s;
	oVec.z = z / s;
}

 FORCEINLINE void Quaternion::SetRotationPYR( const float Pitch, const float Yaw, const float Roll )
 {
	floatQ q1( sin(Pitch/2), 0.0f, 0.0f, cos(Pitch/2) );
	floatQ q2( 0.0f, sin(Yaw/2), 0.0f, cos(Yaw/2) );
	
	floatQ r1( q2.w*q1.x, q2.y*q1.w, -q2.y*q1.x, q2.w*q1.w);
	q2.y = 0;	q2.z = sin(Roll/2);	q2.w = cos(Roll/2);
	
	x = -q2.z * r1.y + q2.w * r1.x;
	y =  q2.z * r1.x + q2.w * r1.y;
	z =  q2.z * r1.w + q2.w * r1.z;
	w = -q2.z * r1.z + q2.w * r1.w;

	D3DXQuaternionNormalize( this, this );
 }


FORCEINLINE void Quaternion::Multiply( const Quaternion& q1, const Quaternion& q2 )
{
	D3DXQuaternionMultiply(this, &q1, &q2);
}

FORCEINLINE void Quaternion::Inverse( const Quaternion& q )
{
	D3DXQuaternionInverse(this, &q);
}

FORCEINLINE void Quaternion::Transform( const Quaternion& q, const Matrix& m )
{
	D3DXVec4Transform( reinterpret_cast<D3DXVECTOR4*>(this), reinterpret_cast<D3DXVECTOR4*>(this), &m);
}



//////////////////////////////////////////////////////////////////////////
//	PLANE
//////////////////////////////////////////////////////////////////////////
FORCEINLINE void Plane::Set( const Vector3& a, const Vector3& b, const Vector3 c )
{
	D3DXPlaneFromPoints(this, &a, &b, &c);
}

void Plane::Make( const Vector3& v, const Vector3& n )
{
	D3DXPlaneFromPointNormal(this, &v, &n);
}

FORCEINLINE float Plane::Distance( const Vector3& v ) const
{
	return a*v.x + b*v.y + c*v.z + d;
}

FORCEINLINE void Plane::Normalize( const Plane& p )
{
	D3DXPlaneNormalize(this, &p);
}


//////////////////////////////////////////////////////////////////////////
//	FRUSTUM
//////////////////////////////////////////////////////////////////////////
FORCEINLINE void Frustum::Normalize( const Frustum& f )
{
	for (int i=0; i<6; i++)
		p[i].Normalize( f.p[i] );
}

FORCEINLINE void Frustum::ComputeByMatrix( Matrix& mat )
{
	// Near clipping plane
	p0.a = mat._13;
	p0.b = mat._23;
	p0.c = mat._33;
	p0.d = mat._43;

	// Left clipping plane
	p1.a = mat._14 + mat._11;
	p1.b = mat._24 + mat._21;
	p1.c = mat._34 + mat._31;
	p1.d = mat._44 + mat._41;

	// Right clipping plane
	p2.a = mat._14 - mat._11;
	p2.b = mat._24 - mat._21;
	p2.c = mat._34 - mat._31;
	p2.d = mat._44 - mat._41;

	// Top clipping plane
	p3.a = mat._14 - mat._12;
	p3.b = mat._24 - mat._22;
	p3.c = mat._34 - mat._32;
	p3.d = mat._44 - mat._42;

	// Bottom clipping plane
	p4.a = mat._14 + mat._12;
	p4.b = mat._24 + mat._22;
	p4.c = mat._34 + mat._32;
	p4.d = mat._44 + mat._42;

	// Far clipping plane
	p5.a = mat._14 - mat._13;
	p5.b = mat._24 - mat._23;
	p5.c = mat._34 - mat._33;
	p5.d = mat._44 - mat._43;
}
//////////////////////////////////////////////////////////////////////////
//	3D RECTANGLE
//////////////////////////////////////////////////////////////////////////
Rect3D::Rect3D( void ): v0(VEC3_ZERO), v1(VEC3_ZERO), v2(VEC3_ZERO), v3(VEC3_ZERO) {}
Rect3D::Rect3D( const float3& _v0, const float3& _v1, const float3& _v2, const float3& _v3 ): v0(_v0), v1(_v1), v2(_v2), v3(_v3) {}

FORCEINLINE void Rect3D::Set( const float3& _v0, const float3& _v1, const float3 _v2, const float3 _v3 )
{
	v0 = _v0;	v1 = _v1;	v2 = _v2;	v3 = _v3;
}

FORCEINLINE void Rect3D::Transform( const Rect3D& rect3d, const Matrix& m )
{
	for (int i=0; i<4; i++)
		v[i].Transform(rect3d.v[i], m);
}

//////////////////////////////////////////////////////////////////////////
//	AXIS ALIGNED BOX
//////////////////////////////////////////////////////////////////////////
AABox::AABox( void ): Min(VEC3_ZERO), Max(VEC3_ZERO) {}
AABox::AABox( const float3& _min, const float3& _max ): Min(_min), Max(_max) {}
FORCEINLINE void AABox::Zero( void )
{
	Max = math::VEC3_ZERO;
	Min = math::VEC3_ZERO;
}

FORCEINLINE void AABox::CoverAA( const AABox& box )
{
	if (box.Max.x > Max.x) Max.x = box.Max.x;
	if (box.Max.y > Max.y) Max.y = box.Max.y;
	if (box.Max.z > Max.z) Max.z = box.Max.z;

	if (box.Min.x < Min.x) Min.x = box.Min.x;
	if (box.Min.y < Min.y) Min.y = box.Min.y;
	if (box.Min.z < Min.z) Min.z = box.Min.z;
}

FORCEINLINE void AABox::CoverOB( const OBBox& box )
{
	for (int i=0; i<8; i++)
	{
		if (Max.x < box.v[i].x) Max.x = box.v[i].x;
		if (Max.y < box.v[i].y) Max.y = box.v[i].y;
		if (Max.z < box.v[i].z) Max.z = box.v[i].z;
					
		if (Min.x > box.v[i].x) Min.x = box.v[i].x;
		if (Min.y > box.v[i].y) Min.y = box.v[i].y;
		if (Min.z > box.v[i].z) Min.z = box.v[i].z;
	}
}

FORCEINLINE float AABox::GetVolume( void ) const
{
	return (Max.x-Min.x)*(Max.y-Min.y)*(Max.z-Min.z);
}

FORCEINLINE bool AABox::operator==( const AABox& box ) const 
{
	return (Min == box.Min) && (Max == box.Max);
}

//////////////////////////////////////////////////////////////////////////
//	ORIENTED BOX
//////////////////////////////////////////////////////////////////////////
OBBox::OBBox( void ) { ZeroMemory(v, sizeof(Vector)*8); }
OBBox::OBBox( const AABox& aaBox )
{
	v[0] = aaBox.Max;
	v[1].Set( aaBox.Min.x, aaBox.Max.y, aaBox.Max.z );
	v[2].Set( aaBox.Min.x, aaBox.Min.y, aaBox.Max.z );
	v[3].Set( aaBox.Max.x, aaBox.Min.y, aaBox.Max.z );
	v[4].Set( aaBox.Max.x, aaBox.Min.y, aaBox.Min.z );
	v[5].Set( aaBox.Max.x, aaBox.Max.y, aaBox.Min.z );
	v[6].Set( aaBox.Min.x, aaBox.Max.y, aaBox.Min.z );
	v[7] = aaBox.Min;
}

OBBox::OBBox( const float3& _min, const float3& _max )
{
	v[0] = _max;
	v[1].Set( _min.x, _max.y, _max.z );
	v[2].Set( _min.x, _min.y, _max.z );
	v[3].Set( _max.x, _min.y, _max.z );
	v[4].Set( _max.x, _min.y, _min.z );
	v[5].Set( _max.x, _max.y, _min.z );
	v[6].Set( _min.x, _max.y, _min.z );
	v[7] = _min;
}

FORCEINLINE void OBBox::Transform( const AABox& aaBox, const Matrix& m )
{
	v[0] = aaBox.Max;
	v[1].Set( aaBox.Min.x, aaBox.Max.y, aaBox.Max.z );
	v[2].Set( aaBox.Min.x, aaBox.Min.y, aaBox.Max.z );
	v[3].Set( aaBox.Max.x, aaBox.Min.y, aaBox.Max.z );
	v[4].Set( aaBox.Max.x, aaBox.Min.y, aaBox.Min.z );
	v[5].Set( aaBox.Max.x, aaBox.Max.y, aaBox.Min.z );
	v[6].Set( aaBox.Min.x, aaBox.Max.y, aaBox.Min.z );
	v[7] = aaBox.Min;

	for (int i=0; i<8; i++)
		v[i].Transform(v[i], m);
}

FORCEINLINE bool OBBox::IntersectFrustum( Frustum& fr ) const
{
	// check the visibility via intersect between box and frustum
	bool result = true;
	for (int i=0; i<6 && result; i++)
	{
		result = false;
		for (int j=0; j<8; j++)
		{
			if ( fr.p[i].Distance( v[j] ) >= 0 )
			{
				result = true;
				break;
			}
		}
	}

	return result;
}


//////////////////////////////////////////////////////////////////////////
//	SPHERE
//////////////////////////////////////////////////////////////////////////
Sphere::Sphere( void ): center(VEC3_ZERO), radius(0.0f) {}
Sphere::Sphere( const float3& cen, const float rad ): center(cen), radius(rad) {}

FORCEINLINE void Sphere::Set( const float3& cen, const float rad )
{
	x = cen.x;
	y = cen.y;
	z = cen.z;
	r = rad;
}

FORCEINLINE void Sphere::Zero( void )
{
	x = 0;
	y = 0;
	z = 0;
	r = 0;
}
FORCEINLINE void Sphere::Cover( const Sphere& sphere )
{
	float3 R = center - sphere.center;
	float rad = R.Length() + sphere.r;
	if (r < rad) r = rad;
}
FORCEINLINE bool Sphere::Intersect( const Sphere& sphere, pfloat distance ) const
{
	float3 d( sphere.x - x, sphere.y - y, sphere.z - z );
	if (distance)
	{
		*distance = sqrt( d.x*d.x + d.y*d.y + d.z*d.z );
		return ( *distance < (sphere.r + r) );
	}
	else
	{
		float dis = d.x*d.x + d.y*d.y + d.z*d.z;
		float rad = sphere.r + r;
		return ( dis < rad*rad );
	}
}

FORCEINLINE void Sphere::ComputeByAABox( const AABox& box )
{
	center = box.Min + box.Max;
	center *= 0.5f;
	float3 a = box.Max - box.Min;
	radius = a.Length() / 2;
}

FORCEINLINE void Sphere::Transform( const Sphere& sphere, const Matrix& m )
{
	radius = sphere.radius;
	center.Transform( sphere.center, m );
}

FORCEINLINE bool Sphere::operator==( const Sphere& sphere ) const
{
	return (sphere.x == x) && (sphere.y == y) && (sphere.z == z) && (sphere.r == r);
}

//////////////////////////////////////////////////////////////////////////
//	RAY
//////////////////////////////////////////////////////////////////////////
Ray::Ray( const float3& _pos, const float3& _dir ): pos(_pos), dir(_dir) {}
Ray::Ray( float mouse_abs_x, float mouse_abs_y, float vpWidth, float vpHeight, Matrix& matView, Matrix& matProj )
{
	Compute(mouse_abs_x, mouse_abs_y, vpWidth, vpHeight, matView, matProj);
}

FORCEINLINE void Ray::Set( const float3& _pos, const float3& _dir )
{
	pos = _pos;
	dir = _dir;
}

FORCEINLINE void Ray::Compute( float mouse_abs_x, float mouse_abs_y, float vpWidth, float vpHeight, Matrix& matView, Matrix& matProj )
{
	/*	code from "Keith Ditchburn" (www.toymaker.info/Games/html/picking.html)	*/

	float3 v;
	v.x =  ( ( ( 2.0f * mouse_abs_x ) / vpWidth  ) - 1.0f ) / matProj._11;
	v.y = -( ( ( 2.0f * mouse_abs_y ) / vpHeight ) - 1.0f ) / matProj._22;
	v.z =  1.0f;

	Matrix m;
	m.Inverse( matView );

	// Transform the screen space pick ray into 3D space
	dir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	dir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	dir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
	pos.x = m._41;
	pos.y = m._42;
	pos.z = m._43;

	dir.Normalize( dir );
}

bool Ray::Intersect_Plane( const Plane& plane, pfloat3 outPoint /*= NULL*/, pfloat3 outNormal /*= NULL*/ )
{	// code from : www.bandedsoftware.com/hexgear/concepts/tutorials/ray/ray.html
	float3 n( plane.a, plane.b, plane.c );
	float d = n.Dot(dir);

	if( d >= EPSILON )
	{
		return false;
	}
	else
	{
		if (outPoint)
		{
			float amount = - D3DXPlaneDotCoord( &plane, &pos ) / d;
			*outPoint = pos + dir * amount;
		}

		if (outNormal)
			*outNormal = n;

		return true;		
	}	
}

FORCEINLINE bool Ray::Intersect_Rect3D( const Rect3D& rect3d, pfloat3 outPoint /*= NULL*/, pfloat3 outNormal /*= NULL*/ )
{
	return 
		Intersect_Triangle( rect3d.v0, rect3d.v1, rect3d.v2, outPoint, outNormal ) ||
		Intersect_Triangle( rect3d.v0, rect3d.v2, rect3d.v3, outPoint, outNormal )	;

}

FORCEINLINE bool Ray::Intersect_Sphere( const Sphere& sphere, pfloat3 outPoint /*= NULL*/, pfloat3 outNormal /*= NULL*/ )
{
	/*	code from "Tuomas Tonteri" ( www.sci.tuomastonteri.fi/programming/sse/example3 )	*/

	float a = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
	float b = 2.0f * ( dir.x*(pos.x-sphere.x) + dir.y*(pos.y-sphere.y) + dir.z*(pos.z-sphere.z) );
	float c = sphere.x*sphere.x + sphere.y*sphere.y + sphere.z*sphere.z + 
			  pos.x*pos.x + pos.y*pos.y + pos.z*pos.z - 2.0f*(pos.x*sphere.x + pos.y*sphere.y + pos.z*sphere.z) - sphere.r*sphere.r;
	float D = b*b - 4.0f*a*c;

	// If ray can not intersect then stop
	if (D < 0)
		return false;
	D=sqrtf(D);

	// Ray can intersect the sphere, solve the closer hit point
	float t = (-0.5f)*(b+D)/a;
	if (t > 0.0f)
	{
		if (outPoint)	*outPoint = pos + t*dir;
		if (outNormal)	*outNormal = ( (outPoint ? *outPoint : (pos + t*dir)) - sphere.center ) / sphere.r;		
	}
	else
	{
		if (outPoint)	*outPoint = pos;
		if (outNormal)	*outNormal = -dir;
	}

	return true;
}

FORCEINLINE bool Ray::Intersect_AABox( const AABox& box, pfloat3 outPoint /*= NULL*/, pfloat3 outNormal /*= NULL*/ )
{
	/*	code from "Sepehr Taghdissian" ( www.hmrEngine.com - sep.tagh@gmail.com )	*/

	// Check for point inside box, trivial reject, and determine parametric
	// distance to each front face

	bool inside = true;

	float xt, xn;
	if (pos.x < box.Min.x)
	{
		xt = box.Min.x - pos.x;
		xt /= dir.x;
		inside = false;
		xn = -1.0f;
	} else if (pos.x > box.Max.x) {
		xt = box.Max.x - pos.x;
		xt /= dir.x;
		inside = false;
		xn = 1.0f;
	} else {
		xt = -1.0f;
	}

	float yt, yn;
	if (pos.y < box.Min.y) {
		yt = box.Min.y - pos.y;
		yt /= dir.y;
		inside = false;
		yn = -1.0f;
	} else if (pos.y > box.Max.y) {
		yt = box.Max.y - pos.y;
		yt /= dir.y;
		inside = false;
		yn = 1.0f;
	} else {
		yt = -1.0f;
	}

	float zt, zn;
	if (pos.z < box.Min.z) {
		zt = box.Min.z - pos.z;
		zt /= dir.z;
		inside = false;
		zn = -1.0f;
	} else if (pos.z > box.Max.z) {
		zt = box.Max.z - pos.z;
		zt /= dir.z;
		inside = false;
		zn = 1.0f;
	} else {
		zt = -1.0f;
	}

	// Inside box?

	if (inside) 
	{
		if (outPoint)	*outPoint = pos;
		if (outNormal)	*outNormal = - dir;
		return true;
	}

	// Select farthest Plane - this is
	// the Plane of intersection.

	int which = 0;
	float t = xt;
	if (yt > t) {
		which = 1;
		t = yt;
	}
	if (zt > t) {
		which = 2;
		t = zt;
	}

	switch (which) {

		case 0: // intersect with yz Plane
			{
				float y = pos.y + dir.y*t;
				if (y < box.Min.y || y > box.Max.y) return false;
				float z = pos.z + dir.z*t;
				if (z < box.Min.z || z > box.Max.z) return false;

				if ( outNormal ) outNormal->Set( xn, 0.0f, 0.0f );
			} break;

		case 1: // intersect with xz Plane
			{
				float x = pos.x + dir.x*t;
				if (x < box.Min.x || x > box.Max.x) return false;
				float z = pos.z + dir.z*t;
				if (z < box.Min.z || z > box.Max.z) return false;

				if (outNormal) outNormal->Set( 0.0f, yn, 0.0f );
			} break;

		case 2: // intersect with xy Plane
			{
				float x = pos.x + dir.x*t;
				if (x < box.Min.x || x > box.Max.x) return false;
				float y = pos.y + dir.y*t;
				if (y < box.Min.y || y > box.Max.y) return false;

				if (outNormal) outNormal->Set( 0.0f, 0.0f, zn );
			} break;
	}

	if (outPoint) *outPoint = pos + t*dir;

	return true;
}

FORCEINLINE bool Ray::Intersect_OBBox( const OBBox& box, pfloat3 outPoint /*= NULL*/, pfloat3 outNormal /*= NULL*/ )
{
	if ( Intersect_Triangle(box.v[0], box.v[1], box.v[2], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[0], box.v[2], box.v[3], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[1], box.v[6], box.v[7], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[1], box.v[7], box.v[2], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[6], box.v[5], box.v[4], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[6], box.v[4], box.v[7], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[0], box.v[3], box.v[4], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[4], box.v[5], box.v[0], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[0], box.v[5], box.v[6], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[0], box.v[6], box.v[1], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[4], box.v[3], box.v[2], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[7], box.v[4], box.v[2], outPoint, outNormal) )  return true;

	if ( Intersect_Triangle(box.v[2], box.v[1], box.v[0], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[3], box.v[2], box.v[0], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[7], box.v[6], box.v[1], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[2], box.v[7], box.v[1], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[4], box.v[5], box.v[6], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[7], box.v[4], box.v[6], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[4], box.v[3], box.v[0], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[0], box.v[5], box.v[4], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[6], box.v[5], box.v[0], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[1], box.v[6], box.v[0], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[2], box.v[3], box.v[4], outPoint, outNormal) )  return true;
	if ( Intersect_Triangle(box.v[2], box.v[4], box.v[7], outPoint, outNormal) )  return true;

	return false;
}

FORCEINLINE bool Ray::Intersect_Triangle( const float3& v0, const float3& v1, const float3& v2, pfloat3 outPoint /*= NULL*/, pfloat3 outNormal /*= NULL*/ )
{
	/*	code from "Sepehr Taghdissian" ( www.hmrEngine.com - sep.tagh@gmail.com )	*/

	float3 edge1;
	float3 edge2;
	float3 tvec;
	float3 pvec;
	float3 qvec;

	float det;
	float u, v, t;

	// find vectors for two edges sharing p0
	edge1 = v1 - v0;
	edge2 = v2 - v0;

	pvec.Cross( dir, edge2 );
	det = edge1.Dot( pvec );

	if( det < EPSILON )
		return false;

	tvec = pos - v0;
	u = tvec.Dot( pvec );
	if( u < 0.0f || u > det )
		return false;

	qvec.Cross( tvec, edge1 );
	v = dir.Dot( qvec );
	if( v < 0.0f || u + v > det )
		return false;

	t = edge2.Dot( qvec ) / det;
	if (t<0) return false;

	if (outPoint) *outPoint = pos + t*dir;
	if (outNormal)
	{
		qvec.Cross(edge1, edge2);
		outNormal->Normalize( qvec );
	}

	return true;

}




//////////////////////////////////////////////////////////////////////////
//	MATH FUNCTIONS
//////////////////////////////////////////////////////////////////////////

float DegToRad( const float deg )
{
	return (PI / 180) * deg;
}

float RadToDeg( const float rad )
{
	return (180 / PI) * rad;
}

bool IntersectTriangle(	const Vector3& orig, const Vector3& dir, const Vector3& v0, const Vector3& v1, const Vector3& v2,const bool TwoSide)
{
	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;
	Vector3 pvec, tvec;

	pvec.Cross(dir, edge2); 
	float det = edge1.Dot(pvec);
	if (det > 0) 
		tvec = orig - v0;
	else
	{
		if (!TwoSide) // for 2side detection remove this line 
			return false;		

		tvec = v0 - orig; 
		det = -det;
	};

	if (det < 0.0001)
		return false;

	float u = tvec.Dot(pvec); 
	if ((u < 0.0) || (u > det))
		return false;

	Vector3 qvec;
	qvec.Cross(tvec, edge1); 
	float v = qvec.Dot(dir);
	if ((v < 0.0) || (u + v > det))
		return false;

	return true;
}

bool IntersectTriangle(	const Vector3& orig, const Vector3& dir, const Vector3& v0, const Vector3& v1, const Vector3& v2, OUT Vector2& vout, const bool TwoSide)
{
	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;
	Vector3 pvec, tvec;

	pvec.Cross(dir, edge2); 
	float det = edge1.Dot(pvec);
	if (det > 0) 
		tvec = orig - v0;
	else
	{
		if (!TwoSide) // for 2side detection remove this line 
			return false;		

		tvec = v0 - orig; 
		det = -det;
	};

	if (det < 0.0001)
		return false;

	float u = tvec.Dot(pvec); 
	if ((u < 0.0) || (u > det))
		return false;

	Vector3 qvec;
	qvec.Cross(tvec, edge1); 
	float v = qvec.Dot(dir);
	if ((v < 0.0) || (u + v > det))
		return false;

	vout.x = u / det - 0.5f;
	vout.y = 0.5f - v / det;

	return true;
}



}}  //  namespace sx { namespace math {