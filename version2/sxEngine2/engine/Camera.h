/********************************************************************
	created:	2013/03/12
	filename: 	d3dCamera.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain structure of 3D camera
*********************************************************************/
#ifndef GUARD_d3dCamera_HEADER_FILE
#define GUARD_d3dCamera_HEADER_FILE


#include "../../sxLib/Lib.h"


//! the mode of camera is perspective or orthographic
enum CameraMode
{
	CM_PERSPECTIVE,			//!	camera has a perspective view
	CM_ORTHOGRAPHIC,		//!	camera has an orthographic view

	CM_32BITENUM = 0xffffffff
};


//! structure of camera
class SEGAN_ENG_API Camera
{
public:
	Camera( void );

	//! set spherical parameters
	void SetSpherical( const float radius, const float phi, const float theta );

	//! return spherical parameters
	void GetSpherical( float* raduis = NULL, float* phi = null, float* theta = null ) const;

	//! return camera direction vector
	float3 GetDirection( void ) const;

	//! return view matrix
	matrix GetViewMatrix( void ) const;

	//! return the projection matrix
	matrix GetProjectionMatrix( void ) const;

	//! return perspective matrix. pass -1 for nearZ to compute that automatically
	matrix GetPerspectiveMatrix( const uint viewportWidth, const uint viewportHeight, const float nearZ = -1 ) const;

	//! return orthographic matrix. NOTE: make matrix depend on camera eye, so is not suitable for shadow map generation
	matrix GetOrthographicMatrix( const uint viewportWidth, const uint viewportHeight ) const;

	//! return ray depend on camera
	Ray GetRay( const float absX = -1, const float absY = -1 );

	//! compute and return view parameter use to set LOD of the objects
	float ComputeViewDistance( const float3& position, const float objRadius );

	//! save properties to the stream
	void Save( Stream& stream );

	//! load properties from stream
	void Load( Stream& stream );

	//! set this camera to the device
	void SetToDevice( void );

public:

	CameraMode	m_mode;
	float3		m_eye;
	float3		m_at;
	float3		m_up;
	float		m_fov;
	float		m_far;
	float		m_velocity;
	float		m_amplitude;

};

#endif	//	GUARD_d3dCamera_HEADER_FILE
