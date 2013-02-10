#include "sxPipeline_deferred.h"
#include "sxShaderStore.h"
#include "sxSettings.h"
#include "sxCamera.h"
#include "sxNode.h"
#include "sxMesh.h"
#include "sxRenderer.h"
#include "sxScene.h"

namespace sx { namespace core {


	Pipeline_Deferred::Pipeline_Deferred( void ) : Pipeline()
	{

	}

	Pipeline_Deferred::~Pipeline_Deferred( void )
	{

	}

	void Pipeline_Deferred::CreateResources( int width, int height )
	{
		if (sx::d3d::Texture::Manager::Create( m_Graphc, NULL ))
			m_Graphc->CreateRenderTarget(D3DRTYPE_TEXTURE, width, height, D3DFMT_A8R8G8B8);

		if (sx::d3d::Texture::Manager::Create( m_Normal, NULL ))
			m_Normal->CreateRenderTarget(D3DRTYPE_TEXTURE, width, height, D3DFMT_G16R16F);

		if (sx::d3d::Texture::Manager::Create( m_ZDepth, NULL ))
			m_ZDepth->CreateTextureEx( D3DRTYPE_TEXTURE, width, height, 1, D3DUSAGE_DEPTHSTENCIL, D3DFMT_D24X8, D3DPOOL_DEFAULT );

		if (sx::d3d::Texture::Manager::Create( m_Scene, NULL ))
			m_Scene->CreateRenderTarget(D3DRTYPE_TEXTURE, width, height, D3DFMT_A8R8G8B8);

		if ( !m_postPresent.Exist() )
			m_postPresent.CompileShader( sx::core::ShaderStore::GetPost_Present(L"return float4( tex2D( samp0, tex0 ).rgb, 1.0f );"), SQ_HIGH, L"post present shader");

		if ( !m_postShadeScene.Exist() )
			m_postShadeScene.CompileShader( sx::core::ShaderStore::GetPost_ShadeScene(), SQ_HIGH, L"post shade scene shader");
	}

	void Pipeline_Deferred::DestroyResources( void )
	{
		sx::d3d::Texture::Manager::Release(m_Scene);
		sx::d3d::Texture::Manager::Release(m_Graphc);
		sx::d3d::Texture::Manager::Release(m_Normal);
		sx::d3d::Texture::Manager::Release(m_ZDepth);
		//sx::d3d::Texture::Manager::Release(m_Shadow);
	}

	void Pipeline_Deferred::Update( float elpTime )
	{

	}

	void Pipeline_Deferred::RenderScene( DWORD flag )
	{

	}

	//////////////////////////////////////////////////////////////////////////
	//  additional functions
	//////////////////////////////////////////////////////////////////////////
	void Pipeline_Deferred::def_RenderNodes( ArrayPNode_abs& nodeList, DWORD flag )
	{
		// extract meshes from nodes
		ArrayPNodeMember meshList;
		for (int i=0; i<nodeList.Count(); i++)
			nodeList[i]->GetMembersByType(NMT_MESH, meshList, true);

		//  render the meshes
		for (int i=0; i<meshList.Count(); i++)
		{
			PMesh mesh = (PMesh)meshList[i];
			d3d::PGeometry gmtry = mesh->m_Geometry;
			d3d::PMaterial mtrl = mesh->m_Material[ mesh->m_Material.GetActiveMaterialIndex() ];

			if (gmtry)
			{
				//  the actual view distance from camera to target is ' disCameraToTarget = disEyeToAt * (1 - cos(FOV)) '
				//  view distance is different with distance of camera and target. decreasing FOV will decrease view distance
				//  to compute distance between camera and objects we can easily replace target position by object position
				//  the other important parameter is the object's volume. huge objects can be seen from far distance :)
				float cosFOV = cos( Renderer::GetCamera()->FOV );
				float3 v = mesh->m_Owner->GetSphere_world().center - Renderer::GetCamera()->Eye;
				float disSqr = ( v.x*v.x + v.y*v.y + v.z*v.z - mesh->m_Box.GetVolume() ) * cosFOV * cosFOV;

				if (disSqr < 1000000)
				{
					int lod = mesh->ComputeLodValue( disSqr );
					mesh->Validate(lod);
					lod = gmtry->SetToDevice(lod);
					if (lod >= 0)
					{
						d3d::Device3D::Matrix_World_Set( mesh->m_Owner->GetMatrix_world() );
						d3d::Device3D::SetMaterialColor(0xffffffff);

						mtrl->SetToDevice( flag );
						d3d::Device3D::DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, gmtry->GetVertexCount(lod), 0, gmtry->GetFaceCount(lod));
					}
				}
			}

		}
	}

