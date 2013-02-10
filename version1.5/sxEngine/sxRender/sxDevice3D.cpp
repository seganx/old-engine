#include "sxDevice3D.h"
#include "sxResource3D.h"
#include "../sxSystem/sxSystem.h"

const D3DVERTEXELEMENT9 SeganVrtxDecl[] = {
	{0, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0 },

	{1, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0 },
	{1, 12,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0 },
	{1, 20,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0 },
	{1, 24,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		1 },

	{2, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TANGENT,	0 },
	{2, 12,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	1 },

	{3, 0,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDWEIGHT,	0 },
	{3, 16,	D3DDECLTYPE_UBYTE4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },

	D3DDECL_END()
};

//////////////////////////////////////////////////////////////////////////
//	SOME INTERNAL CLASSES AND FUNCTIONS
//////////////////////////////////////////////////////////////////////////

/*
this class can create and hold some static functions and help me to easy access to them
*/
class Direct3D_internal
{
public:
	static bool Initialize(void){
		ZeroMemory(&m_DriverInfo, sizeof(DisplayDriverInfo));
		ZeroMemory(&m_PrsntParam, sizeof(D3DPresentParameters));
		m_pD3D			= NULL;
		m_pDevice		= NULL;
		m_pVrtxDecl		= NULL;
		m_pTargetSurface= NULL;
		m_pDepthSurface	= NULL;
		m_pStateInit	= NULL;

		for (int i=0; i<8; i++)
		{
			m_pVB[i] = NULL;
			m_pTX[i] = NULL;
		}
		m_pIB			= NULL;

		ZeroMemory(&m_caps,		sizeof(m_caps));
		ZeroMemory(&m_mtrl,		sizeof(m_mtrl));
		ZeroMemory(&m_vp,		sizeof(m_vp));
		ZeroMemory(&m_rc,		sizeof(m_rc));
		ZeroMemory(&m_vDisp,	sizeof(m_vDisp));
		ZeroMemory(&m_fog_Desc,	sizeof(m_fog_Desc));
		m_fog_Desc.Color = D3DColor(0.7f, 0.7f, 0.7f, 1.0f);

		m_hDisplay		= NULL;
		m_FPS			= 0;
		m_numCalls		= 0;
		m_numTris		= 0;
		m_numVerts		= 0;

		//  initialize direc3d interface and driver capabilities
		return InitAdaptor();
	}

	static bool InitAdaptor(void){
		String strlog = L"\r\n\nGraphic card information:\r\n";

		// Create Interface
		Direct3D_internal::m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		if ( !Direct3D_internal::m_pD3D )
		{
			sxLog::Log_( L"I can not create Direct3D interface !" );
			return false;
		}

		//  get display adapter
		Direct3D_internal::m_uAdapter = D3DADAPTER_DEFAULT;//adapterCount > 1 ? adapterCount - 1 : D3DADAPTER_DEFAULT;
		UINT adapterCount = Direct3D_internal::m_pD3D->GetAdapterCount();

		//strlog << L"\tNumber of adapters : " << (int)adapterCount << L"\r\n";

		if ( adapterCount > 1 )
		{
			for ( UINT i=0; i<adapterCount; i++ )
			{
				D3DADAPTER_IDENTIFIER9 ident;
				if ( FAILED(Direct3D_internal::m_pD3D->GetAdapterIdentifier(i, 0, &ident)) )
				{
					sxLog::Log_( L"I can not get driver identifier for adapter %d !" , i );
					continue;
				}

				Direct3D_internal::m_uAdapter = i;
				str512 tmpstr = ident.Description;
				strlog << L"\tadapter description ";// << (int)i << tmpstr.Text() << L"\r\n";

				tmpstr.MakeLower();
				if ( tmpstr.Find( L"intel" ) )
					continue;
			}

			strlog << L"\r\n";
		}

		// Get the current desktop display mode, so we can setup back buffer format
		if (FAILED(Direct3D_internal::m_pD3D->GetAdapterDisplayMode(Direct3D_internal::m_uAdapter, &m_CurDisplayMode)))
		{
			sxLog::Log_(L"I can not get adapter display mode !");
			return false;
		}

		//  Get device capabilities before create 3D Device
		if (FAILED(Direct3D_internal::m_pD3D->GetDeviceCaps(Direct3D_internal::m_uAdapter, D3DDEVTYPE_HAL, &Direct3D_internal::m_caps)))
		{
			sxLog::Log_(L"I can not get device capabilities !");
			return false;
		}

		// get adapter and driver information
		D3DADAPTER_IDENTIFIER9 adinfo;
		ZeroMemory(&adinfo, sizeof(adinfo));
		if (FAILED(Direct3D_internal::m_pD3D->GetAdapterIdentifier(Direct3D_internal::m_uAdapter, 0, &adinfo)))
		{
			sxLog::Log_(L"I can not get driver information !");
			return false;
		}

		Direct3D_internal::m_DriverInfo.Product		= HIWORD(adinfo.DriverVersion.HighPart);
		Direct3D_internal::m_DriverInfo.Version		= LOWORD(adinfo.DriverVersion.HighPart);
		Direct3D_internal::m_DriverInfo.SubVersion	= HIWORD(adinfo.DriverVersion.LowPart);
		Direct3D_internal::m_DriverInfo.Build		= LOWORD(adinfo.DriverVersion.LowPart);

		str1024 tmp = adinfo.Description;
		memcpy(Direct3D_internal::m_DriverInfo.Description, *tmp, tmp.Length()*2);

		tmp = adinfo.Driver;
		memcpy(Direct3D_internal::m_DriverInfo.Driver, *tmp, tmp.Length()*2);


// 		strlog << 
// 			L"\tDriver: \t"				<< Direct3D_internal::m_DriverInfo.Driver			<< 
// 			L"\r\n\tDescription: \t"	<< Direct3D_internal::m_DriverInfo.Description		<< 
// 			L"\r\n\tProduct: \t"		<< Direct3D_internal::m_DriverInfo.Product			<<
// 			L"\r\n\tVersion: \t"		<< Direct3D_internal::m_DriverInfo.Version			<<
// 			L"\r\n\tSubVersion: \t"		<< Direct3D_internal::m_DriverInfo.SubVersion		<<
// 			L"\r\n\tBuild: \t\t"		<< Direct3D_internal::m_DriverInfo.Build			<< L"\r\n\r\n";

		sxLog::Log_(strlog);

		return true;
	}

