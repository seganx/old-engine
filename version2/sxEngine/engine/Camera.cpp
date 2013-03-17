#include "Camera.h"
#include "../sxEngine.h"


static const uint cameraFileID = SEGAN_FCC('C', 'A', 'M', 'R');

//////////////////////////////////////////////////////////////////////////
//	CAMERA STRUCTURE
Camera::Camera( void )
: m_mode(CM_PERSPECTIVE)
, m_fov(PI/3.0f)
, m_far(6000.0f)
, m_velocity(0)
, m_amplitude(0)
, m_eye(0, 0, 1)
, m_at(0, 0, 0)
, m_up(0, 1, 0)
{
	
}

SEGAN_INLINE void Camera::SetSpherical( const float radius, const float phi, const float theta )
{
	float sphi, cphi, stheta, ctheta;
	sx_sin_cos_fast( phi, sphi, cphi );
	sx_sin_cos_fast( theta, stheta, ctheta );

	m_eye.x = m_at.x + radius * sphi * ctheta;
	m_eye.y = m_at.y + radius * stheta;
	m_eye.z = m_at.z + radius * cphi * ctheta;
}

SEGAN_INLINE void Camera::GetSpherical( float* raduis /*= NULL*/, float* phi /*= NULL*/, float* theta /*= NULL*/ ) const
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

SEGAN_INLINE matrix Camera::GetProjectionMatrix( void ) const
{
	if ( m_mode == CM_PERSPECTIVE )
	{
		//  compute near distance depend on m_far distance
		const float znear = ( 0.00005f * m_far );
		const float width = sx_vp_width;
		const float height = sx_vp_height;
		return sx_perspective_fov( m_fov, ( height / width ), znear, m_far );
	}
	else
	{
		const float3 d = m_eye - m_at;
		const float r = sx_length( d );
		const float width = sx_vp_width;
		const float height = sx_vp_height;
		return sx_orthographic( r * ( height / width ), r, - m_far, m_far );
	}
}

SEGAN_INLINE matrix Camera::GetPerspectiveMatrix( const uint viewportWidth, const uint viewportHeight, const float nearZ /*= -1 */ ) const
{
	//  compute near distance depend on m_far distance
	const float znear = nearZ < 0 ? ( 0.00005f * m_far ) : nearZ;
	return sx_perspective_fov( m_fov, (float)viewportWidth / (float)viewportHeight, znear, m_far );
}

SEGAN_INLINE matrix Camera::GetOrthographicMatrix( const uint viewportWidth, const uint viewportHeight ) const
{
	const float3 d = m_eye - m_at;
	const float r = sx_length( d );
	return sx_orthographic( r * ( viewportWidth / viewportHeight ), r, -m_far, m_far );
}

SEGAN_INLINE Ray Camera::GetRay( const float absX /*= -1*/, const float absY /*= -1*/ )
{
	matrix matview = GetViewMatrix();
	matrix matProj = GetProjectionMatrix();
	const float width = sx_vp_width;
	const float height = sx_vp_height;
	const float x = ( absX > -0.1f ) ? absX : sx_mouse_absx(0);
	const float y = ( absY > -0.1f ) ? absY : sx_mouse_absy(0);
	return sx_ray( x, y, width, height, matview, matProj );
	
}

SEGAN_INLINE float Camera::ComputeViewDistance( const float3& position, float objRadius )
{
	return sx_view_distance( m_eye, m_at, m_fov, position, objRadius );
}

void Camera::Save( Stream& stream )
{
	sx_stream_write( cameraFileID );

	uint version = 1;
	sx_stream_write( version);
	sx_stream_write( m_mode );
	sx_stream_write( m_eye );
	sx_stream_write( m_at );
	sx_stream_write( m_up );
	sx_stream_write( m_fov );
	sx_stream_write( m_far );
	sx_stream_write( m_velocity );
	sx_stream_write( m_amplitude );
}

void Camera::Load( Stream& stream )
{
	uint id = 0;
	sx_stream_read( id );
	if ( id != cameraFileID )
	{
		g_logger->Log( L"Incompatible file format for loading camera !" );
		return;
	}

	sint version = 0;
	sx_stream_read( version );

	if ( version == 1 )
	{
		sx_stream_read( version);
		sx_stream_read( m_mode );
		sx_stream_read( m_eye );
		sx_stream_read( m_at );
		sx_stream_read( m_up );
		sx_stream_read( m_fov );
		sx_stream_read( m_far );
		sx_stream_read( m_velocity );
		sx_stream_read( m_amplitude );
	}
}

void Camera::SetToDevice( void )
{
	matrix matview = GetViewMatrix();
	matrix matProj = GetProjectionMatrix();
	g_engine->m_device3D->SetMatrix( MM_VIEW, matview );
	g_engine->m_device3D->SetMatrix( MM_PROJECTION, matProj );
}

