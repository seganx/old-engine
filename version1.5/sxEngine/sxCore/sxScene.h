/********************************************************************
created:	2011/03/08
filename: 	sxSettings.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain an static Scene class.
			this scene system can use different typed of scene manager.
			suit scene manager depend on game / simulation type can be
			implemented to scene system in initialization.

			I'm currently thinking of hybrid scene managers which can 
			perform some sort of scene manager types for in/out door scenes.
			there are many article and papers about scene management and
			learning, implementing, skilling up them need more times and
			minds and at this time I have no one :D
*********************************************************************/
#ifndef GUARD_sxScene_HEADER_FILE
#define GUARD_sxScene_HEADER_FILE

#include "sxCore_def.h"
#include "sxNode.h"


typedef void (*Callback_LoadScene)(int count, int index, const WCHAR* state, const WCHAR* name);

namespace sx { namespace core {


	// forward declaration
	class SceneManager;


	/*!
	this static class manage nodes in the scene by given scene manager in 
	the initialization. this class use implemented scene manager at initialize 
	time to manage the objects in the scene. it also contain some global parameters 
	of the scene include sun light matrix, world time, weather info and etc ...
	*/
	class SEGAN_API Scene
	{
	public:

		//! initialize the scene system by given scene manager object
		static void Initialize(SceneManager* sceneManager);

		//! delete all nodes and finalize the scene system.
		static void Finalize(void);

		//! return the current scene manager used in scene system
		static SceneManager* GetSceneManager(void);

		/*!
		clean up the current scene from any node type
		NOTE: this function will delete all nodes in the Scene
		*/
		static void Cleanup(void);

		//! return the number of nodes in the scene
		static int Count(void);

		//! create and return new node but do NOT insert created node to the scene
		static PNode CreateNode(void);

		//! delete a node and all members and childes
		static void DeleteNode(PNode& pNode);

		//! add created node to scene.
		static void AddNode(const PNode pNode);

		/*!
		remove node from Scene
		NOTE: this function does NOT delete the node structure and just remove node's pointer from scene manager
		*/
		static void RemoveNode(const PNode pNode);

		//! fill the node list by all nodes in the scene
		static void GetAllNodes(IN_OUT ArrayPNode_abs& nodeList);

		/*!
		fill the node list by founded nodes in the frustum. use node types as filter
		NOTE: this function called by many parts of core/rendering/AI/etc and should be fast as possible.
		*/
		static void GetNodesByFrustum(const Frustum& frustum, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType = NMT_ALL);

		/*!
		fill the node list by founded nodes in specified area. position and threshold or radius. use node types as filter
		also the application can use nodeTag to additional filter for faster search in scene
		NOTE: this function called by many parts of core/rendering/AI/etc and should be fast as possible.
		*/
		static void GetNodesByArea(const float3& position, float threshold, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType = NMT_ALL, DWORD nodeTag = 0);

		/*!
		search the scene and fill the node list by founded node with specified name
		NOTE: this function is not fast as the others. however using Hash/CRC code can increase performance
		*/
		static void GetNodesByName(const WCHAR* nodeName, IN_OUT ArrayPNode_abs& nodeList);

		//!	send a ray to the scene and fill ray structure as result
		static void GetNodeByRay(IN_OUT msg_IntersectRay& msgRay);

		//! update transformed node to update scene structure
		static void UpdateNode(const PNode pNode);

		//! save all nodes of current scene to the stream except the other data
		static void SaveNodes(Stream& stream);

		/*!
		just load new scene nodes from stream and append them to the current scene
		NOTE: this function does NOT remove current scene nodes and just append new scene
		so call Scene::Cleanup() before load new scene to load levels in game.
		*/
		static void LoadNodes(Stream& stream, Callback_LoadScene callback = 0);

		//! render sectors to the scene
		static void Debug_Draw(const float3& position, const float threshold, DWORD flag);
	};


}} // namespace sx { namespace core {


#endif	//	GUARD_sxScene_HEADER_FILE