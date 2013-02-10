/********************************************************************
	created:	2011/05/03
	filename: 	sxPipeline.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain base class of rendering pipelines.
				the other class of rendering pipeline should inherit
				from this class to implement their technique.

				Rendering pipelines can use Scene and Renderer class
				to accessing and gathering necessary data like camera 
				state, sun light data, nodes in the scene, etc

				Rendering pipelines should use class Settings to check
				target system capabilities and handle settings of game 
				considered by artist and player

				NOTE : SeganX engine use a forward rendering pipeline
				by default to render the scene
*********************************************************************/
#ifndef GUARD_sxPipeline_HEADER_FILE
#define GUARD_sxPipeline_HEADER_FILE

#include "sxCore_def.h"

namespace sx { namespace core {

	/*!
	base class of rendering pipeline will use in renderer system. 
	the other class of rendering pipeline should inherit from this class to implement their technique.
	*/
	class SEGAN_API Pipeline
	{
		SEGAN_STERILE_CLASS(Pipeline);

	public:
		Pipeline(void) {};
		virtual ~Pipeline(void) {};

		//! called by rendering system to create needed resources, render targets, etc
		virtual void CreateResources(int width, int height) = 0;

		//! called by rendering system to destroy resources
		virtual void DestroyResources(void) = 0;

		//! update renderer
		virtual void Update(float elpTime) = 0;

		//! render the scene with determined features
		virtual void RenderScene(DWORD flag) = 0;

		//! use to communicate by this member
		virtual UINT MsgProc(UINT msgType, void* data) = 0;

	};
	typedef Pipeline *PPipeline;
	

}} // namespace sx { namespace core {


#endif	//	GUARD_sxPipeline_HEADER_FILE