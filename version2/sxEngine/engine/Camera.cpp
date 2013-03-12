#include "Camera.h"

static const uint cameraFileID = SEGAN_FCC('C', 'A', 'M', 'R');

//////////////////////////////////////////////////////////////////////////
//	CAMERA STRUCTURE
Camera::Camera( void )
: m_fov(PI/3.0f)
, m_far(6000.0f)
, m_velocity(0)
, m_amplitude(0)
, m_eye(0.0f, 0.0f, -1.0f)
, m_at(0.0f, 0.0f, 0.0f)
, m_up(0.0f, 1.0f, 0.0f)
{
	
}

SEGAN_INLINE void Camera::SetSpherical( const float radius, const float phi, const float theta )
{
	float sphi, cphi, stheta, ctheta;
	sx_sin_cos_fast( phi, sphi, cphi );
	sx_sin_cos_fast( theta, stheta, ctheta );

	m_eye.x = m_at.x + radius * cphi * stheta;
	m_eye.y = m_at.y + radius * ctheta;
	m_eye.z = m_at.z + radius * sphi * stheta;
}

void Camera::GetSpherical( float* raduis /*= NULL*/, float* phi /*= NULL*/, float* theta /*= NULL*/ ) const
{
	float3 d = m_eye - m_at;
	float r = sx_length( d );
	if ( r < 0.01f ) r = 0.01f;
	if ( raduis ) *raduis = r;
	
	float t = sx_clamp_f( sx_acos( d.y / r ), 0.001f, 3.135f );
	if (theta) *theta = t;

	if (phi)
	{
		float rad = sx_sin( t ) * r;
		if ( rad < EPSILON ) rad = EPSILON;
		rad = sx_asin( d.z / rad );

		if ( d.x > 0 )
			*phi = rad;
		else
			*phi = PI - rad;
	}
}

SEGAN_INLINE float3 Camera::GetDirection( void ) const
{
	return sx_normalize( m_at - m_eye );
}

SEGAN_INLINE matrix Camera::GetViewMatrix( void ) const
{
	return sx_lookat( m_eye, m_at, m_up );
}

SEGAN_INLINE matrix Camera::GetPerspectiveMatrix( const uint viewportWidth, const uint viewportHeight, const float nearZ /*= -1 */ ) const
{
	//  compute near distance depend on m_far distance
	const float znear = nearZ < 0 ? ( 0.00005f * m_far ) : nearZ;
	return sx_perspective_fov( m_fov, (float)viewportWidth / (float)viewportHeight, znear, m_far );
}

matrix Camera::GetOrthographicMatrix( const uint viewportWidth, const uint viewportHeight ) const
{
	const float3 d = m_eye - m_at;
	const float r = sx_length( d );
	return sx_orthographic( r * ( viewportWidth / viewportHeight ), r, -m_far, m_far );
}

#if 0

Ray& Camera::GetRay( float absX, float absY )
{
	static Ray ray(math::VEC3_ZERO, math::VEC3_ZERO);

	Matrix matView, matProj;
	GetViewMatrix(matView);
	GetPerspectiveMatrix(matProj);
	ray.Compute( absX, absY, SEGAN_VP_WIDTH, SEGAN_VP_HEIGHT, matView, matProj );
	return ray;
}

FORCEINLINE float Camera::ComputeViewDistance( const float3& position, float objRadius )
{
	return cmn::ViewDistanceByCamera(m_eye, m_at, m_fov, position, objRadius);
}

void Camera::Save( Stream& stream )
{
	SEGAN_STREAM_WRITE(stream, cameraFileID);

	int version = 1;
	SEGAN_STREAM_WRITE(stream, version);

	SEGAN_STREAM_WRITE(stream, m_fov);
	SEGAN_STREAM_WRITE(stream, m_aspect);
	SEGAN_STREAM_WRITE(stream, m_far);
	SEGAN_STREAM_WRITE(stream, m_velocity);
	SEGAN_STREAM_WRITE(stream, m_amplitude);
	SEGAN_STREAM_WRITE(stream, m_eye);
	SEGAN_STREAM_WRITE(stream, m_at);
	SEGAN_STREAM_WRITE(stream, m_up);
	SEGAN_STREAM_WRITE(stream, m_orthoWidth);
	SEGAN_STREAM_WRITE(stream, m_orthoHeight);
	SEGAN_STREAM_WRITE(stream, m_matMode);

}

void Camera::Load( Stream& stream )
{
	UINT id = 0;
	SEGAN_STREAM_READ(stream, id);
	if (id != cameraFileID)
	{
		sxLog::Log(L"Incompatible file format for loading camera !");
		return;
	}

	int version = 0;
	SEGAN_STREAM_READ(stream, version);

	if ( version == 1 )
	{
		SEGAN_STREAM_READ(stream, m_fov);
		SEGAN_STREAM_READ(stream, m_aspect);
		SEGAN_STREAM_READ(stream, m_far);
		SEGAN_STREAM_READ(stream, m_velocity);
		SEGAN_STREAM_READ(stream, m_amplitude);
		SEGAN_STREAM_READ(stream, m_eye);
		SEGAN_STREAM_READ(stream, m_at);
		SEGAN_STREAM_READ(stream, m_up);
		SEGAN_STREAM_READ(stream, m_orthoWidth);
		SEGAN_STREAM_READ(stream, m_orthoHeight);
		SEGAN_STREAM_READ(stream, m_matMode);
	}

}

#endif

