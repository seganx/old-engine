#include "d3dDevice_gl.h"

#include "d3dVertexBuffer_gl.h"
#include "d3dIndexBuffer_gl.h"
#include "d3dTexture_gl.h"

#pragma comment (lib, "opengl32.lib")


const GLenum glPrimitiveTypes[] =
{
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_QUADS,
};


//////////////////////////////////////////////////////////////////////////
//	implementation
//////////////////////////////////////////////////////////////////////////
d3dDevice_gl::d3dDevice_gl( void )
{
	ZeroMemory( m_vertexBuffer,		sizeof(m_vertexBuffer)	);
	ZeroMemory( &m_indexBuffer,		sizeof(m_indexBuffer)	);
	ZeroMemory( m_textureBuffer,	sizeof(m_textureBuffer)	);
	ZeroMemory( &m_initParam,		sizeof(m_initParam)		);
	m_initParam.colorBits = 32;
	m_initParam.depthBits = 16;

	m_world.Identity();
	m_view.Identity();
	m_projection.Identity();

	m_rs_alpha = 0;
	m_rs_cull = 1;
	m_rs_fill = 1;
	m_rs_zenable = 1;
	m_rs_zwrite = 1;
}

d3dDevice_gl::~d3dDevice_gl( void )
{
	if ( m_vertexBufferArray.m_count )
		sx_logger( L"WARNING : all vertex buffers should release before releasing device !" );
	if ( m_indexBufferArray.m_count )
		sx_logger( L"WARNING : all index buffers should release before releasing device !" );
	if ( m_textureArray.m_count )
		sx_logger( L"WARNING : all textures should release before releasing device !" );
}

