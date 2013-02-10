#include "sxRenderer.h"
#include "sxScene.h"
#include "sxMesh.h"
#include "sxSettings.h"
#include "sxShaderStore.h"
#include "sxPipeline_forward.h"

using namespace sx::core;

//  these of positions of validation spheres use to validation process
#define IV_MAXRANGE		7000.0f		//  invalidation maximum range
#define IV_RANGE		9			//  invalidation range
#define V_COUNT			100			//  number of validation positions

static const UINT renderFileID = MAKEFOURCC('R', 'N', 'D', 'R');

//  if you want to change these constants, you should match them 
//  with the other constants in validation/invalidation process in Update()/RenderScene()
Renderer::ValidationParam	s_validationParam = { 2000.0f, 1000.0f, 0.001f };

static const float3 s_validationPos[V_COUNT] = {
	float3( 0, 0, 0),
	float3( 1, 0, 1),
	float3(-1, 0, 1),
	float3( 1, 0,-1),
	float3(-1, 0,-1),
	float3( 2, 0, 0),
	float3(-2, 0, 0),
	float3( 0, 0, 2),
	float3( 0, 0,-2),
	float3( 2, 0, 2),
	float3(-2, 0, 2),
	float3( 2, 0,-2),
	float3(-2, 0,-2),
	float3( 3, 0, 1),
	float3(-3, 0, 1),
	float3( 3, 0,-1),
	float3(-3, 0,-1),
	float3( 1, 0, 3),
	float3(-1, 0, 3),
	float3( 1, 0,-3),
	float3(-1, 0,-3),
	float3( 4, 0, 2),
	float3(-4, 0, 2),
	float3( 4, 0,-2),
	float3(-4, 0,-2),
	float3( 3, 0, 3),
	float3(-3, 0, 3),
	float3( 3, 0,-3),
	float3(-3, 0,-3),
	float3( 2, 0, 4),
	float3(-2, 0, 4),
	float3( 2, 0,-4),
	float3(-2, 0,-4),
	float3( 4, 0, 4),
	float3(-4, 0, 4),
	float3( 4, 0,-4),
	float3(-4, 0,-4),
	float3( 1, 1, 0),
	float3(-1, 1, 0),
	float3( 0, 1, 1),
	float3( 0, 1,-1),
	float3( 4, 0, 0),
	float3(-4, 0, 0),
	float3( 0, 0, 4),
	float3( 0, 0,-4),
	float3( 1, 1, 2),
	float3(-1, 1, 2),
	float3( 1, 1,-2),
	float3(-1, 1,-2),
	float3( 2, 1, 1),
	float3(-2, 1, 1),
	float3( 2, 1,-1),
	float3(-2, 1,-1),
	float3( 3, 1, 0),
	float3(-3, 1, 0),
	float3( 0, 1, 3),
	float3( 0, 1,-3),
	float3( 1,-1, 0),
	float3(-1,-1, 0),
	float3( 0,-1, 1),
	float3( 0,-1,-1),
	float3( 1,-1, 2),
	float3(-1,-1, 2),
	float3( 1,-1,-2),
	float3(-1,-1,-2),
	float3( 2,-1, 1),
	float3(-2,-1, 1),
	float3( 2,-1,-1),
	float3(-2,-1,-1),
	float3( 3,-1, 0),
	float3(-3,-1, 0),
	float3( 0,-1, 3),
	float3( 0,-1,-3),
	float3( 0, 2, 0),
	float3( 1, 2, 1),
	float3(-1, 2, 1),
	float3( 1, 2,-1),
	float3(-1, 2,-1),
	float3( 2, 2, 0),
	float3(-2, 2, 0),
	float3( 0, 2, 2),
	float3( 0, 2,-2),
	float3( 1, 3, 0),
	float3(-1, 3, 0),
	float3( 0, 3, 1),
	float3( 0, 3,-1),
	float3( 0,-2, 0),
	float3( 1,-2, 1),
	float3(-1,-2, 1),
	float3( 1,-2,-1),
	float3(-1,-2,-1),
	float3( 2,-2, 0),
	float3(-2,-2, 0),
	float3( 0,-2, 2),
	float3( 0,-2,-2),
	float3( 1,-3, 0),
	float3(-1,-3, 0),
	float3( 0,-3, 1),
	float3( 0,-3,-1),
	float3( 0, 4, 0)
};


