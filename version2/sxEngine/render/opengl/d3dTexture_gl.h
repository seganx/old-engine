/********************************************************************
	created:	2012/08/26
	filename: 	d3dTexture_gl.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple texture object for openGL API
*********************************************************************/
#ifndef GUARD_d3dTexture_gl_HEADER_FILE
#define GUARD_d3dTexture_gl_HEADER_FILE

#include "d3dDevice_gl.h"

class SEGAN_ENG_API d3dTexture_gl : public d3dTexture
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

	d3dDevice_gl*	m_device;
	GLuint			m_tbo;
	GLuint			m_target;
};

#endif	//	GUARD_d3dTexture_gl_HEADER_FILE