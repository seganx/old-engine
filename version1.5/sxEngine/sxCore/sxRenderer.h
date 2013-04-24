/********************************************************************
	created:	2011/04/01
	filename: 	sxRenderer.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the static renderer class.
*********************************************************************/
#ifndef GUARD_sxRenderer_HEADER_FILE
#define GUARD_sxRenderer_HEADER_FILE

#include "sxCore_def.h"
#include "sxPipeline.h"
#include "sxCamera.h"
#include "sxNode.h"

namespace sx { namespace core {

	//! use this class to render the scene in the game
	class SEGAN_API Renderer
	{
	public:
		struct ValidationParam
		{
			float camPosThreshold;			//	threshold of changing camera position
			float validationRadius;			//	radius of validation sphere
			float validationCoefficient;	//	validation coefficient. default is 1.0f
		};

	public:
		//! initialize the rendering system by given rendering pipeline object
		static void Initialize( PPipeline pipeline );

		//! delete pipeline object and finalize the rendering system.
		static void Finalize(void);

		//! set renderer size, initialize devices and needed resources
		static void SetSize(HWND Display, UINT Width = 0, UINT Height = 0, SX_D3D_ DWORD Flag = 0);

		//! return false if renderer can't render any thing
		static bool CanRender(void);

		//! set render target texture. pass NULL to set render target to default surface
		static void SetRenderTarget(d3d::PTexture target);

		//! return pointer to render target. return NULL if no render target exist
		static d3d::PTexture GetRenderTarget(void);

		//! prepare renderer device to ready to render the scene. return false if Begin() failed
		static void Begin(void);

		//! finish rendering and present the back buffer to the screen
		static void End(void);

		//! set current renderer camera
		static void SetCamera(const PCamera camera);

		//! return current camera
		static PCamera GetCamera(void);

		/*! 
		return sun light information of the scene. these information packed to the matrix include
		sunLight[0].xyz : sun light direction
		sunLight[1].xyz : sun light diffuse color
		sunLight[2].xyz : sun light ambient color
		sunLight[3].xyz : reserved
		*/
		static PMatrix GetSunLight( void );

		//! return reference to sun light diffuse. use w as intensity
		static float4& SunLightColor( void );

		//! return reference to sun light ambient. use w as intensity
		static float4& AmbientColor( void );

		//! return pointer to current rendering pipeline
		static PPipeline GetRenderingPipeline(void);

		//! update renderer
		static void Update(float elpTime);

		//! render the scene with determined features 
		static void RenderScene(DWORD flag);

		//! use to communicate by this system
		static UINT MsgProc(MT_ UINT msgType, void* data);

		//! save some additional information include camera / sun light properties
		static void Save(Stream& stream);

		//! load some additional information include camera / sun light properties
		static void Load(Stream& stream);

		//! return the validation structure
		static ValidationParam* ValidationParameters(void);

	};

} } // namespace sx { namespace core {

#endif	//	GUARD_sxRenderer_HEADER_FILE