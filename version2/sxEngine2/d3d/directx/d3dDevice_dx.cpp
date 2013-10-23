#include "d3dDevice_dx.h"

#include "../../system/System.h"


const D3DPRIMITIVETYPE dxPrimitiveTypes[] =
{
	D3DPT_POINTLIST,
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,
	D3DPT_TRIANGLELIST	//	as QUAD
};


const D3DVERTEXELEMENT9 SeganVrtxDecl[] = {
	{0, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,		0 },

	{1, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,		0 },

	{2, 0,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		0 },

	{3, 0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TANGENT,		0 },

	{4, 0,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,		1 },

	{5, 0,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,			0 },
	{5, 4,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,			1 },

	{6, 0,	D3DDECLTYPE_UBYTE4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDINDICES,	0 },

	{7, 0,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_BLENDWEIGHT,	0 },

	D3DDECL_END()
};


HRESULT IsDepthFormatOK(LPDIRECT3D9 d3dInterface, UINT adapter, D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT DepthFormat, D3DFORMAT BackBufferFormat);
D3DFORMAT FindBestDepthFormat(LPDIRECT3D9 d3dInterface, UINT adapter, D3DCAPS9* pCaps, D3DFORMAT AdapterFormat);
d3dFormat ConvertFromD3DFormat( const uint format );



//////////////////////////////////////////////////////////////////////////
//	d3ddevice for DirectX implementation
//////////////////////////////////////////////////////////////////////////
d3dDevice_dx::d3dDevice_dx( void )
: d3dDevice()
, m_direct3D(0)
, m_device3D(0)
, m_rs_zenable(true)
, m_rs_zwrite(true)
, m_rs_cull(SX_CULL_CCW)
, m_rs_alpha(SX_ALPHA_OFF)
, m_rs_fill(true)
{
	ZeroMemory( &m_initData, sizeof(m_initData) );
}

d3dDevice_dx::~d3dDevice_dx( void )
{
	Finalize();
}