	static void Cleanup(bool cleanupAdaptor){

		SEGAN_RELEASE_AND_NULL(m_pStateInit);
		SEGAN_RELEASE_AND_NULL(m_pDepthSurface);
		SEGAN_RELEASE_AND_NULL(m_pTargetSurface);
		SEGAN_RELEASE_AND_NULL(m_pVrtxDecl);
		SEGAN_RELEASE_AND_NULL(m_pDevice);

		if (cleanupAdaptor)
		{
			SEGAN_RELEASE_AND_NULL(m_pD3D);
		}
	}

	static DisplayDriverInfo			m_DriverInfo;		// store the deriver information
	static D3DDISPLAYMODE				m_CurDisplayMode;	// store the current display mode
	static D3DPresentParameters			m_PrsntParam;		// store present parameters for reseting
	static DWORD						m_creationFlag; 	// store the device creation flags
	static PDirect3D					m_pD3D;				// used to create the D3DDevice
	static UINT							m_uAdapter;			// display adapter
	static PDirect3DDevice				m_pDevice;			// our rendering device
	static PDirect3DVertexDeclaration	m_pVrtxDecl;		// engine vertex declaration
	static PDirect3DSurface				m_pTargetSurface;	// our Direct3D target Surface
	static PDirect3DSurface				m_pDepthSurface;	// our Direct3D depth Surface
	static PDirect3DStateBlock			m_pStateInit;		// store State block of ReInit() function
	static HRESULT						m_hCoopLevel;		// result of cooperative level

	static PDirect3DVertexBuffer		m_pVB[8];
	static PDirect3DIndexBuffer			m_pIB;
	static PDirect3DBaseTexture			m_pTX[8];

	static D3DCaps						m_caps;			// get the system capabilities
	static D3DMaterial					m_mtrl;
	static D3DViewport					m_vp;			// 3D device view port structure
	static D3DViewport					m_vDisp;		// store default view port
	static HWND							m_hDisplay;		// store handle of display window
	static RECT							m_rc;			// rect of view port will use in present function
	static int2							m_InitSize;		//  size of back buffer on create device

	static FogDesc						m_fog_Desc;		//  fog description

	static DWORD						rs_alphaBlend;
	static bool							rs_zEnabled;
	static bool							rs_zWritable;
	static bool							rs_vertexColor;
	static bool							rs_culling;
	static bool							rs_lighting;
	static bool							rs_backface;
	static bool							rs_rendering;
	static bool							rs_wireFrame;
	static bool							rs_fog;
	static bool							m_adaptorInited;

	static int							m_FPS;
	static int							m_numCalls;
	static int							m_numTris;
	static int							m_numVerts;

	static Matrix						m_matWorld;		// current world matrix of the device
	static Matrix						m_matView;		// current view matrix of the device
	static Matrix						m_matProj;		// current proj matrix of the device
};
DisplayDriverInfo			Direct3D_internal::m_DriverInfo;
D3DDISPLAYMODE				Direct3D_internal::m_CurDisplayMode;
D3DPresentParameters		Direct3D_internal::m_PrsntParam;
DWORD						Direct3D_internal::m_creationFlag	= SX_D3D_VSYNC;
PDirect3D					Direct3D_internal::m_pD3D			= NULL;
UINT						Direct3D_internal::m_uAdapter		= 0;
PDirect3DDevice				Direct3D_internal::m_pDevice		= NULL;		
PDirect3DVertexDeclaration	Direct3D_internal::m_pVrtxDecl		= NULL;
PDirect3DSurface			Direct3D_internal::m_pTargetSurface	= NULL;
PDirect3DSurface			Direct3D_internal::m_pDepthSurface	= NULL;
PDirect3DStateBlock			Direct3D_internal::m_pStateInit		= NULL;
HRESULT						Direct3D_internal::m_hCoopLevel		= D3D_OK;
PDirect3DVertexBuffer		Direct3D_internal::m_pVB[8];
PDirect3DIndexBuffer		Direct3D_internal::m_pIB;
PDirect3DBaseTexture		Direct3D_internal::m_pTX[8];
D3DCaps						Direct3D_internal::m_caps;
D3DMaterial					Direct3D_internal::m_mtrl;
D3DViewport					Direct3D_internal::m_vp;		
D3DViewport					Direct3D_internal::m_vDisp;
HWND						Direct3D_internal::m_hDisplay		= NULL;
RECT						Direct3D_internal::m_rc;
int2						Direct3D_internal::m_InitSize(0, 0);
FogDesc						Direct3D_internal::m_fog_Desc;
Matrix						Direct3D_internal::m_matWorld		= sx::math::MTRX_IDENTICAL;
Matrix						Direct3D_internal::m_matView		= sx::math::MTRX_IDENTICAL;
Matrix						Direct3D_internal::m_matProj		= sx::math::MTRX_IDENTICAL;
int							Direct3D_internal::m_FPS			= 0;
int							Direct3D_internal::m_numCalls		= 0;
int							Direct3D_internal::m_numTris		= 0;
int							Direct3D_internal::m_numVerts		= 0;

DWORD						Direct3D_internal::rs_alphaBlend	= 0;
bool						Direct3D_internal::rs_zEnabled		= true;
bool						Direct3D_internal::rs_zWritable		= true;
bool						Direct3D_internal::rs_vertexColor	= true;
bool						Direct3D_internal::rs_culling		= true;
bool						Direct3D_internal::rs_lighting		= true;
bool						Direct3D_internal::rs_backface		= true;
bool						Direct3D_internal::rs_rendering		= false;
bool						Direct3D_internal::rs_wireFrame		= false;
bool						Direct3D_internal::rs_fog			= false;
bool						Direct3D_internal::m_adaptorInited	= false;



HRESULT IsDepthFormatOK(PDirect3D d3dInterface, PD3DCaps pCaps, D3DFormat AdapterFormat, D3DFormat DepthFormat, D3DFormat BackBufferFormat)
{
	// Verify that the depth format exists
	HRESULT result = d3dInterface->CheckDeviceFormat(
		Direct3D_internal::m_uAdapter,
		pCaps->DeviceType,
		AdapterFormat,
		D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE,
		DepthFormat
		);

	if (FAILED(result)) return result;

	// Verify that the depth format is compatible
	return d3dInterface->CheckDepthStencilMatch(
		Direct3D_internal::m_uAdapter,
		pCaps->DeviceType,
		AdapterFormat,
		BackBufferFormat,
		DepthFormat
		);
}

