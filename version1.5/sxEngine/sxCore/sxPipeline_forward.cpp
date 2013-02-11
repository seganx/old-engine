#include "sxPipeline_forward.h"
#include "sxShaderStore.h"
#include "sxSettings.h"
#include "sxCamera.h"
#include "sxNode.h"
#include "sxRenderer.h"
#include "sxScene.h"
#include "sxMesh.h"
#include "sxParticle.h"
#include "sxTerrain.h"

namespace sx { namespace core {

	static float3	camDir(0,0,0);
	static Frustum	camFrustum;

	int CompareNodeDistance_FtoB(const PNode& n1, const PNode& n2)
	{
		if ( n1->m_drawOrder || n2->m_drawOrder )
		{
			return (n1->m_drawOrder < n2->m_drawOrder) ? -1 : (n1->m_drawOrder > n2->m_drawOrder) ? 1 : 0;
		}
		else
		{
			const float3& p1 = n1->GetPosition_world();
			const float3& p2 = n2->GetPosition_world();
			const float d1 = camDir.x*p1.x + camDir.y*p1.y + camDir.z*p1.z;
			const float d2 = camDir.x*p2.x + camDir.y*p2.y + camDir.z*p2.z;
			return (d1 < d2) ? -1 : (d1 > d2) ? 1 : 0;
		}
	}

	int CompareMemberDistance_BtoF(const PNodeMember& m1, const PNodeMember& m2)
	{
		Sphere c1, c2;
		m1->MsgProc(MT_GETSPHERE_WORLD, &c1);
		m2->MsgProc(MT_GETSPHERE_WORLD, &c2);
		const float d1 = camDir.x*c1.x + camDir.y*c1.y + camDir.z*c1.z;
		const float d2 = camDir.x*c2.x + camDir.y*c2.y + camDir.z*c2.z;
		return (d1 > d2) ? -1 : (d1 < d2) ? 1 : 0;
	}

	Pipeline_Forward::Pipeline_Forward( void ) : Pipeline(), m_nodes(2048)
	{

	}

	Pipeline_Forward::~Pipeline_Forward( void )
	{

	}

	void Pipeline_Forward::CreateResources( int width, int height )
	{
		int shadowLevel = Settings::GetOption_Shadow()->GetLevel();
		m_Shadow.DestroyResources();
		m_Shadow.CreateResources( shadowLevel==0 ? 2048 : (shadowLevel==1 ? 1024 : (shadowLevel==2 ? 512 : 256)) );

		int reflectorLevel = Settings::GetOption_Reflection()->GetLevel() + 1;
		m_reflector.DestroyResources();
		m_reflector.CreateResources( width / reflectorLevel, height / reflectorLevel );
	}

	void Pipeline_Forward::DestroyResources( void )
	{
		m_Shadow.DestroyResources();
		m_reflector.DestroyResources();
	}

	void Pipeline_Forward::Update( float elpTime )
	{
		Renderer::GetCamera()->GetFrustum( camFrustum );

		m_nodes.Clear();
		sx::core::Scene::GetNodesByFrustum( camFrustum, m_nodes );		
		for (int i=0; i<m_nodes.Count(); i++)
		{
			m_nodes[i]->Update(elpTime);
		}
	}