void d3dDevice_dx::Initialize( const handle displayHandle )
{
	if ( !displayHandle ) return;

	// Create Interface
	m_direct3D = Direct3DCreate9( D3D_SDK_VERSION );
	if ( !m_direct3D )
	{
		g_logger->Log( L"Error: I can not create Direct3D interface !" );
		return;
	}

	//  get correct display adapters

	D3DCAPS9 caps;
	uint adapter = -1;

	uint adapterCount = m_direct3D->GetAdapterCount();
	for ( uint i=0; i<adapterCount; i++ )
	{
		//  Get device capabilities for each adapter
		if ( FAILED( m_direct3D->GetDeviceCaps( i, D3DDEVTYPE_HAL, &caps ) ) )
		{
			g_logger->Log( L"Error: can not get device capabilities for display adapter %d !", i );
			continue;
		}

		if ( caps.DeviceType == D3DDEVTYPE_HAL && caps.DevCaps & D3DDEVCAPS_PUREDEVICE )
		{
			adapter = i;
			//g_logger->Log( L"Number of display adapters : %d - selected adapter : %d", adapterCount, adapter );
			break;
		}
	}

	if ( adapter == -1 )
	{
		g_logger->Log( L"Error: no display adapter found which support 3D acceleration !" );
		sx_release_and_null( m_direct3D );
		return;
	}

	m_driverCaps.maxMrtCount		= caps.NumSimultaneousRTs;
	m_driverCaps.maxTextureSize		= caps.MaxTextureWidth;
	m_driverCaps.maxAnisotropy		= (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY) ? caps.MaxAnisotropy : 0;

	// Get the current desktop display mode, so we can setup back buffer format
	D3DDISPLAYMODE curDisplayMode;
	if ( FAILED( m_direct3D->GetAdapterDisplayMode( adapter, &curDisplayMode ) ) )
	{
		g_logger->Log( L"Error: I can not get adapter display mode !" );
		sx_release_and_null( m_direct3D );
		return;
	}
	m_driverDisplayMode.width			= curDisplayMode.Width;
	m_driverDisplayMode.height			= curDisplayMode.Height;
	m_driverDisplayMode.refereshRate	= curDisplayMode.RefreshRate;
	m_driverDisplayMode.colorFormat		= ConvertFromD3DFormat( curDisplayMode.Format );
	m_driverDisplayMode.depthFormat		= ConvertFromD3DFormat( FindBestDepthFormat( m_direct3D, adapter, &caps, curDisplayMode.Format ) );

	// get adapter and driver information
	D3DADAPTER_IDENTIFIER9 adinfo;
	ZeroMemory( &adinfo, sizeof(adinfo) );
	if ( FAILED( m_direct3D->GetAdapterIdentifier( adapter, 0, &adinfo ) ) )
	{
		g_logger->Log( L"Error: I can not get driver information !" );
		sx_release_and_null( m_direct3D );
		return;
	}

	int vendoroffset = 1;
	for ( int i=0; i<256 && adinfo.Description[i] && adinfo.Description[i]!=' '; i++, vendoroffset++ )
		m_driverInfo.vendor[i] = adinfo.Description[i];
	
	for ( int i=0; i<256 && adinfo.Description[i]; i++ )
		m_driverInfo.description[i] = adinfo.Description[i + vendoroffset];


	//////////////////////////////////////////////////////////////////////////

	// Set up the structure used to create the D3DDevice...
	m_initData.prentParam.BackBufferFormat				= curDisplayMode.Format;
	m_initData.prentParam.BackBufferCount				= 0;
	m_initData.prentParam.Flags							= 0;
	m_initData.prentParam.MultiSampleQuality			= 0;
	m_initData.prentParam.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	m_initData.prentParam.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_initData.prentParam.EnableAutoDepthStencil		= FALSE;
	m_initData.prentParam.AutoDepthStencilFormat		= FindBestDepthFormat( m_direct3D, adapter, &caps, curDisplayMode.Format );
	m_initData.prentParam.BackBufferWidth				= 0;
	m_initData.prentParam.BackBufferHeight				= 0;
	m_initData.prentParam.hDeviceWindow					= *( (HWND*)displayHandle );
	m_initData.prentParam.Windowed						= TRUE;
	m_initData.prentParam.FullScreen_RefreshRateInHz	= 0;
	m_initData.prentParam.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

	
	// Create the Direct3D device...............................................
	if ( FAILED ( m_direct3D->CreateDevice(
		adapter,
		D3DDEVTYPE_HAL,
		m_initData.prentParam.hDeviceWindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING /*| D3DCREATE_PUREDEVICE*/, //  I discard this flag because of "http://msdn.microsoft.com/en-us/library/ee416788.aspx#What_is_the_purpose_of_the_D3DCREATE_PUREDEVICE_flag"
		&m_initData.prentParam,
		&m_device3D
		) ) )
	{
		g_logger->Log( L"Error: I created Direct3D interface but I can not create Direct3D Device !" );
		return;
	}

	//  create necessary resources and settings
	PostReset();

	// turn off all lights
	for (UINT i=0; i<caps.MaxActiveLights; i++)
		m_device3D->LightEnable(i, FALSE);

	// Reset d3dDevice settings
	ResetStates();

	String slog = L"Graphic card information:";
	slog <<
		L"\r\n\tVendor:			"		<< m_driverInfo.vendor		<< 
		L"\r\n\tDescription:	"		<< m_driverInfo.description	<< 
		L"\r\n\tAPI:			DirectX 9"	<<
		L"\r\n\tBackBuffer:		";
	switch ( m_initData.prentParam.BackBufferFormat )
	{
	case D3DFMT_X8R8G8B8:	slog << L"X8R8G8B8";	break;
	case D3DFMT_A8R8G8B8:	slog << L"A8R8G8B8";	break;
	case D3DFMT_X8B8G8R8:	slog << L"X8R8G8B8";	break;
	case D3DFMT_A8B8G8R8:	slog << L"A8R8G8B8";	break;
	default:				slog << L"UNKNOWN - WARNING !";
	}

	slog << L"\r\n\tDepth:			";
	switch ( m_initData.prentParam.AutoDepthStencilFormat )
	{
	case D3DFMT_D16:	slog << L"D16";		break;
	case D3DFMT_D24X8:	slog << L"D24x8";	break;
	case D3DFMT_D24S8:	slog << L"D24S8";	break;
	case D3DFMT_D32:	slog << L"D32";		break;
	default:			slog << L"UNKNOWN - WARNING !";
	}

	slog << L"\r\n";
	g_logger->Log( slog );

}

