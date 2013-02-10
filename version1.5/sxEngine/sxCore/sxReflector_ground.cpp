#include "sxReflector_ground.h"
#include "sxShaderStore.h"
#include "sxRenderer.h"
#include "sxMesh.h"
#include "sxAnimator.h"
#include "sxTerrain.h"


namespace sx { namespace core {

	//////////////////////////////////////////////////////////////////////////
	//	STATIC VARIABLES
	//////////////////////////////////////////////////////////////////////////
	static float3	camDir(0,0,0);
	static Frustum	camFrustum;

	int reflct_CompareNodeDistance_FtoB(const PNode& n1, const PNode& n2)
	{
		if ( n1->m_drawOrder || n2->m_drawOrder )
		{
			return (n1->m_drawOrder < n2->m_drawOrder) ? 1 : (n1->m_drawOrder > n2->m_drawOrder) ? -1 : 0;
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

	int reflct_CompareMemberDistance_BtoF(const PNodeMember& m1, const PNodeMember& m2)
	{
		Sphere c1, c2;
		m1->MsgProc(MT_GETSPHERE_WORLD, &c1);
		m2->MsgProc(MT_GETSPHERE_WORLD, &c2);
		const float d1 = camDir.x*c1.x + camDir.y*c1.y + camDir.z*c1.z;
		const float d2 = camDir.x*c2.x + camDir.y*c2.y + camDir.z*c2.z;
		return (d1 > d2) ? -1 : (d1 < d2) ? 1 : 0;
	}

	//////////////////////////////////////////////////////////////////////////
	//  SOME HELPER FUNCTIONS
	//////////////////////////////////////////////////////////////////////////
	void DrawNodeToReflector( PNode node, DWORD flag )
	{
		// compute view parameter use to set LOD of the objects
		float dis = 1.5f * cmn::ViewDistanceByFrustom(camFrustum, Renderer::GetCamera()->FOV, node->GetPosition_world(), node->GetSphere_local().r );

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

			memberList.Sort( &reflct_CompareMemberDistance_BtoF );
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
					if ( 
						!member->HasOption( SX_MESH_INVIS_IN_REFLET )	&&
						!member->MsgProc( MT_GETBOX_WORLD, &box )		&& 
						box.IntersectFrustum( camFrustum )
						)
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
					if (!member->HasOption( SX_PARTICLE_INVIS_IN_REFLET )	&& 
						!member->MsgProc( MT_GETBOX_WORLD, &box )			&&
						box.IntersectFrustum(camFrustum ) )
					{
						member->DrawByViewParam( flag, dis );
					}
				}
			}
		}
	}

	void RenderNodesToReflector( ArrayPNode_abs& nodes, DWORD flag )
	{
		nodes.Sort( &reflct_CompareNodeDistance_FtoB );

		sx::d3d::Device3D::RS_Fog( true );
		sx::d3d::Device3D::RS_WireFrame( (flag & SX_DRAW_WIRED) != 0 );
		
		int h = nodes.Count()-1;
		if ( flag & SX_DRAW_MESH )
		{
			for (int i=0; i<=h; i++)
				DrawNodeToReflector( nodes[i], SX_DRAW_MESH /*| SX_SHADER_SHADOW*/ );
		}

		if ( flag & SX_DRAW_TERRAIN )
		{
			Terrain::Manager::BeginPatch( camFrustum, Renderer::GetCamera()->FOV );

			for (int i=h; i>=0; i--)
				DrawNodeToReflector( nodes[i], SX_DRAW_TERRAIN /*| SX_SHADER_SHADOW*/ );

			Terrain::Manager::EndPatch( flag );
		}


		for (int i=h; i>=0; i--)
			DrawNodeToReflector( nodes[i], SX_DRAW_ALPHA | flag );

		sx::d3d::Device3D::RS_WireFrame( false );
		sx::d3d::Device3D::RS_Fog( false );
	}

	//////////////////////////////////////////////////////////////////////////
	//  REFLECTOR ON GROUND
	//////////////////////////////////////////////////////////////////////////
	Reflector_ground::Reflector_ground( void ): m_y(0)
	{
		ZeroMemory( &m_fog, sizeof(m_fog) );
	}