void d3dDevice_gl::Initialize( const handle displayHandle )
{
	m_initParam.hwnd = *( (HWND*)displayHandle );

	//	initialize fake GL to get capabilities
	{
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof (PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			m_initParam.colorBits,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			m_initParam.depthBits,
			m_initParam.stencilBits,
			0,
			PFD_MAIN_PLANE,
			0, 0, 0, 0
		};

		m_initParam.hdc = GetDC( m_initParam.hwnd );
		int pixelFormat = ChoosePixelFormat( m_initParam.hdc, &pfd );
		SetPixelFormat( m_initParam.hdc, pixelFormat, &pfd );
		m_initParam.context = wglCreateContext( m_initParam.hdc );
		wglMakeCurrent( m_initParam.hdc, m_initParam.context );

		initExtensions( m_initParam.hdc );
	}

	//	collect driver information
	{
		const char* strVendor = (const char*)glGetString(GL_VENDOR);
		for ( int i=0; i<256 && strVendor[i] && strVendor[i]!=' '; i++ )
			m_driverInfo.vendor[i] = strVendor[i];

		const char* strDesc = (const char*)glGetString(GL_RENDERER);
		for ( int i=0; i<256 && strDesc[i]; i++ )
			m_driverInfo.description[i] = strDesc[i];
	}

	//	collect device capabilities
	{
#if TURNON_STENCIL
		glGetIntegerv( GL_STENCIL_BITS, &m_initParam.stencilBits );
#endif
		glGetIntegerv( GL_DEPTH_BITS, &m_initParam.depthBits );

		switch ( m_initParam.depthBits )
		{
		case 16:
			m_driverDisplayMode.depthFormat = FMT_D16;
			break;
		case 24:
			m_driverDisplayMode.depthFormat = m_initParam.stencilBits ? FMT_D24S8 : FMT_D24X8;
			break;
		case 32:
			m_driverDisplayMode.depthFormat = FMT_D32;
			break;
		}

		int maxTextureSize = 0;
		glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxTextureSize );
		m_driverInfo.maxTextureSize = maxTextureSize;

		int maxMRT = 0;
		glGetIntegerv( GL_MAX_DRAW_BUFFERS_ARB, &maxMRT );
		m_driverInfo.maxMrtCount = maxMRT;

		int maxAniso = 0;
		glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso );
		m_driverInfo.maxAnisotropy = maxAniso;
	}

	//	get current display properties
	{
		DISPLAY_DEVICE device;	ZeroMemory( &device, sizeof(device) );
		device.cb = sizeof(device);
		EnumDisplayDevices( NULL, 0, &device, 0 );

		DEVMODE devMode;
		int i = 0;
		int maxcolorbits = 0;
		while ( EnumDisplaySettings( device.DeviceName, i, &devMode ) )
		{
			if ( maxcolorbits <= int( devMode.dmBitsPerPel ) )
			{
				maxcolorbits = int( devMode.dmBitsPerPel );
				if ( int(devMode.dmPelsWidth) == m_driverDisplayMode.width && int(devMode.dmPelsHeight) == m_driverDisplayMode.height )
				{
					m_driverDisplayMode.refereshRate = int(devMode.dmDisplayFrequency);
					m_driverDisplayMode.width		 = devMode.dmPelsWidth;
					m_driverDisplayMode.height		 = devMode.dmPelsHeight;

					switch ( maxcolorbits )
					{
					case 16:
					case 24:	m_driverDisplayMode.colorFormat = FMT_XRGB8;		break;
					case 32:	m_driverDisplayMode.colorFormat = FMT_ARGB8;		break;
					}					
				}
			}			
			i++;
		}
	}

	//	finalize fake GL
	{
		wglMakeCurrent( null, null );
		wglDeleteContext( m_initParam.context );
		ReleaseDC( m_initParam.hwnd, m_initParam.hdc );
	}

	//	initialize main GL
	{
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof (PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			m_initParam.colorBits,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			m_initParam.depthBits,
			m_initParam.stencilBits,
			0,
			PFD_MAIN_PLANE,
			0, 0, 0, 0
		};

		m_initParam.hdc = GetDC( m_initParam.hwnd );
		int pixelFormat = ChoosePixelFormat( m_initParam.hdc, &pfd );
		SetPixelFormat( m_initParam.hdc, pixelFormat, &pfd );
		m_initParam.context = wglCreateContext( m_initParam.hdc );
		wglMakeCurrent( m_initParam.hdc, m_initParam.context );

		initExtensions( m_initParam.hdc );

#if defined(_WIN32)
		WINDOWINFO winfo;
		GetWindowInfo( m_initParam.hwnd, &winfo );
		m_creationData.width	= winfo.rcClient.right - winfo.rcClient.left;
		m_creationData.height	= winfo.rcClient.bottom - winfo.rcClient.top;
#endif

		glClearDepth( 1.0f );
		glClearStencil( 0 );
		glDepthFunc( GL_LEQUAL );
 		glPixelStorei( GL_PACK_ALIGNMENT,   1 );
 		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );
		glHint( GL_TEXTURE_COMPRESSION_HINT, GL_NICEST );

		SetRenderState( RS_ZENABLE, true );
		SetRenderState( RS_FILL, true );
		SetRenderState( RS_CULL, SX_CULL_CCW );

		String slog = L"Graphic card information:";
		slog <<
			L"\r\n\tVendor:			"	<< m_driverInfo.vendor		<< 
			L"\r\n\tDescription:	"	<< m_driverInfo.description	<< 
			L"\r\n\tAPI:			OpenGL " << (char*)glGetString(GL_VERSION);

		slog << L"\r\n\tBackBuffer:		";
		switch ( m_initParam.colorBits )
		{
		case 32:	slog << L"ARGB8";	break;
		case 24:	slog << L"XRGB8";	break;
		default:	slog << L"UNKNOWN - WARNING !";
		}

		slog << L"\r\n\tDepth:			";
		switch ( m_initParam.depthBits )
		{
		case 16:	slog << L"D16";		break;
		case 38:	slog << L"D32";		break;
		case 24:	slog << ( m_initParam.stencilBits ? L"D24S8" : L"D24x8" );	break;
		default:	slog << L"UNKNOWN - WARNING !";
		}

		slog << L"\r\n";

		sx_logger( slog );
	}
}

void d3dDevice_gl::Finalize( void )
{
#if defined(_WIN32)
	wglMakeCurrent( null, null );
	wglDeleteContext( m_initParam.context );
	ReleaseDC( m_initParam.hwnd, m_initParam.hdc );
#endif
}

