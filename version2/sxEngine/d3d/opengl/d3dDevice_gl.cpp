#include "d3dDevice_gl.h"

#include "../../system/System.h"
#include "../../math/Math.h"

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
	GL_TRIANGLE_FAN
};


//////////////////////////////////////////////////////////////////////////
//	implementation
//////////////////////////////////////////////////////////////////////////
d3dDevice_gl::d3dDevice_gl( void )
: d3dDevice()
{
	ZeroMemory( m_vertexBuffer,		sizeof(m_vertexBuffer)	);
	ZeroMemory( &m_indexBuffer,		sizeof(m_indexBuffer)	);
	ZeroMemory( m_textureBuffer,	sizeof(m_textureBuffer)	);
	ZeroMemory( &m_initParam,		sizeof(m_initParam)		);
	m_initParam.colorBits = 32;
	m_initParam.depthBits = 16;

}

d3dDevice_gl::~d3dDevice_gl( void )
{
	if ( m_vertexBufferArray.Count() )
		g_logger->Log( L"WARNING : all vertex buffers should release before releasing device !" );
	if ( m_indexBufferArray.Count() )
		g_logger->Log( L"WARNING : all index buffers should release before releasing device !" );
	if ( m_textureArray.Count() )
		g_logger->Log( L"WARNING : all textures should release before releasing device !" );
	if ( m_shaderArray.Count() )
		g_logger->Log( L"WARNING : all shaders should release before releasing device !" );
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
		m_driverCaps.maxTextureSize = maxTextureSize;

		int maxMRT = 0;
		glGetIntegerv( GL_MAX_DRAW_BUFFERS_ARB, &maxMRT );
		m_driverCaps.maxMrtCount = maxMRT;

		int maxAniso = 0;
		glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso );
		m_driverCaps.maxAnisotropy = maxAniso;
	}

	//	get current display properties
	{
		m_driverDisplayMode.width = sx_os_get_monitor()->areaWidth;
		m_driverDisplayMode.height= sx_os_get_monitor()->areaHeight;

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

		glEnable( GL_DEPTH_TEST );
		glClearDepth( 1.0f );
		glClearStencil( 0 );
		glDepthFunc( GL_LEQUAL );
		glFrontFace( GL_CCW );
 		glPixelStorei( GL_PACK_ALIGNMENT,   1 );
 		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );
		glHint( GL_TEXTURE_COMPRESSION_HINT, GL_NICEST );

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

		g_logger->Log( slog );
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

	m_viewport.width = m_creationData.width;
	m_viewport.height = m_creationData.height;

	g_logger->Log_( L"OpenGL device has been resized [ %d x %d ]", m_creationData.width, m_creationData.height );

	return true;
}

void d3dDevice_gl::CreateVertexBuffer( d3dVertexBuffer*& OUT vertexBuffer )
{
	d3dVertexBuffer_gl* vb = sx_new( d3dVertexBuffer_gl );
	m_vertexBufferArray.PushBack( vb );
	vb->m_device = this;
	vertexBuffer = vb;
}

void d3dDevice_gl::DestroyVertexBuffer( d3dVertexBuffer*& IN_OUT vertexBuffer )
{
	if ( !vertexBuffer ) return;
	m_vertexBufferArray.Remove( vertexBuffer );
	sx_delete_and_null( vertexBuffer );
}

SEGAN_INLINE void d3dDevice_gl::SetVertexBuffer( const d3dVertexBuffer* vertexBuffer, uint streamIndex )
{
	if ( !vertexBuffer )
	{
		m_vertexBuffer[streamIndex].current = 0;
		return;
	}
	d3dVertexBuffer_gl* vbgl = (d3dVertexBuffer_gl*)vertexBuffer;
	m_vertexBuffer[streamIndex].current = vbgl->m_vbo;
}

void d3dDevice_gl::CreateIndexBuffer( d3dIndexBuffer*& OUT indexBuffer )
{
	d3dIndexBuffer_gl* ib = sx_new( d3dIndexBuffer_gl );
	m_indexBufferArray.PushBack( ib );
	ib->m_device = this;
	indexBuffer = ib;
}

void d3dDevice_gl::DestroyIndexBuffer( d3dIndexBuffer*& IN_OUT indexBuffer )
{
	if ( !indexBuffer ) return;
	m_indexBufferArray.Remove( indexBuffer );
	sx_delete_and_null( indexBuffer );
}

void d3dDevice_gl::SetIndexBuffer( const d3dIndexBuffer* indexBuffer )
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
	m_textureArray.PushBack( tx );
	tx->m_device = this;
	texture = tx;
}