	void Pipeline_Forward::RenderScene( DWORD flag )
	{
		if ( Settings::GetOption_Reflection()->HasPermission() )
		{
			if ( !( flag & SX_DRAW_DEBUG || flag & SX_DRAW_PATH ) )
			{
				//  check settings of the reflector
				static int reflectorLevel = Settings::GetOption_Reflection()->GetLevel() + 1;
				if ( reflectorLevel != Settings::GetOption_Reflection()->GetLevel() + 1 )
				{
					reflectorLevel = Settings::GetOption_Reflection()->GetLevel() + 1;
					m_reflector.DestroyResources();
					m_reflector.CreateResources( int(SEGAN_VP_WIDTH / reflectorLevel), int(SEGAN_VP_HEIGHT / reflectorLevel) );
				}

				//  get frustum
				Frustum reflectFrustum;
				m_reflector.ComputeFrustum( reflectFrustum, *Renderer::GetCamera() );

				//  collect nodes in reflect frustum and render them to shadow map
				static sx::core::ArrayPNode_inline nodes(2048);
				nodes.Clear();
				core::Scene::GetNodesByFrustum( reflectFrustum, nodes );
				m_reflector.RenderToReflector( nodes, flag );
			}

			m_reflector.SetToDevice();
		}


		//  update shadow
		if ( flag & SX_SHADER_SHADOW && Settings::GetOption_Shadow()->HasPermission() )
		{
			{
				//  check settings of the shadow
				static int shadowLevel = Settings::GetOption_Shadow()->GetLevel();
				if ( shadowLevel != Settings::GetOption_Shadow()->GetLevel() )
				{
					shadowLevel = Settings::GetOption_Shadow()->GetLevel();
					m_Shadow.DestroyResources();

					//  choose texture size of shadow map
					m_Shadow.CreateResources( shadowLevel==0 ? 2048 : (shadowLevel==1 ? 1024 : (shadowLevel==2 ? 512 : 256)) );
				}

				for (int i=0; i<m_nodes.Count(); i++)
					SEGAN_SET_REM( m_nodes[i]->m_reserved, SX_SHADER_SHADOW );

				//  collect shadow information
				float shadowRange = shadowLevel==0 ? 250.0f : (shadowLevel==1 ? 200.0f : (shadowLevel==2 ? 150.0f : 100.0f) );
				float3 lightDir( (float*)Renderer::GetSunLight() );
				
				//  get frustum of the light
				Frustum lightFrustum;
				m_Shadow.ComputeLightFrustum(lightFrustum, *Renderer::GetCamera(), lightDir, shadowRange);

				//  collect nodes in light frustum and render them to shadow map
				static sx::core::ArrayPNode_inline nodes(2048);
				nodes.Clear();
				core::Scene::GetNodesByFrustum(lightFrustum, nodes, NMT_MESH);
				m_Shadow.RenderToShadowMaps( nodes );

				for (int i=0; i<nodes.Count(); i++)
					SEGAN_SET_ADD( nodes[i]->m_reserved, SX_SHADER_SHADOW );
			}
			m_Shadow.SetShadowMapsToDevice();

			//  draw nodes with shadow
			DrawNodes(flag);
		}
		else
		{
			for (int i=0; i<m_nodes.Count(); i++)
				SEGAN_SET_REM( m_nodes[i]->m_reserved, SX_SHADER_SHADOW );

			//  draw nodes without shadow
			SEGAN_SET_REM(flag, SX_SHADER_SHADOW);
			DrawNodes(flag);
		}
	}

	void Pipeline_Forward::DrawNodes( DWORD flag )
	{
		Matrix matView;
		d3d::Device3D::Matrix_View_Get(matView);

		//  sort nodes from front to back
		camDir.Set( matView._13, matView._23, matView._33 );
		m_nodes.Sort( &CompareNodeDistance_FtoB );
	
		sx::d3d::Device3D::RS_Fog( true );
		sx::d3d::Device3D::RS_WireFrame( (flag & SX_DRAW_WIRED) != 0 );

		//////////////////////////////////////////////////////////////////////////
		//  draw node usually or not ?
		if ( flag & SX_DRAW_DEBUG || flag & SX_DRAW_PATH )
		{
			int h = m_nodes.Count()-1;
			for (int i=0; i<=h; i++)
				m_nodes[i]->Draw(flag);

			for (int i=h; i>=0; i--)
				m_nodes[i]->Draw(flag | SX_DRAW_ALPHA);
		}
		else
		{
			int h = m_nodes.Count()-1;

			if ( flag & SX_DRAW_MESH )
			{
				for (int i=0; i<=h; i++)
					DrawANode( m_nodes[i], SX_DRAW_MESH | SX_SHADER_SHADOW );
			}

			if ( flag & SX_DRAW_TERRAIN )
			{
				Terrain::Manager::BeginPatch( camFrustum, Renderer::GetCamera()->FOV );

				for (int i=h; i>=0; i--)
					DrawANode( m_nodes[i], SX_DRAW_TERRAIN | SX_SHADER_SHADOW );
				
				Terrain::Manager::EndPatch( flag );
			}


			for (int i=h; i>=0; i--)
				DrawANode( m_nodes[i], SX_DRAW_ALPHA | flag );

		}

		sx::d3d::Device3D::RS_WireFrame( false );
		sx::d3d::Device3D::RS_Fog( false );
	}

