#include "sxScene.h"
#include "sxSceneManager.h"
#include "sxRenderer.h"
#include "sxTerrain.h"

using namespace sx::core;

static const UINT sceneFileID = MAKEFOURCC('S', 'C', 'E', 'N');

class Scene_internal
{
public:
	static	PSceneManager		s_SceneMan;
};
PSceneManager		Scene_internal::s_SceneMan	= NULL;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace sx { namespace core {

	void Scene::Initialize( SceneManager* sceneManager )
	{
		Scene_internal::s_SceneMan = sceneManager;
		Cleanup();
	}

	void Scene::Finalize( void )
	{
		Cleanup();

		if ( Scene_internal::s_SceneMan )
		{
			sx_delete_and_null( Scene_internal::s_SceneMan );
		}
	}

	SceneManager* Scene::GetSceneManager( void )
	{
		return Scene_internal::s_SceneMan;
	}

	void Scene::Cleanup( void )
	{
		sx_callstack();

		if ( !Scene_internal::s_SceneMan ) return;

		ArrayPNode nodeList(128);
		
		Scene_internal::s_SceneMan->GetAllNodes(nodeList);

		for (int i=0; i<nodeList.Count(); i++)
		{
			PNode cur = nodeList[i];
			sx_delete_and_null( cur );
		}
		Scene_internal::s_SceneMan->Clear();

		Renderer::MsgProc(MT_SCENE_CLEAN, NULL);

		//sxLog::Log(L"Scene cleaned with %d remain node(s)!", nodeList.Count() );
	}

	int Scene::Count( void )
	{
		sx_assert(Scene_internal::s_SceneMan);

		return Scene_internal::s_SceneMan->Count();
	}

	sx::core::PNode Scene::CreateNode( void )
	{
		sx_callstack();

		return sx_new( Node );
	}

	void Scene::DeleteNode( PNode& pNode )
	{
		sx_callstack();

		if (Scene_internal::s_SceneMan && pNode)
			RemoveNode(pNode);

		sx_delete_and_null( pNode );
	}

	void Scene::AddNode( const PNode pNode )
	{
		sx_callstack();

		sx_assert(Scene_internal::s_SceneMan);
		if ( !pNode || pNode->m_Sector ) return;

		Scene_internal::s_SceneMan->Add(pNode);
		Renderer::MsgProc(MT_SCENE_ADD_NODE, pNode);
	}

	void Scene::RemoveNode( const PNode pNode )
	{
		sx_assert(Scene_internal::s_SceneMan);
		if ( !pNode || !pNode->m_Sector ) return;

		Scene_internal::s_SceneMan->Remove(pNode);
		Renderer::MsgProc(MT_SCENE_REMOVE_NODE, pNode);
		pNode->m_Sector = NULL;
	}

	void Scene::GetAllNodes( IN_OUT ArrayPNode_abs& nodeList )
	{
		sx_assert(Scene_internal::s_SceneMan);
		Scene_internal::s_SceneMan->GetAllNodes(nodeList);
	}

	void Scene::GetNodesByFrustum( const Frustum& frustum, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType /*= NMT_ALL*/ )
	{
		sx_assert(Scene_internal::s_SceneMan);
		Scene_internal::s_SceneMan->GetNodesByFrustum(frustum, nodeList, nmType);
	}

	void Scene::GetNodesByArea( const float3& position, float threshold, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType, DWORD nodeTag )
	{
		sx_assert(Scene_internal::s_SceneMan);
		Scene_internal::s_SceneMan->GetNodesByArea(position, threshold, nodeList, nmType, nodeTag);
	}

	void Scene::GetNodesByName( const WCHAR* nodeName, IN_OUT ArrayPNode_abs& nodeList )
	{
		sx_assert(Scene_internal::s_SceneMan);
		Scene_internal::s_SceneMan->GetNodesByName(nodeName, nodeList);
	}

	void Scene::GetNodeByRay( IN_OUT msg_IntersectRay& msgRay )
	{
		sx_assert(Scene_internal::s_SceneMan);
		Scene_internal::s_SceneMan->GetNodeByRay(msgRay);
	}

	void Scene::UpdateNode( const PNode pNode )
	{
		sx_callstack();

		sx_assert(Scene_internal::s_SceneMan);
		Scene_internal::s_SceneMan->UpdateNode(pNode);
	}

	void Scene::SaveNodes( Stream& stream )
	{
		sx_assert(Scene_internal::s_SceneMan);

		SEGAN_STREAM_WRITE(stream, sceneFileID);

		int version = 2;
		SEGAN_STREAM_WRITE(stream, version);

		//  version 2
		Terrain::Manager::Save(stream);

		//  version 1
		ArrayPNode_inline nodeList(128);
		Scene_internal::s_SceneMan->GetAllNodes(nodeList);

		int nodeCount = nodeList.Count();
		SEGAN_STREAM_WRITE(stream, nodeCount);

		for (int i=0; i<nodeCount; i++)
		{
			PNode node = nodeList[i];
			node->Save(stream);
		}
	}

	void Scene::LoadNodes( Stream& stream, Callback_LoadScene callback /*= 0*/ )
	{
		sx_callstack();

		sx_assert(Scene_internal::s_SceneMan);

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != sceneFileID)
		{
			sxLog::Log(L"Incompatible file format for loading scene !");
			return;
		}

		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version >= 2)
		{
			if ( callback )
				callback( 0, 0, L"loading", L"terrain" );
			Terrain::Manager::Load(stream);
		}

		if (version >= 1)
		{
			int nodeCount = 0;
			SEGAN_STREAM_READ(stream, nodeCount);

			for (int i=0; i<nodeCount; i++)
			{
				PNode node = sx_new( Node );
				node->Load(stream);
				if ( callback )
					callback( nodeCount, i, L"loading", node->GetName() );
				AddNode(node);
			}
		}
	}

	void Scene::Debug_Draw( const float3& position, const float threshold, DWORD flag )
	{
		sx_assert(Scene_internal::s_SceneMan);

		Scene_internal::s_SceneMan->Debug_Draw( position, threshold, flag );
	}

}} // namespace sx { namespace core {