bool d3dDevice_gl::SetSize( const uint width, const uint height, const dword SX_D3D_ flag )
{
	//  validate data and save current flags
	if ( flag != -1 )	m_creationData.flag = flag;
	if ( width != -1 )	m_creationData.width = width ? width : m_driverDisplayMode.width;
	if ( height != -1 )	m_creationData.height = height ? height : m_driverDisplayMode.height;

	//  validate window size for full screen support
	if ( m_creationData.flag & SX_D3D_FULLSCREEN )
	{
		m_creationData.width = m_driverDisplayMode.width;
		m_creationData.height = m_driverDisplayMode.height;

#if defined(_WIN32)
		SetWindowLong( m_initParam.hwnd, GWL_STYLE, WS_POPUP );
		SetWindowLong( m_initParam.hwnd, GWL_EXSTYLE, 0 );
		SetWindowPos( m_initParam.hwnd, HWND_TOPMOST, 0, 0, m_creationData.width, m_creationData.height, SWP_NOOWNERZORDER );
		ShowWindow( m_initParam.hwnd, SW_SHOWDEFAULT );	
		UpdateWindow( m_initParam.hwnd );
#endif		
	}

	m_viewport.x = 0;
	m_viewport.y = 0;
	m_viewport.width = m_creationData.width;
	m_viewport.height = m_creationData.height;

	sx_logger( L"OpenGL device has been resized [ %d x %d ]", m_creationData.width, m_creationData.height );

	return true;
}

void d3dDevice_gl::CreateVertexBuffer( d3dVertexBuffer_gl*& OUT vertexBuffer )
{
	d3dVertexBuffer_gl* vb = sx_new( d3dVertexBuffer_gl );
	m_vertexBufferArray.push_back( vb );
	vb->m_device = this;
	vertexBuffer = vb;
}

void d3dDevice_gl::DestroyVertexBuffer( d3dVertexBuffer_gl*& IN_OUT vertexBuffer )
{
	if ( !vertexBuffer ) return;
	m_vertexBufferArray.remove( vertexBuffer );
	sx_delete_and_null( vertexBuffer );
}

SEGAN_INLINE void d3dDevice_gl::SetVertexBuffer( const d3dVertexBuffer_gl* vertexBuffer, uint streamIndex )
{
	if ( !vertexBuffer )
	{
		m_vertexBuffer[streamIndex].current = 0;
		return;
	}
	d3dVertexBuffer_gl* vbgl = (d3dVertexBuffer_gl*)vertexBuffer;
	m_vertexBuffer[streamIndex].current = vbgl->m_vbo;
}

void d3dDevice_gl::CreateIndexBuffer( d3dIndexBuffer_gl*& OUT indexBuffer )
{
	d3dIndexBuffer_gl* ib = sx_new( d3dIndexBuffer_gl );
	m_indexBufferArray.push_back( ib );
	ib->m_device = this;
	indexBuffer = ib;
}

void d3dDevice_gl::DestroyIndexBuffer( d3dIndexBuffer_gl*& IN_OUT indexBuffer )
{
	if ( !indexBuffer ) return;
	m_indexBufferArray.remove( indexBuffer );
	sx_delete_and_null( indexBuffer );
}

void d3dDevice_gl::SetIndexBuffer( const d3dIndexBuffer_gl* indexBuffer )
{
	if ( !indexBuffer )
	{
		m_indexBuffer.current = 0;
		return;
	}
	d3dIndexBuffer_gl* ibgl = (d3dIndexBuffer_gl*)indexBuffer;
	m_indexBuffer.current = ibgl->m_ibo;
}

void d3dDevice_gl::CreateTexture( d3dTexture*& OUT texture )
{
	d3dTexture_gl* tx = sx_new( d3dTexture_gl );
	m_textureArray.push_back( tx );
	tx->m_device = this;
	texture = tx;
}

void d3dDevice_gl::DestroyTexture( d3dTexture*& IN_OUT texture )
{
	if ( !texture ) return;
	m_textureArray.remove( texture );
	sx_delete_and_null( texture );
}

void d3dDevice_gl::SetTexture( const d3dTexture* texture, uint stage /*= 0 */ )
{
	if ( !texture )
	{
		m_textureBuffer[stage].current = 0;
		return;
	}
	d3dTexture_gl* txgl = (d3dTexture_gl*)texture;
	m_textureBuffer[stage].current = txgl->m_tbo;
	m_textureBuffer[stage].target = txgl->m_target;
}

SEGAN_INLINE void d3dDevice_gl::SetViewport( const d3dViewport* viewport )
{
	m_viewport = *viewport;
}

SEGAN_INLINE void d3dDevice_gl::SetMatrix( const d3dMatrixMode mode, const matrix& _matrix )
{
	switch ( mode )
	{
	case MM_WORLD:
		{
			m_world = _matrix;
			glMatrixMode( GL_MODELVIEW );
			glLoadMatrixf( sx_mul( m_world, m_view ) );
		}
		break;

	case MM_VIEW:
		{
			m_view = _matrix;

			glMatrixMode( GL_MODELVIEW );
			glLoadMatrixf( sx_mul( m_world, m_view ) );
		}
		break;

	case MM_PROJECTION:
		{
			m_projection = _matrix;
			glMatrixMode( GL_PROJECTION );
			glLoadMatrixf( &m_projection.m00 );
		}
		break;
	}
}

