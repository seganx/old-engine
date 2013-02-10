/********************************************************************
	created:	2011/05/03
	filename: 	sxPipeline_forward.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple rendering pipeline which
				will be used by engine as default rendering pipeline.


				NOTE : 
				The actual view distance from camera to target is ' disCameraToTarget = disEyeToAt * (1 - cos(FOV)) '
				view distance is different with distance of camera and target. decreasing FOV will decrease view distance
				To compute distance between camera and objects we can easily replace target position by object position
				the other important parameter is the object's volume. huge objects can be seen from far distance :)
*********************************************************************/
#ifndef GUARD_sxPipeline_forward_HEADER_FILE
#define GUARD_sxPipeline_forward_HEADER_FILE

#include "sxPipeline.h"
#include "sxNode.h"
#include "sxShadow_direction.h"
#include "sxReflector_ground.h"

namespace sx { namespace core {

	//! simple rendering pipeline use forward rendering technique
	class SEGAN_API Pipeline_Forward : public Pipeline
	{
		SEGAN_STERILE_CLASS(Pipeline_Forward);

	public:
		Pipeline_Forward(void);
		virtual ~Pipeline_Forward(void);

		//! called by rendering system to create needed resources, render targets, etc
		void CreateResources(int width, int height);

		//! called by rendering system to destroy resources
		void DestroyResources(void);

		//! update renderer
		void Update(float elpTime);

		//! render the scene with determined features
		void RenderScene(DWORD flag);

		//! use to communicate by this member
		virtual UINT MsgProc(UINT msgType, void* data);

	private:

		//! draw nodes in the scene
		void DrawNodes(DWORD flag);

		//  draw a single node
		void DrawANode(PNode node, DWORD flag);

		ArrayPNode			m_nodes;
		Shadow_direction	m_Shadow;		//  simple directional shadow
		Reflector_ground	m_reflector;	//	simple ground reflection

	};

}} // namespace sx { namespace core {


#endif	//	GUARD_sxPipeline_forward_HEADER_FILE