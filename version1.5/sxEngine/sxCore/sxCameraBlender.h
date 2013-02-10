/********************************************************************
	created:	2011/04/03
	filename: 	sxCameraBlender.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the calss of camera blender which blends cameras
*********************************************************************/
#ifndef GUARD_sxCameraBlender_HEADER_FILE
#define GUARD_sxCameraBlender_HEADER_FILE

#include "sxCore_def.h"
#include "sxCamera.h"

namespace sx { namespace core {

	//! camera blender blends given camera
	class SEGAN_API CameraBlender
	{

	public:
		//! update the camera parameters
		static void Update(Camera& curCamera, float elpsTime);

		//! return current blended Eye position vector
		static const float3& GetEyePosition(void);

		//! return current blended Target position vector
		static const float3& GetTarget(void);

		//! return current blended Up vector
		static const float3& GetUp(void);

		//! return current blended Field Of View
		static const float GetFOV(void);

		//! return view matrix
		static const Matrix& GetViewMatrix(void);

		//! return projection matrix
		static const Matrix& GetProjectionMatrix(void);

		//! return frustum of the camera
		static const Frustum& GetFrustum(void);

	};

}} // namespace sx { namespace core {

#endif	//	GUARD_sxCameraBlender_HEADER_FILE