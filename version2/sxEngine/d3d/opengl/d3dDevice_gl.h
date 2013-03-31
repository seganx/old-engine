/********************************************************************
created:	2012/05/07
filename: 	d3dDevice_gl.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain the class of device 3d used OpenGL
			most of codes from Humus :
			http://www.humus.name
*********************************************************************/
#ifndef GUARD_d3dDevice_gl_HEADER_FILE
#define GUARD_d3dDevice_gl_HEADER_FILE


#include "../d3dDevice.h"

#if defined(_WIN32)
#include <Windows.h>
#include <gl/GL.h>
#endif

#include "OpenGLExtensions.h"



//////////////////////////////////////////////////////////////////////////
//	OPENGL DEVICE
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API d3dDevice_gl: public d3dDevice
{
	SEGAN_STERILE_CLASS( d3dDevice_gl );

public:

	d3dDevice_gl( void );
	virtual ~d3dDevice_gl( void );

	virtual void Initialize( const handle displayHandle );

	virtual void Finalize( void );

	virtual bool SetSize( const uint width, const uint height, const dword SX_D3D_ flag );

	virtual void CreateVertexBuffer( d3dVertexBuffer*& OUT vertexBuffer );

	virtual void DestroyVertexBuffer( d3dVertexBuffer*& IN_OUT vertexBuffer );

	virtual void SetVertexBuffer( const d3dVertexBuffer* vertexBuffer, uint streamIndex );

	virtual void CreateIndexBuffer( d3dIndexBuffer*& OUT indexBuffer );

	virtual void DestroyIndexBuffer( d3dIndexBuffer*& IN_OUT indexBuffer );

	virtual void SetIndexBuffer( const d3dIndexBuffer* indexBuffer );

	virtual void CreateTexture( d3dTexture*& OUT texture );

	virtual void DestroyTexture( d3dTexture*& IN_OUT texture );

	virtual void SetTexture( const d3dTexture* texture, uint stage = 0 );

	virtual void CreateShader( d3dShader*& OUT shader );

	virtual void DestroyShader( d3dShader*& IN_OUT shader );
	
	virtual void SetViewport( const d3dViewport* viewport );

	virtual void SetMatrix( const d3dMatrixMode mode, const matrix& _matrix );

	virtual const matrix& GetMatrix( const d3dMatrixMode mode );
	
	virtual void SetRenderState( const d3dRenderState type, const uint mode );

	virtual uint GetRenderState( const d3dRenderState type );

	virtual void DrawPrimitive( const d3dPrimitiveType primType, const int firstVertex, const int vertexCount );

	virtual void DrawIndexedPrimitive( const d3dPrimitiveType primType, const int firstIndex, const int indicesCount, const int firstVertex, const int vertexCount );

	virtual void DrawDebug( const d3dPrimitiveType primType, const uint vertxcount, const float* vertices, const Color& color );

	virtual bool BeginScene( void );

	virtual void EndScene( void );

	virtual void Present( void );

	virtual void ClearScreen( const Color& bgcolor );

	virtual void ClearTarget( const Color& bgcolor );

	virtual void ClearZBuffer( void );

	virtual void SetClipPlane( const uint index, const float* pplane );

	virtual void GetClipPlane( const uint index, float* pplane );

private:

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


	PresentParameter	m_initParam;
	
	BufferStates		m_indexBuffer;
	BufferStates		m_vertexBuffer[8];
	BufferStates		m_textureBuffer[8];

	Array<d3dVertexBuffer*>		m_vertexBufferArray;
	Array<d3dIndexBuffer*>		m_indexBufferArray;
	Array<d3dTexture*>			m_textureArray;
	Array<d3dShader*>			m_shaderArray;

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