//////////////////////////////////////////////////////////////////////////
//  internal static classes hold necessary resources
static	ArrayPNode				nodesValidation(128);
static	ArrayPNode				nodesInvalidation(128);
static	Camera					s_Camera;
static  sx::d3d::PTexture		s_sceneRT	= NULL;
static	sx::core::PPipeline		s_Pipeline	= NULL;
static	Matrix					s_sunLight( -0.4f,-0.7f, 0.2f, 1.0f,
											 1.0f, 0.9f, 0.8f, 1.0f,
											 0.3f, 0.3f, 0.4f, 1.0f,
											 0.0f, 0.0f, 0.0f, 0.0f);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
namespace sx { namespace core {


	void Renderer::Initialize( PPipeline pipeline )
	{
		if ( s_Pipeline )
		{
			s_Pipeline->DestroyResources();
			sx_delete_and_null( s_Pipeline );
		}
		s_Pipeline = pipeline;
	}

	void Renderer::Finalize( void )
	{
		if ( s_Pipeline )
		{
			s_Pipeline->DestroyResources();
			sx_delete_and_null( s_Pipeline );
		}

		//  delete shader pool object
		d3d::ShaderPool::Finalize();
	}

	void Renderer::SetSize( HWND Display, UINT Width, UINT Height, SX_D3D_ DWORD Flag )
	{
		sx_callstack_push(Renderer::SetSize(Width=%d, Height=%d), Width, Height);

		static bool Creating = false;
		static DWORD d3dFlag = 0;
		if (Creating) return;

		if ( d3d::Device3D::IsCreated() )
		{
			d3d::Device3D::SetRenderTarget(0, NULL);

			if (Flag!=0 && Flag != d3dFlag)
			{
				Creating = true;
				d3d::Device3D::Reset(Width, Height, Flag);
				Creating = false;
			}
			else
			{
				d3d::Device3D::Display_SetHandle(Display);
				d3d::Device3D::Display_Resized();
			}

		}
		else
		{
			Creating = true;
			sx::d3d::Device3D::Create(Display, Width, Height, Flag);
			sx::d3d::ShaderPool::Initialize();
			Creating = false;
		}
		d3dFlag = Flag;

		d3d::Device3D::Display_Resized();
		d3d::Device3D::Viewport_SetDefault();

		if ( !s_Pipeline )
		{

			sxLog::Log(L"ERROR : Rendering pipeline is not exist to render the scene !");
			return;
		}

		s_Pipeline->DestroyResources();
		s_Pipeline->CreateResources( d3d::Device3D::Viewport()->Width, d3d::Device3D::Viewport()->Height );

	}

	bool Renderer::CanRender( void )
	{
		return sx::d3d::Device3D::CheckHealth(NULL, NULL);
	}

	void Renderer::SetRenderTarget( d3d::PTexture target )
	{
		sx_callstack_push(Renderer::SetRenderTarget());

		s_sceneRT = target;

		if ( target && target->GetD3DTexture() )
		{
			target->SetToDevice(6);
			PDirect3DSurface pSurf = NULL;
			PDirect3DTexture pTxur = (PDirect3DTexture)target->GetD3DTexture();
			pTxur->GetSurfaceLevel(0, &pSurf);
			sx::d3d::Device3D::SetRenderTarget(0, pSurf);
			SEGAN_RELEASE_AND_NULL(pSurf);
		}
		else sx::d3d::Device3D::SetRenderTarget(0, NULL);
	}

	d3d::PTexture Renderer::GetRenderTarget( void )
	{
		return s_sceneRT;
	}

	void Renderer::Begin( void )
	{
		sx_callstack_push(Renderer::Begin());

		//  init device to default
		d3d::Device3D::InitStates();

		d3d::Device3D::Scene_Begin();
		d3d::Device3D::Viewport_SetDefault();
	}

	void Renderer::End( void )
	{
		sx_callstack_push(Renderer::End());

		d3d::Device3D::Scene_End();
		d3d::Device3D::Scene_Present();
	}

