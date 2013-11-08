/********************************************************************
created:	2012/05/07
filename: 	Device.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain the class of device 3d used OpenGL
			most of codes from Humus :
			http://www.humus.name
*********************************************************************/
#ifndef GUARD_d3dDevice_gl_HEADER_FILE
#define GUARD_d3dDevice_gl_HEADER_FILE


#include "../Render.h"

#if defined(_WIN32)
#include <Windows.h>
#include <gl/GL.h>
#endif

#include "OpenGLExtensions.h"

//!	flags may used for multi streaming
#define SX_VERTEX_POSITION				0
#define SX_VERTEX_NORMAL				1
#define SX_VERTEX_UV0					2
#define SX_VERTEX_TANGENT				3
#define SX_VERTEX_UV1					4
#define SX_VERTEX_COLORS				5
#define SX_VERTEX_BLENDINDICES			6
#define SX_VERTEX_BLENDWEIGHT			7

//! flags of alpha blending mode
#define SX_ALPHA_OFF					0		//	turn alpha blending off
#define	SX_ALPHA_BLEND					1		//	blend source and destination color
#define	SX_ALPHA_ADD					2		//	add source color to destination color
#define	SX_ALPHA_SUB					3		//	subtract source color from destination color

//! flags of culling mode
#define SX_CULL_OFF						0		//	turn culling mode off
#define SX_CULL_CW						1		//	use clock wise system to cull triangles
#define SX_CULL_CCW						2		//	use counter clock wise facing to cull triangles


//! matrix modes
enum d3dMatrixMode
{
	MM_WORLD = 0,
	MM_VIEW,
	MM_PROJECTION,

	MM_32BITENUM = 0xffffffff
};
#define MM_

//! render state mode
enum d3dRenderState
{
	RS_ALPHA = 1,		//! use SX_ALPHA_ to turn on/off alpha blending mode
	RS_CULL,			//!	use SX_CULL_ to turn on/off culling mode
	RS_FILL,			//! use true or false to fill/wire triangles
	RS_ZENABLE,			//! use true or false to enable/disable z depth check
	RS_ZWRITE,			//! use true or false to enable/disable z write

	RS_32BITENUM = 0xffffffff
};
#define RS_

//! primitives supported by draw-primitive API
enum d3dPrimitiveType
{
	PT_POINT = 0,
	PT_LINE_LIST,
	PT_LINE_STRIP,
	PT_TRIANGLE_LIST,
	PT_TRIANGLE_STRIP,
	PT_TRIANGLE_FAN,
	PT_QUAD_LIST,

	PT_32BITENUM = 0xffffffff
};
#define PT_

//! view port structure
struct d3dViewport
{
	sint		x;
	sint		y;
	sint		width;
	sint		height;
};

//! index buffer structure
class d3dIndexBuffer
{
	SEGAN_STERILE_CLASS(d3dIndexBuffer);

public:

	d3dIndexBuffer( void );
	~d3dIndexBuffer( void );

	void SetDesc( const uint flag, const uint sizeinbytes, void* data = null );

	void* Lock( void );

	void Unlock( void );

public:
	dword					m_flag;		//!	resource flag SX_D3D_RESOURCE_
	uint					m_size;		//!	size of buffer in bytes
	GLuint					m_ibo;
	void*					m_data;
	class d3dDevice*		m_device;
};

class d3dVertexBuffer
{
	SEGAN_STERILE_CLASS(d3dVertexBuffer);

public:

	d3dVertexBuffer( void );
	~d3dVertexBuffer( void );

	void SetDesc( const uint flag, const uint sizeinbytes, void* data = null );

	void* Lock( void );

	void Unlock( void );

public:
	dword					m_flag;		//!	resource flag SX_D3D_RESOURCE_
	uint					m_size;		//!	size of buffer in bytes
	GLuint					m_vbo;
	void*					m_data;
	class d3dDevice*		m_device;
};

class d3dTexture_gl : public d3dTexture
{
	SEGAN_STERILE_CLASS( d3dTexture_gl );

public:

	d3dTexture_gl( void );
	virtual ~d3dTexture_gl( void );

	virtual void set_desc( d3dTextureDesc& desc );

	virtual void set_image( void* data, uint level, uint face = 0 );

	virtual void get_image( void* data, uint level, uint face = 0 );

	virtual uint get_data_size( uint level );

public:

	d3dDevice*		m_device;
	GLuint			m_tbo;
	GLuint			m_target;
};

