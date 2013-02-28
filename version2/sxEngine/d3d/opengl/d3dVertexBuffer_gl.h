/********************************************************************
	created:	2012/08/22
	filename: 	d3dVertexBuffer_gl.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple vertex buffer object for
				OpenGL API
*********************************************************************/
#ifndef GUARD_d3dVertexBuffer_gl_HEADER_FILE
#define GUARD_d3dVertexBuffer_gl_HEADER_FILE

#include "d3dDevice_gl.h"

class SEGAN_ENG_API	d3dVertexBuffer_gl : public d3dVertexBuffer
{
	SEGAN_STERILE_CLASS(d3dVertexBuffer_gl);

public:

	d3dVertexBuffer_gl( void );
	virtual ~d3dVertexBuffer_gl( void );

	virtual void SetDesc( d3dVertexBufferDesc& desc, void* data = null );

	virtual void* Lock( void );

	virtual void Unlock( void );

	virtual void SetToDevice( uint streamIndex );

public:
	GLuint			m_vbo;
	void*			m_data;
	d3dDevice_gl*	m_device;
};

#endif	//	GUARD_d3dVertexBuffer_gl_HEADER_FILE