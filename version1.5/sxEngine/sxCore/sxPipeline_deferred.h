/********************************************************************
	created:	2011/05/03
	filename: 	sxPipeline_deferred.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class of rendering pipeline which
				use deferred rendering technique
*********************************************************************/
#ifndef GUARD_sxPipeline_deferred_HEADER_FILE
#define GUARD_sxPipeline_deferred_HEADER_FILE

#include "sxPipeline.h"
#include "sxNode.h"

namespace sx { namespace core {

	//! rendering pipeline which use deferred rendering technique
	class SEGAN_API Pipeline_Deferred : public Pipeline
	{
		SEGAN_STERILE_CLASS(Pipeline_Deferred);

	public:
		Pipeline_Deferred(void);
		virtual ~Pipeline_Deferred(void);

		//! called by rendering system to create needed resources, render targets, etc
		void CreateResources(int width, int height);

		//! called by rendering system to destroy resources
		void DestroyResources(void);

		//! update renderer
		void Update(float elpTime);

		//! render the scene with determined features
		void RenderScene(DWORD flag);

		//////////////////////////////////////////////////////////////////////////
		//  additional functions
		//////////////////////////////////////////////////////////////////////////

		//! render nodes in the list to the current back buffer
		void def_RenderNodes(ArrayPNode_abs& nodeList, DWORD flag);

		//! render point lights of nodes in the list to the scene
		void def_RenderPointLights(ArrayPNode_abs& nodeList, DWORD flag);

		//! render nodes to the shadow map
		void def_RenderToShadowMap(ArrayPNode_abs& nodeList, DWORD flag);

		//! render nodes to the prerequisite textures 
		void def_RenderToGBuffer(ArrayPNode_abs& nodeList, DWORD flag);

		//! render the scene with full effects
		void def_Render(ArrayPNode_abs& nodeList, DWORD flag);

	private:
		sx::d3d::PTexture		m_mainRT;
		sx::d3d::PTexture		m_Scene;
		sx::d3d::PTexture		m_Graphc;
		sx::d3d::PTexture		m_ZDepth;
		sx::d3d::PTexture		m_Normal;
		//sx::d3d::PTexture		m_Shadow;
		sx::d3d::Shader3D		m_postPresent;
		sx::d3d::Shader3D		m_postShadeScene;
	};

}} // namespace sx { namespace core {


#endif	//	GUARD_sxPipeline_deferred_HEADER_FILE