D3DFormat FindBestDepthFormat(PDirect3D d3dInterface, PD3DCaps pCaps, D3DFormat AdapterFormat)
{
	//  First try to 32 bit depth buffer and then 24 bit and so on...
	if (SUCCEEDED(IsDepthFormatOK(d3dInterface, pCaps, AdapterFormat, D3DFMT_D32, AdapterFormat)))
		return D3DFMT_D32;

	if (SUCCEEDED(IsDepthFormatOK(d3dInterface, pCaps, AdapterFormat, D3DFMT_D24S8, AdapterFormat)))	
		return D3DFMT_D24S8;

	if (SUCCEEDED(IsDepthFormatOK(d3dInterface, pCaps, AdapterFormat, D3DFMT_D24X8, AdapterFormat)))
		return D3DFMT_D24X8;

	return D3DFMT_D16;
}

UINT FindBestVertexProcessing(PD3DCaps pCaps)
{
	// Verify that the hardware support vertex processing ...
	if (D3DDEVCAPS_HWTRANSFORMANDLIGHT & pCaps->DevCaps)
		return D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}


namespace sx { namespace d3d
{

	//////////////////////////////////////////////////////////////////////////
	//	CLASS DEVICE 3D
	//////////////////////////////////////////////////////////////////////////
	void Device3D::GetDriverInfo( DisplayDriverInfo& dInfo )
	{
		if ( !Direct3D_internal::m_adaptorInited )
			Direct3D_internal::m_adaptorInited = Direct3D_internal::Initialize();

		dInfo.Product		= Direct3D_internal::m_DriverInfo.Product;
		dInfo.Version		= Direct3D_internal::m_DriverInfo.Version;
		dInfo.SubVersion	= Direct3D_internal::m_DriverInfo.SubVersion;
		dInfo.Build			= Direct3D_internal::m_DriverInfo.Build;

		memcpy(dInfo.Driver, Direct3D_internal::m_DriverInfo.Driver, MAX_DEVICE_IDENTIFIER_STRING);
		memcpy(dInfo.Description, Direct3D_internal::m_DriverInfo.Description, MAX_DEVICE_IDENTIFIER_STRING);
	}

	FORCEINLINE PDirect3DDevice Device3D::GetDevice( void )
	{
		return Direct3D_internal::m_pDevice;
	}

	FORCEINLINE PD3DCaps Device3D::GetCaps( void )
	{
		return &Direct3D_internal::m_caps;
	}

	FORCEINLINE int Device3D::GetFPS( void )
	{
		return Direct3D_internal::m_FPS;
	}

	int Device3D::GetAdapterRefreshRate( void )
	{
		return Direct3D_internal::m_CurDisplayMode.RefreshRate;
	}

	int Device3D::GetNumberOfDrawCalls( void )
	{
		return Direct3D_internal::m_numCalls;
	}

	int Device3D::GetNumberOfTriangles( void )
	{
		return Direct3D_internal::m_numTris;
	}

	int Device3D::GetNumberOfVertices( void )
	{
		return Direct3D_internal::m_numVerts;
	}

