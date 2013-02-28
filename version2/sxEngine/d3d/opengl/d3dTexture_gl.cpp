#include "d3dTexture_gl.h"

const GLenum glInternalFormat[] =
{
	0,

	GL_INTENSITY8,
	GL_LUMINANCE8_ALPHA8,
	GL_RGB8,
	GL_RGBA8,

	GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT24,
	GL_DEPTH24_STENCIL8_EXT,
	GL_DEPTH_COMPONENT32
};

const GLenum glChannelFormat[] =
{
	0,

	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_RGB,
	GL_RGBA,

	GL_RGB,
	GL_RGBA,

	GL_LUMINANCE,
	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_LUMINANCE
};

const GLenum glChannelTypes[] =
{
	0,

	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,

	0,
	0,

	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT,
	GL_UNSIGNED_INT_24_8_EXT,
	GL_UNSIGNED_INT
};

const int glBytesPerBlock[] =
{
	0,

	1,
	2,
	3,
	4,

	8,
	16,

	2,
	4,
	4,
	4
};

const GLenum glAddressMode[] =
{
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP,
	GL_CLAMP_TO_BORDER
};

const GLenum glFilter[] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR,
	GL_LINEAR,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR
};

//////////////////////////////////////////////////////////////////////////
//	HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////
SEGAN_INLINE bool IsCompressedFormat( d3dFormat format )
{
	switch ( format )
	{
	case FMT_DXT1:
	case FMT_DXT5:
		return true;
	}
	return false;
}

SEGAN_INLINE uint GetSizeInMipMapLevel( const uint size, const uint level )
{
	return size >> level;
}


//////////////////////////////////////////////////////////////////////////
//	TEXTURE GL
//////////////////////////////////////////////////////////////////////////
d3dTexture_gl::d3dTexture_gl( void )
:	d3dTexture()
,	m_device(0)
,	m_tbo(0)
,	m_target(0)
{

}

d3dTexture_gl::~d3dTexture_gl( void )
{
	if ( m_tbo )
		glDeleteTextures( 1, &m_tbo );
}

void d3dTexture_gl::SetDesc( d3dTextureDesc& desc )
{
	if ( m_tbo && ( !desc.width || !desc.height ) )
	{
		glDeleteTextures( 1, &m_tbo );
		m_tbo = 0;
		m_desc = desc;
		return;
	}

	if ( !m_tbo )
		glGenTextures( 1, &m_tbo );
	m_desc = desc;

	//  recompute mipmaps depend on texture size
	if ( m_desc.mipmaps )
	{
		m_desc.mipmaps = 0;
		sint size = sx_min_i( m_desc.width, m_desc.height );
		while ( size > 1 )
		{
			size /= 2;
			m_desc.mipmaps++;
		}
	}

	m_target = m_desc.type == TT_2D ? GL_TEXTURE_2D : ( m_desc.type == TT_CUBE ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_3D );
	sx_glBindTexture( m_target, m_tbo );

	// Set requested wrapping modes
	glTexParameteri( m_target, GL_TEXTURE_WRAP_S, glAddressMode[m_desc.addressMode] );
	glTexParameteri( m_target, GL_TEXTURE_WRAP_T, glAddressMode[m_desc.addressMode] );
	if ( m_target == GL_TEXTURE_3D )
		glTexParameteri( m_target, GL_TEXTURE_WRAP_R, glAddressMode[m_desc.addressMode] );

	// Set requested filter modes
	GLenum magFilter = ( m_desc.filter == FILTER_POINT ) ? GL_NEAREST : GL_LINEAR;
	GLenum minfilter = m_desc.mipmaps ? glFilter[ m_desc.filter ] : magFilter;
	glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, magFilter );
	glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, minfilter );

	// Setup anisotropic filtering
	if ( m_desc.mipmaps && m_device->m_driverCaps.maxAnisotropy > 1 && GL_EXT_texture_filter_anisotropic_supported )
	{
		if (m_desc.filter == FILTER_LINEAR_ANISOTROPIC		|| 
			m_desc.filter == FILTER_BILINEAR_ANISOTROPIC	|| 
			m_desc.filter == FILTER_TRILINEAR_ANISOTROPIC	)
			glTexParameteri( m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_desc.anisotropy );
	}

	sx_glBindTexture( m_target, 0 );
}

void d3dTexture_gl::SetImage( void* data, uint level, uint face /*= 0 */ )
{
	if ( !m_desc.width || !m_desc.height )
		return;

	if ( !m_tbo )
		glGenTextures( 1, &m_tbo );

	//	collect information
	GLenum internalFormat	= glInternalFormat[ m_desc.format ];
	GLenum channelFormat	= glChannelFormat[ m_desc.format ];
	GLenum channelType		= glChannelTypes[ m_desc.format ];
	GLuint width			= GetSizeInMipMapLevel( m_desc.width, level );
	GLuint height			= GetSizeInMipMapLevel( m_desc.height, level );
	m_target				= m_desc.type == TT_2D ? GL_TEXTURE_2D : ( m_desc.type == TT_CUBE ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_3D );

	//////////////////////////////////////////////////////////////////////////
	//	set data to the texture image
	//////////////////////////////////////////////////////////////////////////
	sx_glBindTexture( m_target, m_tbo );

	switch ( m_target )
	{
	case GL_TEXTURE_2D:
		{
			if ( IsCompressedFormat( m_desc.format ) )
				glCompressedTexImage2D( m_target, level, internalFormat, width, height, 0, GetDataSize( level ), data );
			else
				glTexImage2D( m_target, level, internalFormat, width, height, 0, channelFormat, channelType, data );
		}
		break;
	}

	sx_glBindTexture( m_target, 0 );
}

void d3dTexture_gl::GetImage( void* data, uint level, uint face /*= 0 */ )
{
	if ( !m_tbo || !m_desc.width || !m_desc.height )
		return;

	//	collect information
	GLenum channelFormat	= glChannelFormat[ m_desc.format ];
	GLenum channelType		= glChannelTypes[ m_desc.format ];
	
	//////////////////////////////////////////////////////////////////////////
	//	get data from the texture image
	//////////////////////////////////////////////////////////////////////////
	sx_glBindTexture( m_target, m_tbo );

	switch ( m_target )
	{
	case GL_TEXTURE_2D:
		{
			if ( IsCompressedFormat( m_desc.format ) )
				glGetCompressedTexImage( m_target, level, data );
			else
				glGetTexImage( m_target, level, channelFormat, channelType, data );
		}
		break;
	}

	sx_glBindTexture( m_target, 0 );
}

SEGAN_INLINE uint d3dTexture_gl::GetDataSize( uint level )
{
	bool c	= IsCompressedFormat( m_desc.format );
	uint w	= GetSizeInMipMapLevel( m_desc.width,	level );
	uint h	= GetSizeInMipMapLevel( m_desc.height,	level );
	uint s	= c ? ( (w + 3) >> 2) * ((h + 3) >> 2 ) : ( w * h ) ;
	return s * glBytesPerBlock[ m_desc.format ];
}

SEGAN_INLINE void d3dTexture_gl::SetToDevice( uint stage )
{
	sx_assert( m_device );
	m_device->SetTexture( this, stage );
}