	void Renderer::SetCamera( const PCamera camera )
	{
		if (camera)
			s_Camera = *camera;

		// update camera
		s_Camera.SetToDevice();
	}

	sx::core::PCamera Renderer::GetCamera( void )
	{
		return &s_Camera;
	}

	void Renderer::SetSunLight( PMatrix sunLight )
	{
		if (sunLight)
			s_sunLight = *sunLight;
	}

	PMatrix Renderer::GetSunLight( void )
	{
		return &s_sunLight;
	}

	sx::core::PPipeline Renderer::GetRenderingPipeline( void )
	{
		return s_Pipeline;
	}

	//  collect objects and validate them
	void Renderer::Update( float elpTime )
	{
		sx::d3d::ShaderPool::Update(elpTime);

		static int curPos = 0;
		static int curStep = 0;
		static float4 curCam(math::VEC4_MAX);

		//  check to see if camera position is changed, restart validation process
		if (abs(s_Camera.Eye.x - curCam.x) > s_validationParam.camPosThreshold	|| 
			abs(s_Camera.Eye.y - curCam.y) > s_validationParam.camPosThreshold	|| 
			abs(s_Camera.Eye.z - curCam.z) > s_validationParam.camPosThreshold	|| 
			abs(s_Camera.FOV - curCam.w)>0.1f				||
			curPos > V_COUNT								)
		{
			curCam.x = s_Camera.Eye.x;
			curCam.y = s_Camera.Eye.y;
			curCam.z = s_Camera.Eye.z;
			curCam.w = s_Camera.FOV;

			curPos = 0;
			curStep = 0;
		}

		//  validate objects in the validation position and validation range
		if (curPos < V_COUNT)
		{
			curStep++;

			switch (curStep)
			{
			case 1:	//  at first step collect objects from scene in specified position
				{
					nodesValidation.Clear();
					Scene::GetNodesByArea( 
						s_validationPos[curPos] * s_validationParam.validationRadius + s_Camera.Eye, 
						s_validationParam.validationRadius,
						nodesValidation );
				}
				break;

			case 2:	//  at the second step traverse the list to validate nodes
				{
					float3 v; 
					float dis;
					Frustum frust;
					s_Camera.GetFrustum( frust );

					PNode node = NULL;
					int n = nodesValidation.Count();
					for (int i=0; i<n; i++)
					{
						node = nodesValidation.At(i);

						//  compute view distance
						dis = cmn::ViewDistanceByFrustom(frust, s_Camera.FOV, node->GetPosition_world(), node->GetSphere_local().r);

						//  validation is different to show LOD of objects
						dis *= s_validationParam.validationCoefficient;
						node->MsgProc(MT_ACTIVATE, &dis);
					}

					curPos++;
					curStep = 0;
				}
				break;

			}
		}

		static float updateTime = 0;
		updateTime += elpTime;
		if ( updateTime > 32 )
		{
			static Array<PNode> nodesUpdate(128);
			nodesUpdate.Clear();
			Scene::GetNodesByArea( s_Camera.Eye, s_validationParam.validationRadius, nodesUpdate );
			
			Frustum camFrustum;
			s_Camera.GetFrustum( camFrustum );

			PNode node = NULL;
			int n = nodesUpdate.Count();
			for (int i=0; i<n; i++)
			{
				node = nodesUpdate[i];

				//  check to see if sphere of node is fully out side of the frustum
				bool isOut = false;
				for (int i=0; i<6; i++)
				{
					if ( camFrustum.p[i].Distance( node->GetSphere_world().center ) < -node->GetSphere_local().r ) 
					{
						isOut = true;
						break;
					}
				}

				if ( isOut )
				{
					node->Update(updateTime);
				}
				else
				{
					// check the visibility via intersect between box and frustum
					if ( ! node->GetBox_world().IntersectFrustum(camFrustum) )
						node->Update(updateTime);
				}
			}
			updateTime = 0;
		}

		//  update pipeline to prepare rendering
		if ( s_Pipeline )
		{
			s_Pipeline->Update(elpTime);
		}
		else
		{
			sxLog::Log(L"ERROR : Rendering pipeline is not exist to render the scene !");
			Sleep(100);
		}
	}