void d3dDevice_dx::Finalize( void )
{
	if ( !m_direct3D ) return;

	sx_release_and_null( m_device3D );
	sx_release_and_null( m_direct3D );
}

bool d3dDevice_dx::SetSize( const uint width, const uint height, const dword SX_D3D_ flag )
{
	sx_assert( m_device3D );

	//  validate data and save current flags
	if ( flag != -1 )	m_creationData.flag = flag;
	if ( width != -1 )	m_creationData.width = width ? width : m_driverDisplayMode.width;
	if ( height != -1 )	m_creationData.height = height ? height : m_driverDisplayMode.height;

	// Set up the structure used to create the D3DDevice...

	//  set display size
	m_initData.prentParam.BackBufferWidth				= m_creationData.width;
	m_initData.prentParam.BackBufferHeight				= m_creationData.height;

	//  set device behavior
	m_initData.prentParam.Windowed						= ( m_creationData.flag & SX_D3D_FULLSCREEN ) ? FALSE : TRUE;
	m_initData.prentParam.FullScreen_RefreshRateInHz	= ( m_creationData.flag & SX_D3D_FULLSCREEN ) ? m_driverDisplayMode.refereshRate : 0;
	m_initData.prentParam.PresentationInterval			= ( m_creationData.flag & SX_D3D_VSYNC ) ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;

	//  validate window size for full screen support
	if ( m_creationData.flag & SX_D3D_FULLSCREEN )
	{
		HWND hWnd = m_initData.prentParam.hDeviceWindow;
		SetWindowLong( hWnd, GWL_STYLE, WS_POPUP );
		SetWindowLong( hWnd, GWL_EXSTYLE, 0 );
		SetWindowPos( hWnd, HWND_TOPMOST, 0, 0, m_creationData.width, m_creationData.height, SWP_NOOWNERZORDER );
		ShowWindow( hWnd, SW_SHOWDEFAULT );	
		UpdateWindow( hWnd );
	}

	//	release resources
	PreReset();

	if ( FAILED ( m_device3D->Reset( &m_initData.prentParam ) ) )
	{
		g_logger->Log( L"Error: reseting direct3D device failed" );
		return false;
	}

	//  create necessary resources and settings
	PostReset();

	// Reset d3dDevice settings
	ResetStates();

	return true;
}

void d3dDevice_dx::CreateVertexBuffer( d3dVertexBuffer*& OUT vertexBuffer )
{

}

void d3dDevice_dx::DestroyVertexBuffer( d3dVertexBuffer*& IN_OUT vertexBuffer )
{

}

void d3dDevice_dx::SetVertexBuffer( const d3dVertexBuffer* vertexBuffer, uint streamIndex )
{

}

void d3dDevice_dx::CreateIndexBuffer( d3dIndexBuffer*& OUT indexBuffer )
{

}

void d3dDevice_dx::DestroyIndexBuffer( d3dIndexBuffer*& IN_OUT indexBuffer )
{

}

void d3dDevice_dx::CreateTexture( d3dTexture*& OUT texture )
{

}

void d3dDevice_dx::DestroyTexture( d3dTexture*& IN_OUT texture )
{

}

void d3dDevice_dx::SetTexture( const d3dTexture* texture, uint stage /*= 0 */ )
{

}

void d3dDevice_dx::CreateShader( d3dShader*& OUT shader )
{

}

void d3dDevice_dx::DestroyShader( d3dShader*& IN_OUT shader )
{

}

SEGAN_INLINE void d3dDevice_dx::SetViewport( const d3dViewport* viewport )
{
	sx_assert( m_device3D );

	m_viewport = *viewport;
	D3DVIEWPORT9 vp;
	vp.X		= viewport->x;
	vp.Y		= viewport->y;
	vp.Width	= viewport->width;
	vp.Height	= viewport->height;
	vp.MinZ		= 0;
	vp.MaxZ		= 1;
	m_device3D->SetViewport( &vp );
}

