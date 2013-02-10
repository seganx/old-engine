#include "sxShadow_direction.h"
#include "sxShaderStore.h"
#include "sxRenderer.h"
#include "sxMesh.h"
#include "sxAnimator.h"


#define LIGHT_FAR_MAX	10000

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
//////////////////////////////////////////////////////////////////////////

namespace sx { namespace core {

	//////////////////////////////////////////////////////////////////////////
	//  SOME HELPER FUNCTIONS
	//////////////////////////////////////////////////////////////////////////
	void DrawNodeToShadowMap(PNode node, float viewParam)
	{
		if ( node->MemberTypeExist(NMT_MESH) || node->MemberTypeExist(NMT_ANIMATOR) )
		{
			for (int j=0; j<node->GetMemberCount(); j++)
			{
				PNodeMember pMember = node->GetMemberByIndex(j);
				switch (pMember->GetType())
				{
				case NMT_ANIMATOR:
					{
						static_cast<PAnimator>(pMember)->Draw(SX_DRAW_MESH);
					}
					break;

				case NMT_MESH:
					{
						PMesh mesh = static_cast<PMesh>(pMember);
						if ( mesh->HasOption( SX_MESH_CASTSHADOW ) )
						{
							mesh->DrawByViewParam(SX_DRAW_MESH | SX_SHADER_ZDEPTH, viewParam);
							mesh->DrawByViewParam(SX_DRAW_MESH | SX_SHADER_ZDEPTH | SX_DRAW_ALPHA, viewParam);
						}
					}
					break;

				case NMT_PARTICLE:
					{
// 						OBBox box;
// 						if ( (!member->MsgProc( MT_GETBOX_WORLD, &box )) && box.IntersectFrustum(camFrustum ) )
// 						{
// 							member->DrawByViewParam( flag, dis );
// 						}
					}
					break;
				}
			}
		}

		for (int i=0; i<node->GetChildCount(); i++)
		{
			DrawNodeToShadowMap( node->GetChildByIndex(i), viewParam );
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//  SHADOW DIRECTIONAL
	//////////////////////////////////////////////////////////////////////////
	Shadow_direction::Shadow_direction( void ): m_Width(0)
	{
	}

	Shadow_direction::~Shadow_direction( void )
	{
		DestroyResources();
	}

	FORCEINLINE int Shadow_direction::GeShadowSize( void )
	{
		return m_Width;
	}

	void Shadow_direction::CreateResources( int shadowSize )
	{
		if (m_Width == shadowSize) return;
		m_Width = shadowSize;

		if ( !m_texture_depth.CreateTextureEx(D3DRTYPE_TEXTURE, shadowSize, shadowSize, 1, D3DUSAGE_DEPTHSTENCIL, D3DFMT_D24S8, D3DPOOL_DEFAULT) )
		{
			DestroyResources();
			return;
		}

		if ( !m_texture_color.CreateRenderTarget(D3DRTYPE_TEXTURE, shadowSize, shadowSize, D3DFMT_X8R8G8B8) )
		{
			DestroyResources();
			return;
		}
	}

	void Shadow_direction::DestroyResources( void )
	{
		m_Width = 0;
		m_texture_depth.Cleanup();
		m_texture_color.Cleanup();
	}

	void Shadow_direction::ComputeLightFrustum( OUT Frustum& frustum, Camera& sceneCamera, float3& lightDir, float shadowRange )
	{
		//  avoid UP direction ambiguities
		if ( abs(lightDir.x) < 0.001f && abs(lightDir.z) < 0.001f )
		{
			lightDir.x = 0.001f;
			lightDir.z = 0.001f;
		}

 		//  normalize light direction
 		lightDir.Normalize(lightDir);
 
 		// theta angle of the scene camera
  		float camTheta = 0;
  		sceneCamera.GetSpherical(0, 0, &camTheta);
		camTheta = int( camTheta * 5 ) / 5.0f;
 		SEGAN_CLAMP(camTheta, 0.4f, math::PIDIV2);
		float sinTheta = sinf(camTheta) * 0.9f;

		//  compute frustum width and frustum height depend on light direction
		float camHeight = int( sceneCamera.Eye.y / 5 ) * 5.0f;
		if ( camHeight < 5 ) camHeight = 5;

		float orthoSize = sceneCamera.FOV * camHeight * 2.0f + shadowRange * sinTheta;
		m_matProj.OrthoLH( orthoSize, orthoSize, -LIGHT_FAR_MAX, LIGHT_FAR_MAX );
		
 		// calculate light position depend on camera position on XZ plane
 		float3 cen( sceneCamera.Eye.x - sceneCamera.At.x, 0.0f, sceneCamera.Eye.z - sceneCamera.At.z );
 		cen.Normalize( cen );
 		cen *= 0.5f * shadowRange * sinTheta;
 		
 		//  set light camera properties
 		float3 At( sceneCamera.Eye.x - cen.x, 0.0f, sceneCamera.Eye.z - cen.z );
		float3 Eye( At.x - lightDir.x, At.y - lightDir.y, At.z - lightDir.z	);
		m_matView.LookAtLH( Eye, At, math::VEC3_Y );

		//  return frustum
		Matrix matLight;
		matLight.Multiply( m_matView, m_matProj );
 		frustum.ComputeByMatrix( matLight );
	}

	void Shadow_direction::RenderToShadowMaps( ArrayPNode_abs& nodes )
	{
		//  render nodes to the shadow map
		if ( !nodes.Count() ) return;

 		//  correct near/far planes of sun light camera depend on nodes
 		Matrix matInvers; matInvers.Inverse( m_matView );
 		float3 lightDir( m_matView._13, m_matView._23, m_matView._33 );		lightDir.Normalize( lightDir );
 		float3 lightPos( matInvers._41, matInvers._42, matInvers._43 );
 
		static ArrayPNodeMember meshList(256);
		float farMax = -FLT_MAX;
		float farMin = FLT_MAX;
		for (int i=0; i<nodes.Count(); i++)
		{
			meshList.Clear();
			nodes[i]->GetMembersByType( NMT_MESH, meshList, true );
			for (int meshIndex=0; meshIndex < meshList.Count(); meshIndex++)
			{
				OBBox box;
				meshList[meshIndex]->MsgProc( MT_GETBOX_WORLD, &box );
				for (int k=0; k<8; k++)
				{
					float d = lightDir.Dot( box.v[k] );
					if ( d > farMax ) farMax = d;
					if ( d < farMin ) farMin = d;
				}	
			}
		}
		float d = lightDir.Dot( lightPos );
		farMin -= d; 
		farMax -= d;
		float orthoSize = 2.0f / m_matProj._11;
		m_matProj.OrthoLH( orthoSize, orthoSize, farMin, farMax);

		//  correct projection matrix to avoid shimmering edges
		Matrix matLight;
		matLight.Multiply( m_matView, m_matProj );
		float3 Pos(0, 0, 0);
		Pos.Transform( Pos, matLight );
		Pos.x *= m_Width/2;
		Pos.y *= m_Width/2;
		float dx = floorf(Pos.x) - Pos.x;
		float dy = floorf(Pos.y) - Pos.y;
		dx /= m_Width/2;
		dy /= m_Width/2;
		Matrix matShimmer;
		matShimmer.Translation(dx, dy, 0);
		m_matProj.Multiply( m_matProj, matShimmer );

		//  save old states
		D3DViewport	vp_old = *( d3d::Device3D::Viewport() );
		DWORD colorWriteEnable = 0;
		PDirect3DSurface rt_old;
		PDirect3DSurface ds_old;
		Matrix matView_old, matProj_old;
		d3d::Device3D::GetDepthStencil(ds_old);
		d3d::Device3D::GetRenderTarget(0, rt_old);
		d3d::Device3D::Matrix_View_Get(matView_old);
		d3d::Device3D::Matrix_Project_Get(matProj_old);
		d3d::Device3D::RS_Get(D3DRS_COLORWRITEENABLE, colorWriteEnable);

		//  set light camera to the device
		d3d::Device3D::Matrix_View_Set( m_matView );
		d3d::Device3D::Matrix_Project_Set( m_matProj );
		d3d::ShaderPool::Update(0);

		//  set new states
		float slope = -0.1f;
		d3d::Device3D::RS_Set( D3DRS_COLORWRITEENABLE, 0 );
		d3d::Device3D::RS_Set( D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)(&slope) );
		m_texture_color.SetAsRenderTarget(0);
		m_texture_depth.SetAsDepthStencil();

		D3DViewport vp; ZeroMemory(&vp, sizeof(D3DViewport));
		vp.Width = m_Width; vp.Height = m_Width;
		vp.MinZ = 0.0f, vp.MaxZ = 1.0f;
		d3d::Device3D::Viewport_Set(&vp);
		
		d3d::Device3D::Clear_ZBuffer();
		
		//  render meshes to the main shadow map
		Frustum camFrustum;
		Renderer::GetCamera()->GetFrustum( camFrustum );
		for (int i=0; i<nodes.Count(); i++)
		{
			PNode node = nodes[i];

			// compute view parameter use to set LOD of the objects
			float dis = cmn::ViewDistanceByFrustom(camFrustum, Renderer::GetCamera()->FOV, node->GetPosition_world(), node->GetSphere_local().r);
			DrawNodeToShadowMap(node, dis * 1.5f);
		}

		//  restore old states
		d3d::Device3D::SetDepthStencil(ds_old);
		d3d::Device3D::SetRenderTarget(0, rt_old);
		d3d::Device3D::Matrix_View_Set(matView_old);
		d3d::Device3D::Matrix_Project_Set(matProj_old);
		d3d::Device3D::RS_Set(D3DRS_COLORWRITEENABLE, colorWriteEnable);

		slope = 0.0f;
		d3d::Device3D::RS_Set( D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)(&slope) );
		
		d3d::Device3D::Viewport_Set(&vp_old);
		d3d::ShaderPool::Update(0);
	}