SEGAN_INLINE const matrix& d3dDevice_gl::GetMatrix( const d3dMatrixMode mode )
{
	switch ( mode )
	{
	case MM_WORLD:			return m_world;
	case MM_VIEW:			return m_view;
	case MM_PROJECTION:		return m_projection;
	}
	return m_world;
}

void d3dDevice_gl::SetRenderState( const d3dRenderState type, const uint mode )
{
	switch ( type )
	{
	case RS_ALPHA:
		if ( mode != m_rs_alpha )
		{
			switch ( mode )
			{
			case SX_ALPHA_BLEND:
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
				glBlendEquation( GL_FUNC_ADD );
				break;

			case SX_ALPHA_ADD:
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE );
				glBlendEquation( GL_FUNC_ADD );
				break;

			case SX_ALPHA_SUB:
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE );
				glBlendEquation( GL_FUNC_SUBTRACT );
				break;

			case SX_ALPHA_OFF:
				glDisable( GL_BLEND );
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
			case SX_CULL_CCW:	glEnable(  GL_CULL_FACE ); glFrontFace( GL_CCW );	break;
			case SX_CULL_CW:	glEnable(  GL_CULL_FACE ); glFrontFace( GL_CW );	break;
			case SX_CULL_OFF:	glDisable(  GL_CULL_FACE );	break;
			}
			m_rs_cull = mode;
		}
		break;

	case RS_FILL:
		if ( mode != m_rs_fill )
		{
			if ( mode )
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			m_rs_fill = mode;
		}
		break;

	case RS_ZENABLE:
		if ( mode != m_rs_zenable )
		{
			if ( mode )
				glEnable( GL_DEPTH_TEST );
			else
				glDisable( GL_DEPTH_TEST );
			m_rs_zenable = mode;
		}
		break;

	case RS_ZWRITE:
		if ( mode != m_rs_zwrite )
		{
			if ( mode )
				glDepthMask( GL_TRUE );
			else
				glDepthMask( GL_FALSE );
			m_rs_zwrite = mode;
		}
		break;
	}
}

uint d3dDevice_gl::GetRenderState( const d3dRenderState type )
{
	switch ( type)
	{
	case RS_ALPHA:		return m_rs_alpha;
	case RS_CULL:		return m_rs_cull;
	case RS_FILL:		return m_rs_fill;
	case RS_ZENABLE:	return m_rs_zenable;
	case RS_ZWRITE:		return m_rs_zwrite;
	default:			return 0;
	}
}

SEGAN_INLINE void d3dDevice_gl::DrawPrimitive(const d3dPrimitiveType primType, const int firstVertex, const int vertexCount)
{
	ApplyTextureBuffer();
	ApplyVertexBuffer();

	glDrawArrays( glPrimitiveTypes[primType], firstVertex, vertexCount );

	m_debugInfo.drawCalls++;
}

SEGAN_INLINE void d3dDevice_gl::DrawIndexedPrimitive(const d3dPrimitiveType primType, const int firstIndex, const int indicesCount, const int firstVertex, const int vertexCount)
{
	ApplyTextureBuffer();
	ApplyVertexBuffer();

	glDrawElements( glPrimitiveTypes[primType], indicesCount, GL_UNSIGNED_INT, BUFFER_OFFSET(firstIndex*4) );

	m_debugInfo.drawCalls++;
}


void d3dDevice_gl::DrawDebug( const d3dPrimitiveType primType, const uint vertxcount, const float* vertices, const Color& color )
{
	ApplyTextureBuffer();

	switch ( primType )
	{
	case PT_POINT:			glBegin( GL_POINTS );			break;
	case PT_LINE_LIST:		glBegin( GL_LINES );			break;
	case PT_LINE_STRIP:		glBegin( GL_LINE_STRIP );		break;
	case PT_TRIANGLE_LIST:	glBegin( GL_TRIANGLES );		break;
	case PT_TRIANGLE_STRIP:	glBegin( GL_TRIANGLE_STRIP );	break;
	case PT_TRIANGLE_FAN:	glBegin( GL_TRIANGLE_FAN );		break;
	case PT_QUAD_LIST:		glBegin( GL_QUADS );			break;
	default: return;
	}

	//Color c( color );
	glColor4ub( color.b, color.g, color.r, color.a );
	const float* pos = vertices;
	for ( uint i=0; i<vertxcount; ++i )
	{
		float x = *pos++, y = *pos++, z = *pos++;
		glVertex3f( x, y, z );
	}

	glEnd();

}