void d3dDevice_dx::SetMatrix( const d3dMatrixMode mode, const matrix& _matrix )
{
	switch ( mode )
	{
	case MM_WORLD:
		m_world = _matrix;
		m_device3D->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m_world );
		break;

	case MM_VIEW:
		m_view = _matrix;
		m_device3D->SetTransform( D3DTS_VIEW, (D3DMATRIX*)&m_view );
		break;

	case MM_PROJECTION:
		m_projection = _matrix;
		m_device3D->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&m_projection );
		break;
	}
}

const matrix& d3dDevice_dx::GetMatrix( const d3dMatrixMode mode )
{
	switch ( mode )
	{
	case MM_WORLD:			return m_world;
	case MM_VIEW:			return m_view;
	case MM_PROJECTION:		return m_projection;
	}
	return m_world;
}

void d3dDevice_dx::SetRenderState( const d3dRenderState type, const uint mode )
{
	switch ( type )
	{
	case RS_ALPHA:
		if ( mode != m_rs_alpha )
		{
			switch ( mode )
			{
			case SX_ALPHA_BLEND:
				m_device3D->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				m_device3D->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
				m_device3D->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_ADD );
				m_device3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				break;

			case SX_ALPHA_ADD:
				m_device3D->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				m_device3D->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
				m_device3D->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_ADD );
				m_device3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				break;

			case SX_ALPHA_SUB:
				m_device3D->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				m_device3D->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
				m_device3D->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_REVSUBTRACT );
				m_device3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				break;

			case SX_ALPHA_OFF:
				m_device3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				break;
			}
			m_rs_alpha = mode;
		}
		break;

	case RS_CULL:
		if ( mode != m_rs_cull )
		{
			switch ( mode )
			{
			case SX_CULL_CCW:	m_device3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );	break;
			case SX_CULL_CW:	m_device3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );	break;
			case SX_CULL_OFF:	m_device3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	break;
			}
			m_rs_cull = mode;
		}
		break;

	case RS_FILL:
		if ( mode != m_rs_fill )
		{
			if ( mode )
				m_device3D->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
			else
				m_device3D->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			m_rs_fill = mode;
		}
		break;

	case RS_ZENABLE:
		if ( mode != m_rs_zenable )
		{
			if ( mode )
				m_device3D->SetRenderState( D3DRS_ZENABLE, TRUE );
			else
				m_device3D->SetRenderState( D3DRS_ZENABLE, FALSE );
			m_rs_zenable = mode;
		}
		break;

	case RS_ZWRITE:
		if ( mode != m_rs_zwrite )
		{
			if ( mode )
				m_device3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			else
				m_device3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			m_rs_zwrite = mode;
		}
		break;
	}
}

uint d3dDevice_dx::GetRenderState( const d3dRenderState type )
{
	return 0;
}

void d3dDevice_dx::DrawPrimitive( const d3dPrimitiveType primType, const int firstVertex, const int vertexCount )
{

}

void d3dDevice_dx::DrawIndexedPrimitive( const d3dPrimitiveType primType, const int firstIndex, const int indicesCount, const int firstVertex, const int vertexCount )
{
	//m_device3D->DrawIndexedPrimitive()
}