	void Renderer::RenderScene( DWORD flag )
	{
		sx_callstack_push(Renderer::RenderScene());

		// update shader pool
		sx::d3d::ShaderPool::SetLight( &s_sunLight );
		sx::d3d::ShaderPool::Update(0);

		//  render the scene
		if ( s_Pipeline )
		{
			s_Pipeline->RenderScene(flag);
		}
		else
		{
			sxLog::Log(L"ERROR : Rendering pipeline is not exist to render the scene !");
			Sleep(100);
		}

		//  invalidate objects out of range
		static int curStep = 0;
		static float curAngle = 0.0f;
		static float curRadius = IV_RANGE * s_validationParam.validationRadius;
		static float3 curCam(math::VEC3_MAX);

		//  check to see if camera position is changed, restart invalidation process
		if (abs(s_Camera.Eye.x - curCam.x) > s_validationParam.camPosThreshold * 2.0f || 
			abs(s_Camera.Eye.y - curCam.y) > s_validationParam.camPosThreshold * 2.0f || 
			abs(s_Camera.Eye.z - curCam.z) > s_validationParam.camPosThreshold * 2.0f ||
			curRadius > IV_MAXRANGE								   )
		{
			curCam.x = s_Camera.Eye.x;
			curCam.y = s_Camera.Eye.y;
			curCam.z = s_Camera.Eye.z;
			curRadius = IV_RANGE * s_validationParam.validationRadius;
		}

		//  invalidate objects in the invalidation position and range
		if (curRadius < IV_MAXRANGE)
		{
			curStep++;

			switch (curStep)
			{
			case 1:	//  at first step collect objects from scene in specified position
				{
					nodesInvalidation.Clear();
					float3 invalidPos( s_Camera.Eye.x + curRadius * cos(curAngle), s_Camera.Eye.y, s_Camera.Eye.z + curRadius * sin(curAngle) );
					Scene::GetNodesByArea( invalidPos, s_validationParam.validationRadius, nodesInvalidation );
					//sx::d3d::UI3D::DrawSphere( Sphere(invalidPos, validationThreshold), 0xffff0000 );
				}
				break;

			case 2:	//  at the second step traverse the list to invalidate nodes
				{
					PNode node = NULL;
					for (int i=0; i<nodesInvalidation.Count(); i++)
					{
						node = nodesInvalidation[i];
						node->MsgProc(MT_DEACTIVATE, NULL);
					}

					curAngle += ( PI / ( curRadius/s_validationParam.validationRadius * 4.0f ) );
					if ( curAngle >= math::PIMUL2 )
					{
						curAngle = 0.0f;
						curRadius += s_validationParam.validationRadius;
					}

					curStep = 0;
				}
				break;

			}
		}

	}

	UINT Renderer::MsgProc( MT_ UINT msgType, void* data )
	{
		sx_callstack_push(Renderer::MsgProc());

		switch (msgType)
		{
		case MT_SCENE_ADD_NODE:
			{
				PNode node = static_cast<PNode>(data);
				
				float rad = node->GetSphere_local().r;		
				float v_x = node->GetMatrix_world()._41 - s_Camera.Eye.x;
				float v_y = node->GetMatrix_world()._42 - s_Camera.Eye.y;
				float v_z = node->GetMatrix_world()._43 - s_Camera.Eye.z;
				float dis = ( v_x*v_x + v_y*v_y + v_z*v_z );

				//  validation is different to show LOD of objects
				if ( dis < 12250000 )
				{
					dis = cmn::ViewDistanceByCamera(s_Camera.Eye, s_Camera.At, s_Camera.FOV, node->GetPosition_world(), node->GetSphere_local().r);
					dis *= s_validationParam.validationCoefficient;
					node->MsgProc(MT_ACTIVATE, &dis);
				}

				node->Update(0);
			}
			break;

		case MT_NODE_DELETE:
		case MT_SCENE_REMOVE_NODE:
			{
				PNode node = static_cast<PNode>(data);
				nodesValidation.Remove(node);
				nodesInvalidation.Remove(node);
			}
			break;

		case MT_SCENE_CLEAN:
			{
				nodesValidation.Clear();
				nodesInvalidation.Clear();
			}
			break;
		}

		if ( s_Pipeline )
			return s_Pipeline->MsgProc(msgType, data);
		else
			return msgType;
	}

