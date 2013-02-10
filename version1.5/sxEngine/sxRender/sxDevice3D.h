/********************************************************************
	created:	2010/08/23
	filename: 	sxRender.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of Device3D which responsible 
				to manage render device to render the scene
*********************************************************************/
#ifndef GUARD_sxDevice3D_HEADER_FILE
#define GUARD_sxDevice3D_HEADER_FILE

#include "sxTypes3D.h"

#define SEGAN_VP_WIDTH		(static_cast<float>(sx::d3d::Device3D::Viewport()->Width))
#define SEGAN_VP_HEIGHT		(static_cast<float>(sx::d3d::Device3D::Viewport()->Height))

typedef void (*rnrCallback_OnDeviceLost)(void);
typedef void (*rnrCallback_OnDeviceReset)(void);

static const DWORD WM_SX_D3D_RESET = WM_USER + 101;

namespace sx { namespace d3d
{

	class SEGAN_API Device3D		
	{
	public:
		//! retrieve driver info
		static void GetDriverInfo(DisplayDriverInfo& dInfo);

		//! return Direct3D device
		static PDirect3DDevice GetDevice(void);

		//! return video capabilities
		static PD3DCaps	GetCaps(void);

		//! return the current FPS rate
		static int GetFPS(void);

		//! return monitor refresh rate
		static int GetAdapterRefreshRate(void);

		//! return number of draw calls
		static int GetNumberOfDrawCalls(void);

		//! return number of triangles have been drawn
		static int GetNumberOfTriangles(void);

		//! return number of vertices have been drawn
		static int GetNumberOfVertices(void);

		//! create a compatible rendering device
		static bool Create(HWND Display, UINT Width = 0, UINT Height = 0, SX_D3D_ DWORD Flag = 0);

		//! create depth stencil surface
		static HRESULT CreateDepthStencil(IN const UINT width, IN const UINT height, OUT PDirect3DSurface& outSurf);

		//! destroy compatible rendering device
		static void Destroy(void);

		//! return the creation flags
		static DWORD GetCreationFlag(void);

		//! reinitialize rendering device to default settings
		static void InitStates(void);

		//! check device health and reset it if needed
		static bool CheckHealth(rnrCallback_OnDeviceLost OnDeviceLost, rnrCallback_OnDeviceReset OnDeviceReset);

		//! reset the device
		static HRESULT Reset(UINT Width = 0, UINT Height = 0, SX_D3D_ DWORD Flag = 0);

		//! return true if device is created
		static bool IsCreated(void);

		//! return true if device is ready and not has been lost
		static bool IsReady(void);

		static void Clear_Screen(DWORD bgcolor);
		static void Clear_Target(DWORD bgcolor);
		static void Clear_ZBuffer(void);

		static bool Scene_Begin(void);
		static void Scene_End(void);
		static void Scene_Present(PRECT pDestRect = NULL);
		
		static void SetClipPlane(DWORD Index, const float* pPlane);
		static void GetClipPlane(DWORD Index, float* pPlane);

		static void SetMaterialColor(const D3DColor& color);
		static void SetVertexBuffer(BYTE strmIndex, PDirect3DVertexBuffer vrtxbuffr, UINT strd);
		static void SetIndexBuffer(PDirect3DIndexBuffer indxbuffr);
		static void SetTexture(UINT stage, PDirect3DBaseTexture txur);
		static void SetVertexShader(PDirect3DVertexShader vshader);
		static void SetPixelShader(PDirect3DPixelShader pshader);
		static void SetRenderTarget(UINT index, PDirect3DSurface surf);
		static void SetDepthStencil(PDirect3DSurface surf);
		static void SetEffect(PD3DXEffect effect);

		static void GetRenderTarget(UINT index, OUT PDirect3DSurface& surf);
		static void GetDepthStencil(OUT PDirect3DSurface& surf);

		static void DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
		static void DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
		static void DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride, DWORD fvf = 0);
		
		static void Matrix_World_Set(const Matrix& mat);
		static void Matrix_World_Get(Matrix& mat);
		static void Matrix_View_Set(const Matrix& mat);
		static void Matrix_View_Get(Matrix& mat);
		static void Matrix_Project_Set(const Matrix& mat);
		static void Matrix_Project_Get(Matrix& mat);

		static void Texture_ResetStateStages(void);
		static void Texture_SetSampleState(SamplerUV SUV_ samplerUV = SUV_WRAP);
		static UINT	Texture_GetMaxWidth(void);
		static UINT	Texture_GetMaxLevels(void);

		static void Viewport_SetDefault(void);
		static void Viewport_Set(const PD3DViewport vp);
		static PD3DViewport Viewport(void);

		static void Display_SetHandle(HWND vp_Hndl);
		static HWND Display_GetHandle(void);
		static void Display_Resized(void);

		static void Camera_Projection(float FOV, float Aspect = 0, float zNear = 0, float zFar = 0);
		static void Camera_Pos(const Vector& Eye, const Vector& At);
		static void Camera_Pos(const Vector& Eye, const Vector& At, const Vector& Up);

		static void RS_Set(D3DRENDERSTATETYPE State, DWORD Value);
		static void RS_Get(D3DRENDERSTATETYPE State, DWORD& Value);
		static void RS_Alpha( DWORD mode );
		static void RS_Lighting(bool value);
		static void RS_ZEnabled(bool value);
		static void RS_ZWritable(bool value);
		static void RS_Culling(bool value, bool backface = false);
		static void RS_VertexColor(bool value);
		static void RS_Specular(bool value);
		static void RS_WireFrame(bool value);
		static void RS_Fog(bool value);
		static void RS_AntialiasLine(bool value);
		static void RS_TextureFilter(bool value);

		static void AlphaTest_Ref(UINT Value);
		static void Alpha_Reset(void);
		static void Alpha_Function(UINT SRC_Blend, UINT DES_Blend);
		static void Alpha_SetToMaterial(UINT Stage);
		static void Alpha_SetToTexture(UINT Stage);

		static void SetFogDesc(IN FogDesc& inDesc);
		static void GetFogDesc(OUT FogDesc& outDesc);

		static bool Shader_Supported(BYTE Major = 2, BYTE Minor = 0);
	};
	typedef Device3D *PDevice3D;

} } // namespace sx { namespace d3d

#endif	//	GUARD_sxDevice3D_HEADER_FILE