	bool Device3D::Create( HWND Display, UINT Width, UINT Height, SX_D3D_ DWORD Flag)
	{
		if ( !Direct3D_internal::m_adaptorInited )
			Direct3D_internal::m_adaptorInited = Direct3D_internal::Initialize();

		Direct3D_internal::Cleanup(false);

		//  validate data
		if ( !Width )	Width	= Direct3D_internal::m_CurDisplayMode.Width;
		if ( !Height )	Height	= Direct3D_internal::m_CurDisplayMode.Height;

		//  save current creation flags
		Direct3D_internal::m_creationFlag = Flag;
		Direct3D_internal::m_InitSize = int2(Width, Height);

		//  validate window size for full screen support
		if (Flag & SX_D3D_FULLSCREEN)
		{
			sx::sys::PWindow pwin;
			if (sys::Application::Get_Window(Display, pwin))
			{
				pwin->SetBorder(WBT_NONE);
				pwin->SetRect(0, 0, Width, Height);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Set up the structure used to create the D3DDevice...
		D3DPresentParameters PresentParam;
		ZeroMemory(&PresentParam, sizeof(PresentParam));

		PresentParam.BackBufferFormat		= D3DFMT_A8R8G8B8;
		PresentParam.BackBufferCount		= 0;
		PresentParam.Flags					= 0;
		PresentParam.MultiSampleQuality		= 0;
		PresentParam.SwapEffect				= D3DSWAPEFFECT_DISCARD;
		PresentParam.MultiSampleType		= D3DMULTISAMPLE_NONE;


		//  find best depth format
		PresentParam.EnableAutoDepthStencil	= TRUE;
		PresentParam.AutoDepthStencilFormat	= FindBestDepthFormat(Direct3D_internal::m_pD3D, &Direct3D_internal::m_caps, Direct3D_internal::m_CurDisplayMode.Format);

		//  set display size
		PresentParam.BackBufferHeight			= Height;
		PresentParam.BackBufferWidth			= Width;
		PresentParam.hDeviceWindow				= Display;

		//  set device behavior
		PresentParam.Windowed					= (Flag & SX_D3D_FULLSCREEN) ? FALSE : TRUE;
		PresentParam.FullScreen_RefreshRateInHz = 0;//(Flag & SX_D3D_FULLSCREEN) ? Device3D_internal::m_CurDisplayMode.RefreshRate : 0;
		PresentParam.PresentationInterval		= (Flag & SX_D3D_VSYNC) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;


		//  store current presentation parameters
		Direct3D_internal::m_PrsntParam = PresentParam;

		// Create the Direct3D device...............................................
		if (FAILED(Direct3D_internal::m_pD3D->CreateDevice(
			Direct3D_internal::m_uAdapter,
			D3DDEVTYPE_HAL,
			Display,
			FindBestVertexProcessing(&Direct3D_internal::m_caps) /*| D3DCREATE_PUREDEVICE*/, //  I discard this flag because of "http://msdn.microsoft.com/en-us/library/ee416788.aspx#What_is_the_purpose_of_the_D3DCREATE_PUREDEVICE_flag"
			&PresentParam,
			&Direct3D_internal::m_pDevice)))
		{
			sxLog::Log(L"That's strange! I created Direct3D interface but I can not create Direct3D Device !");
			SEGAN_RELEASE_AND_NULL(Direct3D_internal::m_pD3D);
			return false;
		}

		// Create Vertex declaration 
		Direct3D_internal::m_pDevice->CreateVertexDeclaration(SeganVrtxDecl, &Direct3D_internal::m_pVrtxDecl);

		// Get device surface
		Direct3D_internal::m_pDevice->GetDepthStencilSurface( &Direct3D_internal::m_pDepthSurface );
		Direct3D_internal::m_pDevice->GetRenderTarget( 0, &Direct3D_internal::m_pTargetSurface );

		// Save display handle
		Direct3D_internal::m_hDisplay = Display;

		// Reset d3dDevice settings
		InitStates();

		// update display rect
		Display_Resized();

		// Set default view port
		Direct3D_internal::m_vp.Width	= Direct3D_internal::m_vDisp.Width;
		Direct3D_internal::m_vp.Height	= Direct3D_internal::m_vDisp.Height;
		Direct3D_internal::m_vp.MinZ	= 0.0f;
		Direct3D_internal::m_vp.MaxZ	= 1.0f;
		Viewport_Set(&Direct3D_internal::m_vp);

		String slog = L"Rendering device created successfully :\r\n";
		slog << L"	API		: DirectX 9\r\n";
		slog << L"	BackBuffer	: A8R8G8B8\r\n";
		slog << L"	Depth		: ";
		switch (Direct3D_internal::m_PrsntParam.AutoDepthStencilFormat)
		{
		case D3DFMT_D16:	slog << L"D16\r\n";		break;
		case D3DFMT_D24X8:	slog << L"D24x8\r\n";	break;
		case D3DFMT_D24S8:	slog << L"D24S8\r\n";	break;
		case D3DFMT_D32:	slog << L"D32\r\n";		break;
		}

		sxLog::Log( slog.Text() );

		// 		if ( Device3D_internal::m_caps.TextureCaps & D3DPTEXTURECAPS_POW2 )
		// 			slog << L"yes";

		return true;
	}

	HRESULT Device3D::CreateDepthStencil( IN const UINT width, IN const UINT height, OUT PDirect3DSurface& outSurf )
	{
		if (!Direct3D_internal::m_pDevice) return E_FAIL;

		return Direct3D_internal::m_pDevice->CreateDepthStencilSurface(
			width,
			height,
			Direct3D_internal::m_PrsntParam.AutoDepthStencilFormat,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&outSurf,
			NULL);
	}

	void Device3D::Destroy( void )
	{
		if ( Direct3D_internal::m_pDevice )
			Direct3D_internal::m_pDevice->EvictManagedResources();

		sx::d3d::Resource3D::Clear();
		Direct3D_internal::Cleanup(true);
		sxLog::Log( L"Rendering device released.\r\n" );
	}

	FORCEINLINE DWORD Device3D::GetCreationFlag( void )
	{
		return Direct3D_internal::m_creationFlag;
	}

	void Device3D::InitStates( void )
	{
		sx_callstack_push(Device3D::InitStates());

		for (int i=0; i<8; i++)
		{
			Direct3D_internal::m_pTX[i] = NULL;
			Direct3D_internal::m_pVB[i] = NULL;
		}
		Direct3D_internal::m_pIB = NULL;

		Direct3D_internal::rs_alphaBlend	= 0;
		Direct3D_internal::rs_zEnabled		= true;
		Direct3D_internal::rs_zWritable		= true;
		Direct3D_internal::rs_vertexColor	= false;
		Direct3D_internal::rs_culling		= true;
		Direct3D_internal::rs_lighting		= true;
		Direct3D_internal::rs_backface		= false;
		Direct3D_internal::rs_rendering		= false;
		Direct3D_internal::rs_wireFrame		= false;
		Direct3D_internal::rs_fog			= false;

		if (!Direct3D_internal::m_pStateInit)
		{
			// create the state block for fast reinitialization in the next time
			Direct3D_internal::m_pDevice->CreateStateBlock(D3DSBT_ALL, &Direct3D_internal::m_pStateInit);
			Direct3D_internal::m_pDevice->BeginStateBlock();

			// set Engine Vertex declaration
			Direct3D_internal::m_pDevice->SetVertexDeclaration(Direct3D_internal::m_pVrtxDecl);

			// render state initialization
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ZENABLE,			TRUE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,	TRUE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_LIGHTING,		TRUE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_DITHERENABLE,	TRUE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_COLORVERTEX,		FALSE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_SPECULARENABLE,	FALSE);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_CULLMODE,		D3DCULL_CCW);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_AMBIENT,			0xffffffff);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, /*D3DCOLORWRITEENABLE_ALPHA |*/ D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED);
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1 );


			// set AlphaBlending
			Alpha_Reset();

			// set AlphaTesting
			AlphaTest_Ref(156);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			// turn off all lights
			for (UINT i=0; i<Direct3D_internal::m_caps.MaxActiveLights; i++)
				Direct3D_internal::m_pDevice->LightEnable(i, FALSE);

			// basic settings for textures
			Texture_ResetStateStages();
			Texture_SetSampleState();
			for (UINT i=0; i<8; i++)
				Direct3D_internal::m_pDevice->SetTexture(i, NULL);

			// try to anti alias
			//Device3D_internal::m_pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

			// initialize fog
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