	void Pipeline_Forward::DrawANode( PNode node, DWORD flag )
	{
		//  check to use shadow shader or not
		if ( !(node->m_reserved & SX_SHADER_SHADOW) )
			SEGAN_SET_REM( flag, SX_SHADER_SHADOW );

		// compute view parameter use to set LOD of the objects
		float dis = cmn::ViewDistanceByFrustom(camFrustum, Renderer::GetCamera()->FOV, node->GetPosition_world(), node->GetSphere_local().r );

		//  extract all members from the node
		PNodeMember member = NULL;
		static ArrayPNodeMember memberList(4096);
		memberList.Clear();
		node->GetMembersByType(NMT_MESH | NMT_PARTICLE | NMT_ANIMATOR | NMT_TERRAIN, memberList, true);
		int n = memberList.Count();

		//  sort members from back to front for rendering objects with alpha blending
		if ( flag & SX_DRAW_ALPHA )
		{
			//  before sorting alpha objects set first animation data to device
			//  this is a restriction for alpha objects to have a single animator
			//  but currently I have no idea to solve the problem
			for (int i=0; i<n; i++)
			{
				member = memberList[i];
				if ( member->GetType() == NMT_ANIMATOR )
				{
					member->Draw( flag );
					break;
				}
			}

			memberList.Sort(&CompareMemberDistance_BtoF);
		}

		//  draw members
		for (int i=0; i<n; i++)
		{
			member = memberList[i];
			switch ( member->GetType() )
			{
			case NMT_ANIMATOR:
				{
					if ( !(flag & SX_DRAW_ALPHA ) )
						member->Draw( flag );
				}
				break;

			case NMT_MESH:
				if ( flag & SX_DRAW_MESH )
				{
					OBBox box;
					if ( (!member->MsgProc( MT_GETBOX_WORLD, &box )) && box.IntersectFrustum(camFrustum ) )
					{
						member->DrawByViewParam( flag, dis );
					}
				}
				break;

			case NMT_TERRAIN:
				if ( flag & SX_DRAW_TERRAIN )
				{
					OBBox box;
					if ( (!member->MsgProc( MT_GETBOX_WORLD, &box )) && box.IntersectFrustum(camFrustum ) )
					{
						member->DrawByViewParam( flag, dis );
					}
				}
				break;

			case NMT_PARTICLE:
				if ( flag & SX_DRAW_PARTICLE )
				{
					OBBox box;
					if ( (!member->MsgProc( MT_GETBOX_WORLD, &box )) && box.IntersectFrustum(camFrustum ) )
					{
						member->DrawByViewParam( flag, dis );
					}
				}
			}
		}
	}

	UINT Pipeline_Forward::MsgProc( UINT msgType, void* data )
	{
		sx_callstack_param(Pipeline_Forward::MsgProc());

		switch (msgType)
		{
		case MT_NODE_DELETE:
		case MT_SCENE_REMOVE_NODE:
			{
				PNode node = static_cast<PNode>(data);
				m_nodes.Remove(node);
			}
			break;

		case MT_SCENE_CLEAN:
			{
				m_nodes.Clear();
			}
			break;
		}

		return msgType;
	}
}} // namespace sx { namespace core {