//////////////////////////////////////////////////////////////////////////
//	OPENGL DEVICE
class d3dDevice
{
	SEGAN_STERILE_CLASS( d3dDevice );

public:

	d3dDevice( void );
	~d3dDevice( void );

#if defined(_WIN32)
	void Initialize( const struct HWND__* displayHandle );
#endif

	void Finalize( void );

	bool SetSize( const uint width, const uint height, const dword SX_D3D_ flag );

	void CreateVertexBuffer( d3dVertexBuffer*& OUT vertexBuffer );

	void DestroyVertexBuffer( d3dVertexBuffer*& IN_OUT vertexBuffer );

	void SetVertexBuffer( const d3dVertexBuffer* vertexBuffer, uint streamIndex );

	void CreateIndexBuffer( d3dIndexBuffer*& OUT indexBuffer );

	void DestroyIndexBuffer( d3dIndexBuffer*& IN_OUT indexBuffer );

	void SetIndexBuffer( const d3dIndexBuffer* indexBuffer );

	void CreateTexture( d3dTexture_gl*& OUT texture );

	void DestroyTexture( d3dTexture_gl*& IN_OUT texture );

	void SetTexture( const d3dTexture_gl* texture, uint stage = 0 );
	
	void SetViewport( const d3dViewport* viewport );

	void SetMatrix( const d3dMatrixMode mode, const matrix& _matrix );

	const matrix& GetMatrix( const d3dMatrixMode mode );
	
	void SetRenderState( const d3dRenderState type, const uint mode );

	uint GetRenderState( const d3dRenderState type );

	void DrawPrimitive( const d3dPrimitiveType primType, const int firstVertex, const int vertexCount );

	void DrawIndexedPrimitive( const d3dPrimitiveType primType, const int firstIndex, const int indicesCount, const int firstVertex, const int vertexCount );

	void DrawDebug( const d3dPrimitiveType primType, const uint vertxcount, const float* vertices, const Color& color );

	bool BeginScene( void );

	void EndScene( void );

	void Present( void );

	void ClearScreen( const Color& bgcolor );

	void ClearTarget( const Color& bgcolor );

	void ClearZBuffer( void );

	void SetClipPlane( const uint index, const float* pplane );

	void GetClipPlane( const uint index, float* pplane );

	void ApplyVertexBuffer( void );

	void ApplyTextureBuffer( void );

public:

	struct PresentParameter
	{
#if	defined(_WIN32)
		HWND	hwnd;
		HGLRC	context;
		HDC		hdc;
#elif defined(LINUX)
#elif defined(__APPLE__)
#endif
		int		colorBits;
		int		depthBits;
		int		stencilBits;
		bool	fullscreen;
	};

	struct BufferStates
	{
		int		lastOne;
		int		current;
		uint	lastTarget;
		uint	target;
		float	lastLod;
		float	lod;
	};

	struct CreationData
	{
		dword		flag;
		uint		width;
		uint		height;
	};

	d3dDriverInfo				m_driverInfo;			//  contain driver information
	d3dDisplayeInfo				m_driverDisplayMode;	//	contain default driver display mode
	d3dDebugInfo				m_debugInfo;			//	describe debug information	
	d3dViewport					m_viewport;				//	current viewport

	CreationData				m_creationData;			//	describe parameters of creation data
	PresentParameter			m_initParam;

	BufferStates				m_indexBuffer;
	BufferStates				m_vertexBuffer[8];
	BufferStates				m_textureBuffer[8];

	Array<d3dVertexBuffer*>		m_vertexBufferArray;
	Array<d3dIndexBuffer*>		m_indexBufferArray;
	Array<d3dTexture_gl*>		m_textureArray;

	matrix						m_world;
	matrix						m_view;
	matrix						m_projection;

	uint						m_rs_alpha;
	uint						m_rs_cull;
	uint						m_rs_fill;
	uint						m_rs_zenable;
	uint						m_rs_zwrite;
};


//////////////////////////////////////////////////////////////////////////
//	additional helper functions
//////////////////////////////////////////////////////////////////////////

SEGAN_INLINE SEGAN_ENG_API void sx_glBindBuffer( GLenum target, GLuint buffer );
SEGAN_INLINE SEGAN_ENG_API void sx_glBindTexture( GLenum target, GLuint texture );

#endif	//	GUARD_d3dDevice_gl_HEADER_FILE