void d3dDevice_dx::DrawDebug( const d3dPrimitiveType primType, const uint vertxcount, const float* vertices, const Color& color )
{
	D3DMATERIAL9 mtl; ZeroMemory( &mtl, sizeof(mtl) );
	mtl.Ambient = D3DXCOLOR(color);
	mtl.Diffuse = mtl.Ambient;
	m_device3D->SetMaterial( &mtl );

	uint primCount = 0;
	switch ( primType )
	{
	case PT_POINT:			primCount = vertxcount;			break;
	case PT_LINE_LIST:		primCount = vertxcount / 2;		break;
	case PT_LINE_STRIP:		primCount = vertxcount - 1;		break;
	case PT_TRIANGLE_LIST:	primCount = vertxcount / 3;		break;
	case PT_TRIANGLE_STRIP:	primCount = vertxcount - 2;		break;
	case PT_TRIANGLE_FAN:	primCount = vertxcount - 2;		break;
	case PT_QUAD_LIST:		primCount = vertxcount / 3;		break;
	}

	m_device3D->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_device3D->DrawPrimitiveUP( dxPrimitiveTypes[primType], primCount, vertices, sizeof(float3) );
	m_device3D->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

bool d3dDevice_dx::BeginScene( void )
{
	sx_assert( m_device3D );

	switch ( m_device3D->TestCooperativeLevel() )
	{
	case D3DERR_DEVICELOST:	return false;
	case D3DERR_DEVICENOTRESET:

		g_logger->Log( L"Warning: The direct3D device is lost! I'm trying to reset it !" );
		if ( SetSize( m_creationData.width, m_creationData.height, m_creationData.flag ) )
		{
			g_logger->Log( L"The direct3D device is now ready !" );
		}
		else
		{
			g_logger->Log( L"Error: Reseting direct3D device failed !" );
			sx_os_sleep(100);
			return false;
		}
	}

	ResetStates();

	return SUCCEEDED( m_device3D->BeginScene() );
}

void d3dDevice_dx::EndScene( void )
{
	sx_assert( m_device3D );
	m_device3D->EndScene();
}

void d3dDevice_dx::Present( void )
{
	sx_assert( m_device3D );
	static int    s_frameCount	= 0;
	static double s_countTime	= 0;
	static double s_frameTime	= sx_os_get_timer();
	
	m_device3D->Present( null, null, null, null );
	s_frameCount++;

	double curTime = sx_os_get_timer();
	double elpTime = curTime - s_frameTime;
	s_frameTime = curTime;

	s_countTime += elpTime;
	if ( s_countTime > 999.5 )
	{
		m_debugInfo.fps = s_frameCount;
		s_frameCount = 0;
		s_countTime = 0;
	}

	m_debugInfo.frameTime = (float)elpTime;
}

void d3dDevice_dx::ClearScreen( const Color& bgcolor )
{
	sx_assert( m_device3D );

	m_device3D->Clear( 0, null, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, bgcolor, 1.0f, 0 );
}

void d3dDevice_dx::ClearTarget( const Color& bgcolor )
{
	sx_assert( m_device3D );

	m_device3D->Clear( 0, null, D3DCLEAR_TARGET, bgcolor, 1.0f, 0 );
}

void d3dDevice_dx::ClearZBuffer( void )
{
	sx_assert( m_device3D );

	m_device3D->Clear( 0, null, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
}

void d3dDevice_dx::SetClipPlane( const uint index, const float* pplane )
{
	sx_assert( m_device3D );

	m_device3D->SetClipPlane( index, pplane );
	
}

void d3dDevice_dx::GetClipPlane( const uint index, float* pplane )
{
	sx_assert( m_device3D );

	m_device3D->GetClipPlane( index, pplane );
}

void d3dDevice_dx::PreReset( void )
{
	LPDIRECT3DVERTEXDECLARATION9 vertexDecl = null;
	m_device3D->GetVertexDeclaration( &vertexDecl );
	sx_release_and_null( vertexDecl );

	sx_release_and_null( m_initData.depthSurface );
	sx_release_and_null( m_initData.depthTexture );
	sx_release_and_null( m_initData.colorSurface );
	sx_release_and_null( m_initData.resetStateBlock );
}

void d3dDevice_dx::PostReset( void )
{
	// Create Vertex declaration 
	LPDIRECT3DVERTEXDECLARATION9 vertexDecl;
	m_device3D->CreateVertexDeclaration( SeganVrtxDecl, &vertexDecl );
	m_device3D->SetVertexDeclaration( vertexDecl );

	// get main target color surface
	m_device3D->GetRenderTarget( 0, &m_initData.colorSurface );

	// verify that depth stencil surface is not created
	LPDIRECT3DSURFACE9 depthSurface = null;
	m_device3D->GetDepthStencilSurface( &depthSurface );
	if ( depthSurface )
	{
		g_logger->Log( L"Warning: device created with depth stencil surface! I released it manually!" );
		sx_release( depthSurface );
		sx_release_and_null( depthSurface );
	}
	m_device3D->SetDepthStencilSurface( null );

	//  create depth surface as texture
	D3DSURFACE_DESC colorSurfaceDesc;
	m_initData.colorSurface->GetDesc( &colorSurfaceDesc );
	m_device3D->CreateTexture( 
		colorSurfaceDesc.Width,
		colorSurfaceDesc.Height,
		1,
		D3DUSAGE_DEPTHSTENCIL,
		m_initData.prentParam.AutoDepthStencilFormat,
		D3DPOOL_DEFAULT,
		&m_initData.depthTexture,
		null
		);
	if ( !m_initData.depthTexture )
	{
		g_logger->Log( L"Error: can't create depth texture !" );
		Finalize();
		return;
	}
	m_initData.depthTexture->GetSurfaceLevel( 0, &m_initData.depthSurface );
	m_device3D->SetDepthStencilSurface( m_initData.depthSurface );

	// set alpha test
	m_device3D->SetRenderState(D3DRS_ALPHAREF, 156);
	m_device3D->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//  set texture state stages
	for ( UINT i=0; i<8; i++ )
	{
		m_device3D->SetTextureStageState( i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE   );
		m_device3D->SetTextureStageState( i, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE   );
		m_device3D->SetTextureStageState( i, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_device3D->SetTextureStageState( i, D3DTSS_COLORARG1, D3DTA_TEXTURE   );
		m_device3D->SetTextureStageState( i, D3DTSS_COLORARG2, D3DTA_DIFFUSE   );
		m_device3D->SetTextureStageState( i, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	}

	//	set sampler states
	for ( UINT i=0; i<8; i++ )
	{
		m_device3D->SetSamplerState( i, D3DSAMP_BORDERCOLOR, 0xffffffff );

		if ( i < 4 )
		{
			// try to use Anisotropy
			if ( m_driverCaps.maxAnisotropy )
				m_device3D->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, m_driverCaps.maxAnisotropy );

			m_device3D->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_device3D->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_device3D->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		}
		else
		{
			m_device3D->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			m_device3D->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_device3D->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		}
	}

	// save default view port
	D3DVIEWPORT9 vp9;
	m_device3D->GetViewport( &vp9 );
	m_defaultViewport.x			= vp9.X;
	m_defaultViewport.y			= vp9.Y;
	m_defaultViewport.width		= vp9.Width;
	m_defaultViewport.height	= vp9.Height;
	m_viewport = m_defaultViewport;
}

void d3dDevice_dx::ResetStates( void )
{
	sx_assert( m_device3D );

	m_rs_zenable		= true;
	m_rs_zwrite			= true;
	m_rs_cull			= SX_CULL_CCW;
	m_rs_alpha			= SX_ALPHA_OFF;
	m_rs_fill			= true;

	m_world.Identity();
	m_view.Identity();
	m_projection.Identity();

	if ( !m_initData.resetStateBlock )
	{
		// create the state block for fast reinitialization in the next time
		m_device3D->BeginStateBlock();

		// render state initialization
		m_device3D->SetRenderState( D3DRS_ZENABLE,					TRUE );
		m_device3D->SetRenderState( D3DRS_ZWRITEENABLE,				TRUE );
		m_device3D->SetRenderState( D3DRS_CULLMODE,					D3DCULL_CW );
		m_device3D->SetRenderState( D3DRS_ALPHABLENDENABLE,			FALSE );
		m_device3D->SetRenderState( D3DRS_FILLMODE,					D3DFILL_SOLID );
		m_device3D->SetRenderState( D3DRS_FOGENABLE,				FALSE );
		
		m_device3D->SetRenderState( D3DRS_LIGHTING,					TRUE );
		m_device3D->SetRenderState( D3DRS_DITHERENABLE,				TRUE );
		m_device3D->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE,	FALSE );
		m_device3D->SetRenderState( D3DRS_ALPHATESTENABLE,			FALSE );
		m_device3D->SetRenderState( D3DRS_COLORVERTEX,				FALSE );
		m_device3D->SetRenderState( D3DRS_SPECULARENABLE,			FALSE );
	
		m_device3D->SetRenderState( D3DRS_AMBIENT,					0xffffffff );
		m_device3D->SetRenderState( D3DRS_SRCBLEND,					D3DBLEND_SRCALPHA );
		m_device3D->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_INVSRCALPHA );

		m_device3D->SetRenderState( D3DRS_COLORWRITEENABLE,	
			D3DCOLORWRITEENABLE_ALPHA | 
			D3DCOLORWRITEENABLE_BLUE | 
			D3DCOLORWRITEENABLE_GREEN | 
			D3DCOLORWRITEENABLE_RED );

		// setup buffers
		for ( UINT i=0; i<8; i++ )
		{
			m_device3D->SetTexture( i, NULL );
			m_device3D->SetStreamSource( i, NULL, 0, 0 );
		}
		m_device3D->SetIndices( NULL );
		m_device3D->SetVertexShader( NULL );
		m_device3D->SetPixelShader( NULL );

		m_device3D->SetTransform( D3DTS_WORLD,		(D3DMATRIX*)&m_world );
		m_device3D->SetTransform( D3DTS_VIEW,		(D3DMATRIX*)&m_view );
		m_device3D->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&m_projection );

		SetViewport( &m_defaultViewport );

		m_device3D->EndStateBlock( &m_initData.resetStateBlock );
	}

	m_initData.resetStateBlock->Apply();
}