	void Pipeline_Deferred::def_RenderPointLights( ArrayPNode_abs& nodeList, DWORD flag )
	{

	}

	void Pipeline_Deferred::def_RenderToShadowMap( ArrayPNode_abs& nodeList, DWORD flag )
	{

	}

	void Pipeline_Deferred::def_RenderToGBuffer( ArrayPNode_abs& nodeList, DWORD flag )
	{
		// verify that device supports Simultaneous Render Target 
		if ( d3d::Device3D::GetCaps()->NumSimultaneousRTs > 2 )
		{
			m_Graphc->SetAsRenderTarget(0);
			m_ZDepth->SetAsRenderTarget(1);
			m_Normal->SetAsRenderTarget(2);

			d3d::Device3D::Clear_Screen(0x00000000);
			def_RenderNodes( nodeList, SX_SHADER_DEFERRED );

			d3d::Device3D::SetRenderTarget(1, NULL);
			d3d::Device3D::SetRenderTarget(2, NULL);
		}
		else // the device DO NOT supports Simultaneous Render Target 
		{
			// TODO : it's a fault!
		}
	}

	void Pipeline_Deferred::def_Render( ArrayPNode_abs& nodeList, DWORD flag )
	{
		//  render scene to graphic textures
		d3d::Device3D::RS_ZEnabled(true);
		def_RenderToGBuffer( nodeList, flag );
		d3d::Device3D::RS_ZEnabled(false);

		if ( Renderer::GetRenderTarget() )
		{
			Renderer::GetRenderTarget()->SetAsRenderTarget(0);
			m_Graphc->SetToDevice(0);
			m_Normal->SetToDevice(1);
			m_postShadeScene.SetToDevice();
			d3d::UI3D::DrawRectangle(
				float3(-1.0f, 1.0f, 0.0f ), float3( 1.0f, 1.0f, 0.0f ),
				float3(-1.0f,-1.0f, 0.0f ), float3( 1.0f,-1.0f, 0.0f ));
		}
		else
		{
			//  blend texture together
			m_Scene->SetAsRenderTarget(0);
			m_Graphc->SetToDevice(0);
			m_Normal->SetToDevice(1);
			m_postShadeScene.SetToDevice();
			d3d::UI3D::DrawRectangle(
				float3(-1.0f, 1.0f, 0.0f ), float3( 1.0f, 1.0f, 0.0f ),
				float3(-1.0f,-1.0f, 0.0f ), float3( 1.0f,-1.0f, 0.0f ));

			//  set main render target
			Renderer::SetRenderTarget( Renderer::GetRenderTarget() );
			if ( !m_mainRT ) d3d::Device3D::Viewport_SetDefault();

			//  present scene texture
			m_postPresent.SetToDevice();
			m_Scene->SetToDevice(0);
			d3d::UI3D::DrawRectangle(
				float3(-1.0f, 1.0f, 0.0f ), float3( 1.0f, 1.0f, 0.0f ),
				float3(-1.0f,-1.0f, 0.0f ), float3( 1.0f,-1.0f, 0.0f ));
		}
	}


}} // namespace sx { namespace core {