	Reflector_ground::~Reflector_ground( void )
	{
		DestroyResources();
	}

	void Reflector_ground::CreateResources( const int width, const int height )
	{
		if ( !m_texture.CreateRenderTarget(D3DRTYPE_TEXTURE, width, height, D3DFMT_X8R8G8B8) )
		{
			DestroyResources();
			return;
		}
	}

	void Reflector_ground::DestroyResources( void )
	{
		m_texture.Cleanup();
	}

	void Reflector_ground::ComputeFrustum( OUT Frustum& frustum, Camera& sceneCamera )
	{
		Camera cam = sceneCamera;
		cam.Up.x *= -1;
		cam.Up.z *= -1;
		if ( cam.Eye.y > 0 )
		{
			cam.At.y *= -1;
			cam.Eye.y *= -1;
		}
		else
			cam.Up.y *= -1;

		cam.GetFrustum( frustum );
		cam.GetViewMatrix( m_matView );
		cam.GetProjectionMatrix( m_matProj );
		camFrustum = frustum;
		camDir.Normalize( cam.GetDirection() );

		//	apply clip plane in projection matrix
		float3 a( -1.0f, 0.0f,  1.0f );
		float3 b(  1.0f, 0.0f,  1.0f );
		float3 c( -1.0f, 0.0f, -1.0f );
		Plane plane; plane.Set( a, b, c );
		plane.Normalize( plane );

		// transform clip plane into projection space
		Matrix matViewProj;
		matViewProj.Multiply( m_matView, m_matProj );
		matViewProj.Inverse( matViewProj );
		matViewProj.Transpose( matViewProj );

		float4 clipPlane( plane.a, plane.b, plane.c, plane.d );
		clipPlane.Transform( clipPlane, matViewProj );

		if (clipPlane.w == 0)
		{
			return;
		}
		clipPlane.Normalize( clipPlane );
		if (clipPlane.w > 0)
		{
			clipPlane *= -1;
			clipPlane.w += 1;
		}

		// put projection space clip plane in Z column
		Matrix matClip = math::MTRX_IDENTICAL;
		matClip._13 = clipPlane.x;
		matClip._23 = clipPlane.y;
		matClip._33 = clipPlane.z;
		matClip._43 = clipPlane.w;

		// multiply into projection matrix
		m_matProj.Multiply( m_matProj, matClip );
	}

	void Reflector_ground::RenderToReflector( ArrayPNode_abs& nodes, DWORD flag )
	{
		//  render nodes to the texture
		if ( !nodes.Count() ) return;

		//  save old states
		FogDesc old_fog;
		PDirect3DSurface rt_old;
		Matrix matView_old, matProj_old;
		d3d::Device3D::GetFogDesc( old_fog );
		d3d::Device3D::GetRenderTarget(0, rt_old);
		d3d::Device3D::Matrix_View_Get(matView_old);
		d3d::Device3D::Matrix_Project_Get(matProj_old);

		//	prepare the fog to apply
		m_fog.Color = old_fog.Color;

		//  set new states
		m_texture.SetAsRenderTarget(0);
		d3d::Device3D::SetFogDesc( m_fog );
		d3d::Device3D::RS_Fog( true );
		d3d::Device3D::Clear_Screen( m_fog.Color );

		//  set reflector camera to the device
		d3d::Device3D::Matrix_View_Set( m_matView );
		d3d::Device3D::Matrix_Project_Set( m_matProj );
		d3d::ShaderPool::Update(0);

		//  render meshes to the texture
		RenderNodesToReflector( nodes, flag );

		//  restore old states
		d3d::Device3D::SetRenderTarget(0, rt_old);
		d3d::Device3D::Matrix_View_Set(matView_old);
		d3d::Device3D::Matrix_Project_Set(matProj_old);
		d3d::ShaderPool::Update(0);
		d3d::Device3D::SetFogDesc( old_fog );
		d3d::Device3D::RS_Fog( false );
		d3d::Device3D::Clear_ZBuffer();
	}

	FORCEINLINE void Reflector_ground::SetToDevice( void )
	{
		m_texture.SetToDevice( 5 );
	}

}}//namespace sx { namespace core {