void d3dDevice_dx::SetIndexBuffer( const d3dIndexBuffer* indexBuffer )
{

}


//////////////////////////////////////////////////////////////////////////
//	other implementations
//////////////////////////////////////////////////////////////////////////
HRESULT IsDepthFormatOK( LPDIRECT3D9 d3dInterface, UINT adapter, D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT DepthFormat, D3DFORMAT BackBufferFormat )
{
	// Verify that the depth format exists
	HRESULT result = d3dInterface->CheckDeviceFormat(
		adapter,
		pCaps->DeviceType,
		AdapterFormat,
		D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE,
		DepthFormat
		);

	if ( FAILED( result ) ) return result;

	// Verify that the depth format is compatible
	return d3dInterface->CheckDepthStencilMatch(
		adapter,
		pCaps->DeviceType,
		AdapterFormat,
		BackBufferFormat,
		DepthFormat
		);
}

D3DFORMAT FindBestDepthFormat( LPDIRECT3D9 d3dInterface, UINT adapter, D3DCAPS9* pCaps, D3DFORMAT AdapterFormat )
{
	//  first try to 32 bit depth buffer and then 24 bit and so on...
	if ( SUCCEEDED( IsDepthFormatOK( d3dInterface, adapter, pCaps, AdapterFormat, D3DFMT_D32, AdapterFormat ) ) )
		return D3DFMT_D32;

	if ( SUCCEEDED( IsDepthFormatOK( d3dInterface, adapter, pCaps, AdapterFormat, D3DFMT_D24S8, AdapterFormat ) ) )	
		return D3DFMT_D24S8;

	if ( SUCCEEDED( IsDepthFormatOK( d3dInterface, adapter, pCaps, AdapterFormat, D3DFMT_D24X8, AdapterFormat ) ) )
		return D3DFMT_D24X8;

	return D3DFMT_D16;
}

d3dFormat ConvertFromD3DFormat( const uint format )
{
	switch ( format )
	{
	case D3DFMT_L8:				return FMT_L8;
	case D3DFMT_A8L8:			return FMT_A8L8;
	case D3DFMT_X8R8G8B8:		return FMT_XRGB8;
	case D3DFMT_A8R8G8B8:		return FMT_ARGB8;
	
	case D3DFMT_DXT1:			return FMT_DXT1;
	case D3DFMT_DXT5:			return FMT_DXT5;

	case D3DFMT_D16:			return FMT_D16;
	case D3DFMT_D24X8:			return FMT_D24X8;
	case D3DFMT_D24S8:			return FMT_D24S8;
	case D3DFMT_D32:			return FMT_D32;

	default:					return FMT_UNKNOWN;
	}
}