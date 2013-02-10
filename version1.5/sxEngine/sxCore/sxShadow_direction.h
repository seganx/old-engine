/********************************************************************
	created:	2011/06/08
	filename: 	sxShadow_direction.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of directional shadow to manage 
				shadow maps. this class create shadow map texture,
				stencil buffer and an smaller shadow map for smoke
*********************************************************************/
#ifndef GUARD_sxShadow_direction_HEADER_FILE
#define GUARD_sxShadow_direction_HEADER_FILE

#include "sxNode.h"
#include "sxCamera.h"
#include "../sxRender/sxRender.h"

namespace sx { namespace core {

	//  simple directional light shadow map generator
	class SEGAN_API Shadow_direction
	{
		SEGAN_STERILE_CLASS(Shadow_direction);

	public:
		Shadow_direction(void);
		~Shadow_direction(void);

		//! return current size of shadow map texture. return zero if no texture created
		int GeShadowSize(void);

		//! create textures and stencils
		void CreateResources(int shadowSize);

		//! destroy textures and stencils
		void DestroyResources(void);

		//! compute light frustum depend on camera and light direction
		void ComputeLightFrustum(OUT Frustum& frustum, Camera& sceneCamera, float3& lightDir, float shadowRange);

		//! render nodes to the shadow maps depend on light direction
		void RenderToShadowMaps(ArrayPNode_abs& nodes);

		//! set shadow map textures to the rendering API device
		void SetShadowMapsToDevice(void);

	private:

		int				m_Width;
		Matrix			m_matView;
		Matrix			m_matProj;
		Frustum			m_Frustum;

		d3d::Texture3D	m_texture_depth;
		d3d::Texture3D	m_texture_color;
	};

}} // namespace sx { namespace core {

#endif	//	GUARD_sxShadow_direction_HEADER_FILE