			Direct3D_internal::m_pDevice->EndStateBlock(&Direct3D_internal::m_pStateInit);
		}

		Direct3D_internal::m_pStateInit->Apply();
	}

	bool Device3D::CheckHealth( rnrCallback_OnDeviceLost OnDeviceLost, rnrCallback_OnDeviceReset OnDeviceReset )
	{
		sx_callstack_push(Device3D::CheckHealth());

		static int ResetFailed = 0;
		if (!Direct3D_internal::m_pDevice) return false;

		Direct3D_internal::m_hCoopLevel = Direct3D_internal::m_pDevice->TestCooperativeLevel();
		switch (Direct3D_internal::m_hCoopLevel)
		{
		case D3DERR_DEVICELOST:	return false;
		case D3DERR_DEVICENOTRESET:

			if (ResetFailed>10) 
			{
				sxLog::Log(L"I'm really sorry! reseting Direct3D Device failed for more that 10 time \n Please save current state and shutdown the application");
				return false;
			}

			if (OnDeviceLost && !ResetFailed)	
				OnDeviceLost();

			sxLog::Log(L"The Direct3D Device is lost! Now I'm trying to reset it !");
			if ( SUCCEEDED( Reset(Direct3D_internal::m_InitSize.x, Direct3D_internal::m_InitSize.y, Direct3D_internal::m_creationFlag) ) )
			{
				ResetFailed = 0;
				sxLog::Log(L"The Direct3D Device is now ready !");

				if (OnDeviceReset)	
					OnDeviceReset();

				return true;
			}
			else
			{
				ResetFailed++;
				sxLog::Log(L"Reseting Direct3D Device failed !");
				Sleep(1000);
				return false;
			}
		}

		return true;
	}

	HRESULT Device3D::Reset( UINT Width, UINT Height, SX_D3D_ DWORD Flag )
	{
		sx_callstack_push(Device3D::Reset(Width=%d, Height=%d));

		//  cleanup last resources
		Resource3D::OnDeviceLost();
		SEGAN_RELEASE_AND_NULL(Direct3D_internal::m_pVrtxDecl);
		SEGAN_RELEASE_AND_NULL(Direct3D_internal::m_pStateInit);
		SEGAN_RELEASE_AND_NULL(Direct3D_internal::m_pDepthSurface);
		SEGAN_RELEASE_AND_NULL(Direct3D_internal::m_pTargetSurface);

		for (int i =0; i<8; i++)
		{
			Direct3D_internal::m_pVB[i] = NULL;
			Direct3D_internal::m_pTX[i] = NULL;
		}
		Direct3D_internal::m_pIB = NULL;

		//  validate data
		if ( !Width )	Width	= Direct3D_internal::m_CurDisplayMode.Width;
		if ( !Height )	Height	= Direct3D_internal::m_CurDisplayMode.Height;

		//  validate window size for full screen support
		if (Flag & SX_D3D_FULLSCREEN)
		{
			sx::sys::PWindow pwin;
			if (sys::Application::Get_Window(Direct3D_internal::m_hDisplay, pwin))
			{
				pwin->SetBorder(WBT_NONE);
				pwin->SetRect(0, 0, Width, Height);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Set up the structure used to reset the D3DDevice...
		D3DPresentParameters PresentParam	= Direct3D_internal::m_PrsntParam;

		PresentParam.BackBufferFormat		= D3DFMT_A8R8G8B8;
		PresentParam.BackBufferCount		= 0;
		PresentParam.Flags					= 0;
		PresentParam.MultiSampleQuality		= 0;
		PresentParam.SwapEffect				= D3DSWAPEFFECT_DISCARD;
		PresentParam.MultiSampleType		= D3DMULTISAMPLE_NONE;

		//  set display size
		PresentParam.BackBufferHeight			= Height;
		PresentParam.BackBufferWidth			= Width;

		//  set device behavior
		PresentParam.Windowed					= (Flag & SX_D3D_FULLSCREEN) ? FALSE : TRUE;
		PresentParam.FullScreen_RefreshRateInHz = (Flag & SX_D3D_FULLSCREEN) ? Direct3D_internal::m_CurDisplayMode.RefreshRate : 0;
		PresentParam.PresentationInterval		= (Flag & SX_D3D_VSYNC) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

		//  try to reset the device
		int resetTime = 0;

e_reset:
		HRESULT res = Direct3D_internal::m_pDevice->Reset(&PresentParam);
		resetTime++;

		if (res == D3DERR_INVALIDCALL)
		{
			if (resetTime>100)	return res;
			Sleep(20);
			goto e_reset;
		}

		if (res == S_OK)
		{
			//  report device is ready
			Direct3D_internal::m_hCoopLevel = D3D_OK;

			//  save current creation flags
			Direct3D_internal::m_creationFlag = Flag;

			// Create Vertex declaration 
			Direct3D_internal::m_pDevice->CreateVertexDeclaration(SeganVrtxDecl, &Direct3D_internal::m_pVrtxDecl);

			// Get device surface
			Direct3D_internal::m_pDevice->GetDepthStencilSurface( &Direct3D_internal::m_pDepthSurface );
			Direct3D_internal::m_pDevice->GetRenderTarget(0, &Direct3D_internal::m_pTargetSurface);

			// Reset d3dDevice settings
			InitStates();

			// update display rect
			Display_Resized();

			// Set default view port
			Direct3D_internal::m_vp.Width	= Direct3D_internal::m_vDisp.Width;
			Direct3D_internal::m_vp.Height	= Direct3D_internal::m_vDisp.Height;
			Direct3D_internal::m_vp.MinZ	= 0.0f;
			Direct3D_internal::m_vp.MaxZ	= 0.1f;
			Viewport_Set(&Direct3D_internal::m_vp);

			//  set fog properties
			SetFogDesc(Direct3D_internal::m_fog_Desc);

			//  initialize resources
			Resource3D::OnDeviceReset();

			//	notify the application to operate post reset
			::PostMessage(Direct3D_internal::m_hDisplay, WM_SX_D3D_RESET, 0, 0);
		}

		return res;
	}

	FORCEINLINE bool Device3D::IsCreated( void )
	{
		return (Direct3D_internal::m_pDevice != NULL);
	}

	FORCEINLINE bool sx::d3d::Device3D::IsReady( void )
	{
		return (Direct3D_internal::m_pDevice != NULL && Direct3D_internal::m_hCoopLevel == D3D_OK);
	}

	void Device3D::SetMaterialColor( const D3DColor& color )
	{
		D3DMaterial mtrl;
		mtrl.Diffuse = color;
		mtrl.Ambient = color;
		mtrl.Specular = D3DColor(0, 0, 0, 0);
		mtrl.Emissive = D3DColor(0, 0, 0, 0);
		mtrl.Power = 0;

		Direct3D_internal::m_pDevice->SetMaterial(&mtrl);
	}

	void Device3D::Clear_Screen( DWORD bgcolor )
	{
		if (Direct3D_internal::m_creationFlag & SX_D3D_FULLSCREEN)
		{
			Clear_Target(bgcolor);
			Clear_ZBuffer();
		}
		else
		{
			Direct3D_internal::m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, bgcolor, 1.0f, 0);
		}
	}

	FORCEINLINE void Device3D::Clear_Target( DWORD bgcolor )
	{
		Direct3D_internal::m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET , bgcolor, 1.0f, 0);
	}

	FORCEINLINE void Device3D::Clear_ZBuffer( void )
	{
		Direct3D_internal::m_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	}

	bool Device3D::Scene_Begin( void )
	{
		if (Direct3D_internal::rs_rendering) return false;

		InitStates();
		Direct3D_internal::m_numCalls = 0;
		Direct3D_internal::m_numTris = 0;
		Direct3D_internal::m_numVerts = 0;
		Direct3D_internal::rs_rendering = true;
		return (Direct3D_internal::m_pDevice->BeginScene() == S_OK);
	}

	void sx::d3d::Device3D::Scene_End( void )
	{
		sx_callstack_push(Device3D::Scene_End());

		if (!Direct3D_internal::rs_rendering) return;

		Direct3D_internal::rs_rendering = false;
		Direct3D_internal::m_pDevice->EndScene();
	}

	void Device3D::Scene_Present( PRECT pDestRect )
	{
		sx_callstack_push(Device3D::Scene_Present());

		static int   s_iFrame	= 0;
		static float s_iTime	= 0;
		static float s_eTime	= 0;

		float curTime = sys::GetSysTime();
		s_eTime+= abs(curTime - s_iTime);
		s_iTime = curTime;
		if (s_eTime>=1000)
		{
			Direct3D_internal::m_FPS = s_iFrame;
			s_iFrame = 0;
			s_eTime = 0;
		}
		s_iFrame++;

		if (Direct3D_internal::m_creationFlag & SX_D3D_FULLSCREEN)
			Direct3D_internal::m_pDevice->Present(NULL, NULL, NULL, NULL);
		else
			Direct3D_internal::m_pDevice->Present(&Direct3D_internal::m_rc, pDestRect, Direct3D_internal::m_hDisplay, NULL);
	}

	FORCEINLINE void Device3D::SetClipPlane( DWORD Index, const float* pPlane )
	{
		Direct3D_internal::m_pDevice->SetClipPlane(Index, pPlane);
	}

	FORCEINLINE void Device3D::GetClipPlane( DWORD Index, float* pPlane )
	{
		Direct3D_internal::m_pDevice->GetClipPlane(Index, pPlane);
	}

	void Device3D::SetVertexBuffer( BYTE strmIndex, PDirect3DVertexBuffer vrtxbuffr, UINT strd )
	{
		if (vrtxbuffr != Direct3D_internal::m_pVB[strmIndex])
		{
			Direct3D_internal::m_pVB[strmIndex] = vrtxbuffr;
			Direct3D_internal::m_pDevice->SetStreamSource(strmIndex, vrtxbuffr, 0, strd);
		}
	}

	void Device3D::SetIndexBuffer( PDirect3DIndexBuffer indxbfr )
	{
		if (indxbfr != Direct3D_internal::m_pIB)
		{
			Direct3D_internal::m_pIB = indxbfr;
			Direct3D_internal::m_pDevice->SetIndices(indxbfr);
		}
	}

	void Device3D::SetTexture( UINT stage, PDirect3DBaseTexture txur )
	{
		if (txur != Direct3D_internal::m_pTX[stage])
		{
			Direct3D_internal::m_pTX[stage] = txur;
			Direct3D_internal::m_pDevice->SetTexture(stage, txur);
		}
	}

	FORCEINLINE void Device3D::SetVertexShader( PDirect3DVertexShader vshader )
	{
		Direct3D_internal::m_pDevice->SetVertexShader(vshader);
	}

	FORCEINLINE void Device3D::SetPixelShader( PDirect3DPixelShader pshader )
	{
		Direct3D_internal::m_pDevice->SetPixelShader(pshader);
	}

	void Device3D::SetRenderTarget( UINT index, PDirect3DSurface surf )
	{
		sx_callstack_push(Device3D::SetRenderTarget(index=%d), index);

		if (surf)
			Direct3D_internal::m_pDevice->SetRenderTarget(index, surf);
		else
			Direct3D_internal::m_pDevice->SetRenderTarget(index, index==0 ? Direct3D_internal::m_pTargetSurface : NULL);
	}

	void Device3D::SetDepthStencil( PDirect3DSurface surf )
	{
		if (surf)
			Direct3D_internal::m_pDevice->SetDepthStencilSurface(surf);
		else
			Direct3D_internal::m_pDevice->SetDepthStencilSurface(Direct3D_internal::m_pDepthSurface);
	}

	void Device3D::SetEffect( PD3DXEffect effect )
	{
		sx_callstack_push(Device3D::SetEffect());

		static PD3DXEffect lastEffect = NULL;
		if (lastEffect != effect)
		{
			if (lastEffect)
			{
				lastEffect->EndPass();
				lastEffect->End();
			}
			lastEffect = effect;

			if (!effect) 
			{
				SetVertexShader(NULL);
				SetPixelShader(NULL);
				return;
			}
			effect->Begin(NULL, D3DXFX_DONOTSAVESTATE );
			effect->BeginPass(0);
		}
		else 
		{
			if (!effect) return;
			effect->CommitChanges();
		}
	}

	FORCEINLINE void Device3D::GetRenderTarget( UINT index, OUT PDirect3DSurface& surf )
	{
		Direct3D_internal::m_pDevice->GetRenderTarget(index, &surf);
		SEGAN_RELEASE(surf);
	}

	void Device3D::GetDepthStencil( OUT PDirect3DSurface& surf )
	{
		Direct3D_internal::m_pDevice->GetDepthStencilSurface(&surf);
		SEGAN_RELEASE(surf);
	}

	FORCEINLINE void Device3D::DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount )
	{
		Direct3D_internal::m_pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
		Direct3D_internal::m_numTris += PrimitiveCount;
		Direct3D_internal::m_numCalls++;
	}

	FORCEINLINE void Device3D::DrawIndexedPrimitive( D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
	{
		Direct3D_internal::m_pDevice->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		Direct3D_internal::m_numTris += primCount;
		Direct3D_internal::m_numVerts += NumVertices;
		Direct3D_internal::m_numCalls++;
	}

	FORCEINLINE void Device3D::DrawPrimitiveUP( D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride, DWORD fvf )
	{
		if(fvf) Device3D::GetDevice()->SetFVF( fvf );
		Direct3D_internal::m_pDevice->DrawPrimitiveUP( PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride );
		if(fvf) Device3D::GetDevice()->SetVertexDeclaration( Direct3D_internal::m_pVrtxDecl );

		Direct3D_internal::m_numTris += PrimitiveCount;
		Direct3D_internal::m_numCalls++;
	}

	FORCEINLINE void Device3D::Matrix_World_Set( const Matrix& mat )
	{
		Direct3D_internal::m_matWorld = mat;
		Direct3D_internal::m_pDevice->SetTransform(D3DTS_WORLD, &mat);
	}

	FORCEINLINE void Device3D::Matrix_World_Get( Matrix& mat )
	{
		mat = Direct3D_internal::m_matWorld;
	}

	FORCEINLINE void Device3D::Matrix_View_Set( const Matrix& mat )
	{
		Direct3D_internal::m_matView = mat;
		Direct3D_internal::m_pDevice->SetTransform(D3DTS_VIEW, &mat);
	}

	FORCEINLINE void Device3D::Matrix_View_Get( Matrix& mat )
	{
		mat = Direct3D_internal::m_matView;
	}

	FORCEINLINE void Device3D::Matrix_Project_Set( const Matrix& mat )
	{
		Direct3D_internal::m_matProj = mat;
		Direct3D_internal::m_pDevice->SetTransform(D3DTS_PROJECTION, &mat);
	}

	FORCEINLINE void Device3D::Matrix_Project_Get( Matrix& mat )
	{
		mat = Direct3D_internal::m_matProj;
	}

	FORCEINLINE void Device3D::Texture_ResetStateStages( void )
	{
		for (UINT i=0; i<8; i++)
		{
			Direct3D_internal::m_pDevice->SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE  );
			Direct3D_internal::m_pDevice->SetTextureStageState(i, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE  );
			Direct3D_internal::m_pDevice->SetTextureStageState(i, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			Direct3D_internal::m_pDevice->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE  );
			Direct3D_internal::m_pDevice->SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_DIFFUSE  );
			Direct3D_internal::m_pDevice->SetTextureStageState(i, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		}
	}

	FORCEINLINE void Device3D::Texture_SetSampleState( SamplerUV SUV_ samplerUV )
	{
		for (UINT i=0; i<8; i++)
		{
			Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_ADDRESSU,  samplerUV);
			Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_ADDRESSV,  samplerUV);
			Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_ADDRESSW,  samplerUV);

			// try to use Anisotropy
			if (D3DPRASTERCAPS_ANISOTROPY & Direct3D_internal::m_caps.RasterCaps)
			{
				Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, Direct3D_internal::m_caps.MaxAnisotropy);
				//Device3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
				Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
				//Device3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
			}
			else
			{
				//Device3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
				//Device3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			}

			if ( i < 4 )
			{
				Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC/*D3DTEXF_POINT*/);
				Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC/*D3DTEXF_LINEAR*/);
			}
			else
			{
				Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
				Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			}

			Direct3D_internal::m_pDevice->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0xffffffff); 
		}
	}

	FORCEINLINE UINT Device3D::Texture_GetMaxWidth( void )
	{
		UINT level = Texture_GetMaxLevels() - 1;
		return (UINT)pow( 2.0, (int)level);	
	}

	FORCEINLINE UINT Device3D::Texture_GetMaxLevels( void )
	{
		//  TODO :
		//  find a simple equation to find maximum texture levels supported by hardware
		return 13;
	}

	FORCEINLINE void Device3D::Viewport_Set( const PD3DViewport vp )
	{
		Direct3D_internal::m_vp = *vp;
		Direct3D_internal::m_pDevice->SetViewport(&Direct3D_internal::m_vp);
	}

	FORCEINLINE PD3DViewport sx::d3d::Device3D::Viewport( void )
	{
		return &Direct3D_internal::m_vp;
	}

	FORCEINLINE void Device3D::Viewport_SetDefault( void )
	{
		Viewport_Set(&Direct3D_internal::m_vDisp);
	}

	FORCEINLINE void Device3D::Display_SetHandle( HWND vp_Hndl )
	{
		Direct3D_internal::m_hDisplay = vp_Hndl;
	}

	FORCEINLINE HWND Device3D::Display_GetHandle( void )
	{
		return Direct3D_internal::m_hDisplay;
	}

	void Device3D::Display_Resized( void )
	{
		if ( !Device3D::IsReady() ) return;

		D3DSURFACE_DESC sDesc;
		Direct3D_internal::m_pTargetSurface->GetDesc(&sDesc);

		WINDOWINFO winf;
		GetWindowInfo(Direct3D_internal::m_hDisplay, &winf);
		Direct3D_internal::m_rc.right	= winf.rcClient.right - winf.rcClient.left;
		Direct3D_internal::m_rc.bottom	= winf.rcClient.bottom - winf.rcClient.top;

		if (Direct3D_internal::m_rc.right  > (LONG)sDesc.Width)		Direct3D_internal::m_rc.right  = sDesc.Width;
		if (Direct3D_internal::m_rc.bottom > (LONG)sDesc.Height)	Direct3D_internal::m_rc.bottom = sDesc.Height;

		Direct3D_internal::m_vDisp.Width  = Direct3D_internal::m_rc.right;
		Direct3D_internal::m_vDisp.Height = Direct3D_internal::m_rc.bottom;
		Direct3D_internal::m_vDisp.MinZ	  = 0.0f;
		Direct3D_internal::m_vDisp.MaxZ	  = 1.0f;
	}

	void Device3D::Camera_Projection( float FOV, float Aspect /*= 0*/, float zNear /*= 0*/, float zFar /*= 0*/ )
	{
		if (!Aspect)
			Aspect = (float)Direct3D_internal::m_vp.Width / (float)Direct3D_internal::m_vp.Height;

		if (!zNear)
			zNear = 0.5f;

		if (!zFar)
			zFar = 4000.0f;

		// For the projection matrix, we set up a perspective transform
		Direct3D_internal::m_matProj.PerspectiveFovLH(FOV, Aspect, zNear, zFar);
		Direct3D_internal::m_pDevice->SetTransform(D3DTS_PROJECTION, &Direct3D_internal::m_matProj);
	}

	void Device3D::Camera_Pos( const Vector& Eye, const Vector& At )
	{
		static Vector Up(0.0f, 1.0f, 0.0f);

		// Set up our view matrix
		Direct3D_internal::m_matView.LookAtLH(Eye, At, Up);
		Direct3D_internal::m_pDevice->SetTransform(D3DTS_VIEW, &Direct3D_internal::m_matView);
	}

	void Device3D::Camera_Pos( const Vector& Eye, const Vector& At, const Vector& Up )
	{
		// Set up our view matrix
		Direct3D_internal::m_matView.LookAtLH( Eye, At, Up);
		Direct3D_internal::m_pDevice->SetTransform(D3DTS_VIEW, &Direct3D_internal::m_matView);
	}

	FORCEINLINE void Device3D::RS_Set( D3DRENDERSTATETYPE State, DWORD Value )
	{
		Direct3D_internal::m_pDevice->SetRenderState(State, Value);
	}

	FORCEINLINE void Device3D::RS_Get( D3DRENDERSTATETYPE State, DWORD& Value )
	{
		Direct3D_internal::m_pDevice->GetRenderState(State, &Value);
	}

	void Device3D::RS_Alpha( DWORD mode )
	{
		sx_callstack_push(Device3D::RS_Alpha());

		DWORD option;

		if ( mode & SX_MATERIAL_ALPHABLEND )
			option = SX_MATERIAL_ALPHABLEND;
		else if ( mode & SX_MATERIAL_ALPHAADD )
			option = SX_MATERIAL_ALPHAADD;
		else if ( mode & SX_MATERIAL_ALPHAMUL )
			option = SX_MATERIAL_ALPHAMUL;
		else
			option = 0;

		if ( Direct3D_internal::rs_alphaBlend == option ) return;
		Direct3D_internal::rs_alphaBlend = option;

		switch ( option )
		{
		case SX_MATERIAL_ALPHABLEND:
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			break;

		case SX_MATERIAL_ALPHAADD:
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			break;

		case SX_MATERIAL_ALPHAMUL:
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			break;

		default:
			Direct3D_internal::m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			break;
		}

	}

	FORCEINLINE void Device3D::RS_ZEnabled( bool value )
	{
		if (Direct3D_internal::rs_zEnabled == value) return;
		Direct3D_internal::rs_zEnabled = value;
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ZENABLE, DWORD(value));
	}

	FORCEINLINE void Device3D::RS_ZWritable( bool value )
	{
		if (Direct3D_internal::rs_zWritable == value) return;
		Direct3D_internal::rs_zWritable = value;
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, DWORD(value) );
	}

	FORCEINLINE void Device3D::RS_Culling( bool value, bool backface /*= false*/ )
	{
		if (Direct3D_internal::rs_culling == value && Direct3D_internal::rs_backface == backface) return;
		Direct3D_internal::rs_culling = value;
		Direct3D_internal::rs_backface = backface;
		if ( value )
		{
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_CULLMODE, backface ? D3DCULL_CW : D3DCULL_CCW );
		}
		else
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );
	}

	FORCEINLINE void Device3D::RS_Lighting( bool value )
	{
		if (Direct3D_internal::rs_lighting == value) return;
		Direct3D_internal::rs_lighting = value;
		Direct3D_internal::m_pDevice->SetRenderState( D3DRS_LIGHTING, DWORD(value) );
	}

	FORCEINLINE void Device3D::RS_VertexColor( bool value )
	{
		if (Direct3D_internal::rs_vertexColor == value) return;
		Direct3D_internal::rs_vertexColor = value;
		Direct3D_internal::m_pDevice->SetRenderState( D3DRS_COLORVERTEX, DWORD(value) );
	}

	FORCEINLINE void Device3D::RS_WireFrame( bool value )
	{
		if (Direct3D_internal::rs_wireFrame == value) return;
		Direct3D_internal::rs_wireFrame = value;
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FILLMODE, 3-DWORD(value));
	}

	FORCEINLINE void Device3D::RS_Fog( bool value )
	{
		if (Direct3D_internal::rs_fog == value) return;

		if (Direct3D_internal::m_fog_Desc.Mode == D3DFOG_NONE)
		{
			Direct3D_internal::rs_fog = false;
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		}
		else
		{
			Direct3D_internal::rs_fog = value;
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGENABLE, DWORD(value));
		}
	}

	void Device3D::RS_AntialiasLine( bool value )
	{
		return;

		if (value)
		{
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, DWORD(value));
			//	Device3D_internal::m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, DWORD(value));
		}
		else
		{
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, DWORD(value));
			//	Device3D_internal::m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, DWORD(Device3D_internal::rs_alphaBlend));
		}
	}

	void Device3D::RS_TextureFilter( bool value )
	{
		if (value)
		{
			//d3d::Device3D::GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
			//d3d::Device3D::GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
			//d3d::Device3D::GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
		}
		else
		{
			//d3d::Device3D::GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			//d3d::Device3D::GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			//d3d::Device3D::GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		}
	}

	FORCEINLINE void Device3D::AlphaTest_Ref( UINT Value )
	{
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_ALPHAREF, Value);
	}


	FORCEINLINE void Device3D::Alpha_Reset( void )
	{
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	FORCEINLINE void Device3D::Alpha_Function( UINT SRC_Blend, UINT DES_Blend )
	{
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_SRCBLEND,	SRC_Blend);
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_DESTBLEND,	DES_Blend);
	}

	FORCEINLINE void Device3D::Alpha_SetToMaterial( UINT Stage )
	{
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	}

	void Device3D::Alpha_SetToTexture( UINT Stage )
	{
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE  );
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE  );
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_COLORARG1, D3DTA_TEXTURE  );
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_COLORARG2, D3DTA_DIFFUSE  );
		Direct3D_internal::m_pDevice->SetTextureStageState(Stage, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	}

	void Device3D::SetFogDesc( IN FogDesc& inDesc )
	{
		sx_callstack_push(Device3D::SetFogDesc());

		Direct3D_internal::m_fog_Desc = inDesc;

		// Set the fog color.
		Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGCOLOR, inDesc.Color);

		// Set fog parameters.
		if( inDesc.Mode == D3DFOG_LINEAR )
		{
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGTABLEMODE, inDesc.Mode);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&inDesc.Start));
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&inDesc.End));
		}
		else
		{
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGTABLEMODE, inDesc.Mode);
			Direct3D_internal::m_pDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&inDesc.Density));
		}

	}

	void Device3D::GetFogDesc( OUT FogDesc& outDesc )
	{
		outDesc = Direct3D_internal::m_fog_Desc;
	}

	FORCEINLINE bool Device3D::Shader_Supported( BYTE Major /*= 2*/, BYTE Minor /*= 0*/ )
	{
		return !(Direct3D_internal::m_caps.PixelShaderVersion<D3DPS_VERSION(Major, Minor));
	}

} } // namespace sx { namespace d3d

