/********************************************************************
	created:	2012/10/25
	filename: 	d3dIndexBuffer_gl.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple index buffer object for
				OpenGL API
*********************************************************************/
#ifndef GUARD_d3dIndexBuffer_gl_HEADER_FILE
#define GUARD_d3dIndexBuffer_gl_HEADER_FILE


#include "d3dDevice_gl.h"

class SEGAN_ENG_API	d3dIndexBuffer_gl
{
	SEGAN_STERILE_CLASS(d3dIndexBuffer_gl);

public:

	d3dIndexBuffer_gl( void );
	virtual ~d3dIndexBuffer_gl( void );

	virtual void SetDesc( d3dIndexBufferDesc& desc, void* data = null );

	virtual void* Lock( void );

	virtual void Unlock( void );

	virtual void SetToDevice( void );

public:
	d3dIndexBufferDesc		m_desc;		//  description of index buffer
	GLuint					m_ibo;
	void*					m_data;
	d3dDevice_gl*			m_device;
};

#endif	//	GUARD_d3dIndexBuffer_gl_HEADER_FILE
