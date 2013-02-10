/********************************************************************
created:	2011/03/13
filename: 	sxSceneManager.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain the interface of scene managers.
			To implement any type of scene manager use this interface
			and create your own scene manager class.

			NOTE: the scene managers should manage only the parent nodes.
			modifying the child nodes and/or member of nodes in scene manager 
			is highly not recommended. to access the child nodes or member of
			a node, find the parent and access them directly from the parents.
*********************************************************************/
#ifndef GUARD_sxSceneManager_HEADER_FILE
#define GUARD_sxSceneManager_HEADER_FILE

#include "sxCore_def.h"
#include "sxNode.h"

namespace sx { namespace core {

	class SEGAN_API SceneManager
	{
		SEGAN_STERILE_CLASS(SceneManager);

	public:
		SceneManager(void) {};
		virtual ~SceneManager(void) {};

		/*!
		clear all the nodes from Scene
		NOTE: this function does NOT delete any node structure and just remove node's pointer from scene manager
		*/
		virtual void Clear(void) = 0;

		//! return the number of nodes in the scene
		virtual int Count(void) = 0;

		//! add new node to scene.
		virtual void Add(const PNode pNode) = 0;

		/*!
		remove node from Scene
		NOTE: this function does NOT delete the node structure and just remove node's pointer from scene manager
		*/
		virtual void Remove(const PNode pNode) = 0;

		//! fill the node list by all nodes in the scene
		virtual void GetAllNodes(IN_OUT ArrayPNode_abs& nodeList) = 0;

		/*!
		fill the node list by founded nodes in the frustum.. use node types as filter
		NOTE: this function called by many parts of core/rendering/AI/etc and should be fast as possible.
		*/
		virtual void GetNodesByFrustum(const Frustum& frustum, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType = NMT_ALL) = 0;

		/*!
		fill the node list by founded nodes in specified area. position and threshold or radius. use node types as filter
		also the application can use nodeTag to additional filter for faster search in scene
		NOTE: this function called by many parts of core/rendering/AI/etc and should be fast as possible.
		*/
		virtual void GetNodesByArea(const float3& position, const float threshold, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType = NMT_ALL, DWORD nodeTag = 0) = 0;

		/*!
		search the scene and fill the node list by founded node with specified name
		NOTE: this function is not fast as the others. however using Hash/CRC code can increase performance
		*/
		virtual void GetNodesByName(const WCHAR* nodeName, IN_OUT ArrayPNode_abs& nodeList) = 0;

		//!	send a ray to the scene and fill ray structure as result
		virtual void GetNodeByRay(IN_OUT msg_IntersectRay& msgRay) = 0;

		//! update transformed node to update scene structure
		virtual void UpdateNode(const PNode pNode) = 0;

		//! render sectors to the scene for debug mode
		virtual void Debug_Draw(const float3& position, const float threshold, DWORD flag) = 0;

	};
	typedef SceneManager *PSceneManager;

}} // namespace sx { namespace core {

#endif // GUARD_sxSceneManager_HEADER_FILE