bool d3dDevice_gl::BeginScene( void )
{
#if defined(_WIN32)
	InvalidateRect( m_initParam.hwnd, NULL, FALSE );
#endif

	glViewport( m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height );

	return true;
}

void d3dDevice_gl::EndScene( void )
{

}

void d3dDevice_gl::Present( void )
{
	static int    s_frameCount	= 0;
	static double s_countTime	= 0;
	static double s_frameTime	= 0;//timeGetTime();// sx_os_get_timer();

	//	swap buffers to display
	if ( WGL_EXT_swap_control_supported )
		wglSwapIntervalEXT( (m_creationData.flag & SX_D3D_VSYNC) ? 1 : 0 );
	SwapBuffers( m_initParam.hdc );
	s_frameCount++;

	double curTime = 0;//timeGetTime();// sx_os_get_timer();
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

void d3dDevice_gl::ClearScreen( const Color& bgcolor )
{
	GLbitfield clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	float r = bgcolor.r / 255.0f;
	float g = bgcolor.g / 255.0f;
	float b = bgcolor.b / 255.0f;
	float a = bgcolor.a / 255.0f;
	glClearColor( r, g, b, a );

	if ( m_driverDisplayMode.depthFormat == FMT_D24S8 )
		clearBits |= GL_STENCIL_BUFFER_BIT;

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_TRUE );

	glClear( clearBits );
}

void d3dDevice_gl::ClearTarget( const Color& bgcolor )
{
	float r = bgcolor.r / 255.0f;
	float g = bgcolor.g / 255.0f;
	float b = bgcolor.b / 255.0f;
	float a = bgcolor.a / 255.0f;
	glClearColor( r, g, b, a );

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_FALSE );

	glClear( GL_COLOR_BUFFER_BIT );
}

void d3dDevice_gl::ClearZBuffer( void )
{
	GLbitfield clearBits = GL_DEPTH_BUFFER_BIT;

	if ( m_driverDisplayMode.depthFormat == FMT_D24S8 )
		clearBits |= GL_STENCIL_BUFFER_BIT;

	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_TRUE );

	glClear( clearBits );
}

void d3dDevice_gl::SetClipPlane( const uint index, const float* pplane )
{

}

void d3dDevice_gl::GetClipPlane( const uint index, float* pplane )
{

}