	void Renderer::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, renderFileID);

		int version = 4;
		SEGAN_STREAM_WRITE(stream, version);

		//  save camera
		s_Camera.Save( stream );

		//  save light properties
		float2 l = Settings::GetSunLightPosition();
		SEGAN_STREAM_WRITE(stream, l);

		//  save fog properties
		FogDesc fog;
		d3d::Device3D::GetFogDesc( fog );
		SEGAN_STREAM_WRITE(stream, fog);

		//	save reflection settings
		DWORD refOption = Settings::GetOption_Reflection()->value;
		SEGAN_STREAM_WRITE(stream, refOption);

		//	save shadow settings
		DWORD shdOption = Settings::GetOption_Shadow()->value;
		SEGAN_STREAM_WRITE( stream, shdOption );
	}

	void Renderer::Load( Stream& stream )
	{
		sx_callstack_push(Renderer::Load());

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != renderFileID)
		{
			sxLog::Log(L"Incompatible file format for loading renderer properties !");
			return;
		}

		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if ( version == 1 )
		{
			//  load camera
			s_Camera.Load( stream );

			//  load light properties
			float2 l = Settings::GetSunLightPosition();
			SEGAN_STREAM_READ(stream, l);
			Settings::SetSunLightPosition( l.x, l.y );
		}

		else if ( version == 2 )
		{
			//  load camera
			s_Camera.Load( stream );

			//  load light properties
			float2 l = Settings::GetSunLightPosition();
			SEGAN_STREAM_READ(stream, l);
			Settings::SetSunLightPosition( l.x, l.y );

			//  save fog properties
			FogDesc fog;
			SEGAN_STREAM_READ(stream, fog);
			d3d::Device3D::SetFogDesc( fog );
		}

		else if ( version == 3 )
		{
			//  load camera
			s_Camera.Load( stream );

			//  load light properties
			float2 l = Settings::GetSunLightPosition();
			SEGAN_STREAM_READ(stream, l);
			Settings::SetSunLightPosition( l.x, l.y );

			//  save fog properties
			FogDesc fog;
			SEGAN_STREAM_READ(stream, fog);
			d3d::Device3D::SetFogDesc( fog );

			//	load reflection settings
			DWORD refOption = 0;
			SEGAN_STREAM_READ(stream, refOption);
			if ( refOption & OPT_BY_ARTIST )
				Settings::GetOption_Reflection()->AddPermission( OPT_BY_ARTIST );
			else
				Settings::GetOption_Reflection()->RemPermission( OPT_BY_ARTIST );
		}

		else if ( version == 4 )
		{
			//  load camera
			s_Camera.Load( stream );

			//  load light properties
			float2 l = Settings::GetSunLightPosition();
			SEGAN_STREAM_READ(stream, l);
			Settings::SetSunLightPosition( l.x, l.y );

			//  save fog properties
			FogDesc fog;
			SEGAN_STREAM_READ(stream, fog);
			d3d::Device3D::SetFogDesc( fog );

			//	load reflection settings
			DWORD refOption = 0;
			SEGAN_STREAM_READ(stream, refOption);
			if ( refOption & OPT_BY_ARTIST )
				Settings::GetOption_Reflection()->AddPermission( OPT_BY_ARTIST );
			else
				Settings::GetOption_Reflection()->RemPermission( OPT_BY_ARTIST );

			//	load shadow settings
			DWORD shdOption = 0;
			SEGAN_STREAM_READ(stream, shdOption);
			if ( shdOption & OPT_BY_ARTIST )
				Settings::GetOption_Shadow()->AddPermission( OPT_BY_ARTIST );
			else
				Settings::GetOption_Shadow()->RemPermission( OPT_BY_ARTIST );
		}
	}

	Renderer::ValidationParam* Renderer::ValidationParameters( void )
	{
		return &s_validationParam;
	}

}} // namespace sx { namespace core {