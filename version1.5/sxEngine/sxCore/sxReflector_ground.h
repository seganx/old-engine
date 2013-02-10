/********************************************************************
	created:	2012/09/20
	filename: 	sxReflector_ground.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class to create a reflection on the ground
*********************************************************************/
#ifndef GUARD_sxReflector_ground_HEADER_FILE
#define GUARD_sxReflector_ground_HEADER_FILE


#include "sxNode.h"
#include "sxCamera.h"
#include "../sxRender/sxRender.h"

namespace sx { namespace core {

	//  simple directional light shadow map generator
	class SEGAN_API Reflector_ground
	{
		SEGAN_STERILE_CLASS(Reflector_ground);

	public:
		Reflector_ground(void);
		~Reflector_ground(void);

		//! create textures and stencils
		void CreateResources( const int width, const int height );

		//! destroy textures and stencils
		void DestroyResources(void);

		//! compute frustum depend on camera
		void ComputeFrustum( OUT Frustum& frustum, Camera& sceneCamera );

		//! render nodes to the reflection texture
		void RenderToReflector(ArrayPNode_abs& nodes, DWORD flag );

		//! set reflection textures to the rendering API device
		void SetToDevice(void);

	public:

		FogDesc			m_fog;			//	use fog in reflection make scene better
		float			m_y;			//	XZ plan offset
		Matrix			m_matView;
		Matrix			m_matProj;
		d3d::Texture3D	m_texture;		//	reflection texture

	};

}} // namespace sx { namespace core {

#endif	//	GUARD_sxReflector_ground_HEADER_FILE