void d3dDevice_gl::DestroyTexture( d3dTexture*& IN_OUT texture )
{
	if ( !texture ) return;
	m_textureArray.Remove( texture );
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

void d3dDevice_gl::CreateShader( d3dShader*& OUT shader )
{

}

void d3dDevice_gl::DestroyShader( d3dShader*& IN_OUT shader )
{

}

SEGAN_INLINE void d3dDevice_gl::SetViewport( const d3dViewport* viewport )
{
	m_viewport = *viewport;
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

bool d3dDevice_gl::BeginScene( void )
{
#if defined(_WIN32)
	InvalidateRect( m_initParam.hwnd, NULL, FALSE );
#endif

	Matrix mat;
	mat.PerspectiveFov( PI / 3.0f, (float)m_viewport.height / (float)m_viewport.width, 0.5f, 1000.0f );
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( &mat._11 );

	static float timer = 0;
	timer =  (float)( 0.0001f * sx_os_get_timer() );
	float eye[3] = { 5.0f * sx_sin(timer), 0.0f, 30.0f * sx_cos(timer)	};
	float at[3] = { 0.0f, 0.0f, 0.0f };
	float up[3] = { 0.0f, 1.0f, 0.0f };
	mat.LookAt( eye, at, up );
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( &mat._11 );

	glViewport( m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height );

	return true;
}

void d3dDevice_gl::EndScene( void )
{
//  	glTranslatef( -1.5f,0.0f,0.0f );
//  	glBegin( GL_TRIANGLES );
//  	glColor3f( 1.0f,0.0f,0.0f );
//  	glVertex3f( 0.0f, 1.0f, 0.10f );
//  	glColor3f( 0.0f,1.0f,0.0f );
//  	glVertex3f( -1.0f,-1.0f, 0.10f );
//  	glColor3f( 0.0f,0.0f,1.0f );
//  	glVertex3f( 1.0f,-1.0f, 0.10f );
//  	glEnd();
//  

// 	{
// 		BufferStates* tx = &( m_textureBuffer[0] );
//		glEnable( tx->target );
// 		glBindTexture( tx->target, tx->current );
// 	}
	
	ApplyTextureBuffer();
  	glTranslatef( 3.0f,0.0f,0.0f );
  	glColor3f( 1.0f,1.0f,1.0f );
  	glBegin( GL_QUADS );
  	glTexCoord2f( 0, 0 );	glVertex3f( -1.0f, 1.0f, 0.0f );
  	glTexCoord2f( 1, 0 );	glVertex3f(  1.0f, 1.0f, 0.0f );
  	glTexCoord2f( 1, 1 );	glVertex3f(  1.0f,-1.0f, 0.0f );
  	glTexCoord2f( 0, 1 );	glVertex3f( -1.0f,-1.0f, 0.0f );
  	glEnd();
}

void d3dDevice_gl::Present( void )
{
	static int    s_frameCount	= 0;
	static double s_countTime	= 0;
	static double s_frameTime	= sx_os_get_timer();

	//	swap buffers to display
	if ( WGL_EXT_swap_control_supported )
		wglSwapIntervalEXT( (m_creationData.flag & SX_D3D_VSYNC) ? 1 : 0 );
	SwapBuffers( m_initParam.hdc );
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

void d3dDevice_gl::ClearScreen( const dword bgcolor )
{
	GLbitfield clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	float r = SEGAN_2TH_BYTEOF(bgcolor) / 255.0f;
	float g = SEGAN_3TH_BYTEOF(bgcolor) / 255.0f;
	float b = SEGAN_4TH_BYTEOF(bgcolor) / 255.0f;
	float a = SEGAN_1TH_BYTEOF(bgcolor) / 255.0f;
	glClearColor( r, g, b, a );

	if ( m_driverDisplayMode.depthFormat == FMT_D24S8 )
		clearBits |= GL_STENCIL_BUFFER_BIT;

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_TRUE );

	glClear( clearBits );
}

void d3dDevice_gl::ClearTarget( const dword bgcolor )
{
	float r = SEGAN_2TH_BYTEOF(bgcolor) / 255.0f;
	float g = SEGAN_3TH_BYTEOF(bgcolor) / 255.0f;
	float b = SEGAN_4TH_BYTEOF(bgcolor) / 255.0f;
	float a = SEGAN_1TH_BYTEOF(bgcolor) / 255.0f;
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
		case 0:		//	position
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

		case 1:		//	normal
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

		case 2:		//	texture 0
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

		case 3:		//	tangent
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 0 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
			}
			else
			{
				if ( vb->lastOne )
					glDisableVertexAttribArray( 0 );
			}
			break;

		case 4:		//	texture 1
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 1 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 4, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
			}
			else
			{
				if ( vb->lastOne )
					glDisableVertexAttribArray( 1 );
			}
			break;

		case 5:		//	color
			if ( vb->current )
			{
				if ( !vb->lastOne )
				{
					glEnableVertexAttribArray( 2 );
					glEnableClientState( GL_COLOR_ARRAY );
				}

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 5, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, BUFFER_OFFSET(0) );			//	color 0
				glVertexAttribPointer( 5, 4, GL_UNSIGNED_BYTE, GL_FALSE, 8, BUFFER_OFFSET(4) );			//	color 1

				//	enable fixed function pipeline
				glColorPointer( 4, GL_UNSIGNED_BYTE, 8, BUFFER_OFFSET(0) );
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

		case 6:		//	blend indices
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 3 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 6, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, BUFFER_OFFSET(0) );
			}
			else
			{
				if ( vb->lastOne )
					glDisableVertexAttribArray( 3 );
			}
			break;

		case 7:		//	blend weights
			if ( vb->current )
			{
				if ( !vb->lastOne )
					glEnableVertexAttribArray( 4 );

				sx_glBindBuffer( GL_ARRAY_BUFFER, vb->current );
				glVertexAttribPointer( 7, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
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
SEGAN_INLINE SEGAN_API void sx_glBindBuffer( GLenum target, GLuint buffer )
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

SEGAN_INLINE SEGAN_API void sx_glBindTexture( GLenum target, GLuint texture )
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