SEGAN_INLINE void d3dDevice_gl::ApplyVertexBuffer( void )
{
	//	apply vertices to the device
	for ( int streamIndex = 0; streamIndex < 8; streamIndex++ )
	{
		BufferStates* vb = &( m_vertexBuffer[streamIndex] );

		if ( vb->lastOne == vb->current )
			continue;

		switch ( streamIndex )
		{
		case SX_VERTEX_POSITION:		//	position
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableClientState( GL_VERTEX_ARRAY );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexPointer( 3, GL_FLOAT, 0, 0 );
			}
			else
			{
				if ( vb->lastOne )
					glDisableClientState( GL_VERTEX_ARRAY );
			}
			
			break;

		case SX_VERTEX_NORMAL:		//	normal
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableClientState( GL_NORMAL_ARRAY );
				
				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glNormalPointer( GL_FLOAT, 0, 0 );
			}
			else
			{
				if ( vb->lastOne )
					glDisableClientState( GL_NORMAL_ARRAY );
			}
			break;

		case SX_VERTEX_UV0:		//	texture 0
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableClientState( GL_TEXTURE_COORD_ARRAY );

				glClientActiveTexture( GL_TEXTURE0 );
				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glTexCoordPointer( 2, GL_FLOAT, 0, 0 );
			}
			else
			{
				if ( vb->lastOne )
					glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			}
			break;

		case SX_VERTEX_TANGENT:		//	tangent
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 0 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
			}
			else
			{
				if ( vb->lastOne )
					glDisableVertexAttribArray( 0 );
			}
			break;

		case SX_VERTEX_UV1:		//	texture 1
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 1 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
			}
			else
			{
				if ( vb->lastOne )
					glDisableVertexAttribArray( 1 );
			}
			break;

		case SX_VERTEX_COLORS:		//	color
			if ( vb->current )
			{
				if ( !vb->lastOne )
				{
					glEnableVertexAttribArray( 2 );
					glEnableClientState( GL_COLOR_ARRAY );
				}

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 2, GL_BGRA, GL_UNSIGNED_BYTE, GL_FALSE, 8, BUFFER_OFFSET(0) );			//	color 0
				//glVertexAttribPointer( 2, GL_BGRA, GL_UNSIGNED_BYTE, GL_FALSE, 8, BUFFER_OFFSET(4) );			//	color 1

				//	enable fixed function pipeline
				glColorPointer( GL_BGRA, GL_UNSIGNED_BYTE, 8, BUFFER_OFFSET(0) );					//	color 0
				//glSecondaryColorPointer( GL_BGRA, GL_UNSIGNED_BYTE, 8, BUFFER_OFFSET(4) );			//	color 1
			}
			else
			{
				if ( vb->lastOne )
				{
					glDisableClientState( GL_COLOR_ARRAY );
					glDisableVertexAttribArray( 2 );
				}
			}
			break;

		case SX_VERTEX_BLENDINDICES:		//	blend indices
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 3 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 3, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, BUFFER_OFFSET(0) );
			}
			else
			{
				if ( vb->lastOne )
					glDisableVertexAttribArray( 3 );
			}
			break;

		case SX_VERTEX_BLENDWEIGHT:		//	blend weights
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 4 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
			}
			else
			{
				if ( vb->lastOne )
					glDisableVertexAttribArray( 4 );
			}
			break;
		}

		//	store last one
		vb->lastOne = vb->current;
	}


	//	apply indices to the device
	if ( m_indexBuffer.current != m_indexBuffer.lastOne )
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.current );
		m_indexBuffer.current = m_indexBuffer.lastOne;
	}
}

void d3dDevice_gl::ApplyTextureBuffer( void )
{
	for ( int stage = 0; stage < 8; stage++ )
	{
		BufferStates* tx = &( m_textureBuffer[stage] );

 		if ( tx->lastOne == tx->current )
 			continue;

		glActiveTexture( GL_TEXTURE0 + stage );

		if ( !tx->current )
		{
			glDisable( tx->lastTarget );
			sx_glBindTexture( tx->lastTarget, 0 );
		}
		else
		{
			if ( !tx->lastOne )
			{
				glEnable( tx->target );
			}
			else if ( tx->target != tx->lastTarget )
			{
				glDisable( tx->lastTarget );
				glEnable( tx->target );
			}

			sx_glBindTexture( tx->target, tx->current );

			if ( tx->lod != tx->lastLod )
				glTexEnvf( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, tx->lastLod = tx->lod );
		}

		tx->lastOne		= tx->current;
		tx->lastTarget	= tx->target;
	}
}




//////////////////////////////////////////////////////////////////////////
//	additional helper function
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE SEGAN_ENG_API void sx_glBindBuffer( GLenum target, GLuint buffer )
{
	static GLuint targets[2] = { 0, 0 };
	
	switch ( target )
	{
	case GL_ARRAY_BUFFER:
		{
			if ( targets[0] != buffer )
			{
				glBindBuffer( target, buffer );
				targets[0] = buffer;
			}
		}
		break;

	case GL_ELEMENT_ARRAY_BUFFER:
		{
			if ( targets[1] != buffer )
			{
				glBindBuffer( target, buffer );
				targets[1] = buffer;
			}	
		}
		break;
	}
}

SEGAN_INLINE SEGAN_ENG_API void sx_glBindTexture( GLenum target, GLuint texture )
{
	static GLuint targets[4] = { 0, 0, 0, 0 };

	switch ( target )
	{
	case GL_TEXTURE_1D:
		{
			if ( targets[0] != texture )
			{
				glBindTexture( target, texture );
				targets[0] = texture;
			}
		}
		break;

	case GL_TEXTURE_2D:
		{
			if ( targets[1] != texture )
			{
				glBindTexture( target, texture );
				targets[1] = texture;
			}
		}
		break;

	case GL_TEXTURE_3D:
		{
			if ( targets[2] != texture )
			{
				glBindTexture( target, texture );
				targets[2] = texture;
			}
		}
		break;

	case GL_TEXTURE_CUBE_MAP:
		{
			if ( targets[3] != texture )
			{
				glBindTexture( target, texture );
				targets[3] = texture;
			}
		}
		break;

	}
}