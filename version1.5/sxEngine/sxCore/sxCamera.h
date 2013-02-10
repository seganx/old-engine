/********************************************************************
	created:	2011/04/01
	filename: 	sxCamera.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the structure of camera
*********************************************************************/
#ifndef GUARD_sxCamera_HEADER_FILE
#define GUARD_sxCamera_HEADER_FILE

#include "sxCore_def.h"

namespace sx { namespace core {

	//! structure of camera
	class SEGAN_API Camera
	{
	public:
		Camera(void);
		Camera(const Camera& cam);

		//! set spherical parameters
		void SetSpherical(float radius, float phi, float theta);

		//! return spherical parameters
		void GetSpherical(pfloat raduis = NULL, pfloat phi = NULL, pfloat theta = NULL);

		//! return camera direction vector
		float3& GetDirection(void);

		//! return view matrix
		void GetViewMatrix(Matrix& matView);

		//! return projection matrix
		void GetProjectionMatrix(Matrix& matProj);

		//! return frustum of the camera
		void GetFrustum(Frustum& frustum);

		//! return ray depend on camera
		Ray& GetRay(float absX, float absY);

		//! set current camera to device
		void SetToDevice(void);

		//! compute and return view parameter use to set LOD of the objects
		float ComputeViewDistance(const float3& position, float objRadius);

		//! save properties to the stream
		void Save(Stream& stream);

		//! load properties from stream
		void Load(Stream& stream);

	public:
		
		float	FOV;
		float	Aspect;
		float	Far;
		float	Velocity;
		float	Amplitude;
		float3	Eye;
		float3	At;
		float3	Up;
		float	orthoWidth;
		float	orthoHeight;
		UINT	matMode;	//  0 = perspective matrix ,  1 = orthogonal matrix

	};
	typedef Camera *PCamera;


}} // namespace sx { namespace core {

#endif	//	GUARD_sxCamera_HEADER_FILE