	void Shadow_direction::SetShadowMapsToDevice( void )
	{
		if ( m_Width == 0 ) return;

		//  set sun light data to the shader pool
		PMatrix sunLight = Renderer::GetSunLight();

 		//  compute texel size
 		sunLight->_44 = m_Width>0 ? 1.0f/(float)m_Width : 0.01f;

		// set light matrix to the shader pool
		Matrix matLight, matViewInv;
		matLight.Multiply( m_matView, m_matProj );

		//  apply bias matrix
		float zBias = - 0.25f * abs( m_matProj._33 );
		switch (m_Width)
		{
		case 1024:	zBias *= 2.25;	break;
		case 512:	zBias *= 2.75f;	break;
		case 256:	zBias *= 3;		break;
		}
//		zBias *= 0.2f;
		float offsetX = 0.5f + (0.5f / m_Width);
		float offsetY = 0.5f + (0.5f / m_Width);
		Matrix matBias( 
			0.5f,		0.0f,		0.0f,	0.0f,
			0.0f,		-0.5f,		0.0f,	0.0f,
			0.0f,		0.0f,		1.0f,	0.0f,
			offsetX,	offsetY,	zBias,	1.0f );

		matLight.Multiply( matLight, matBias );

		d3d::Device3D::Matrix_View_Get( matViewInv );
		matViewInv.Inverse( matViewInv );
		matLight.Multiply( matViewInv, matLight );

		d3d::ShaderPool::SetLight( sunLight, &matLight);
		m_texture_depth.SetToDevice(7);

// 		d3d::Device3D::Matrix_View_Set( m_matView );
// 		d3d::Device3D::Matrix_Project_Set( m_matProj );
// 		d3d::ShaderPool::Update();
	}

}}//namespace sx { namespace core {
