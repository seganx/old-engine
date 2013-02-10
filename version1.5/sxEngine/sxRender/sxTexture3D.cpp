#include "sxTexture3D.h"
#include "sxDevice3D.h"
#include "../sxSystem/sxLog.h"

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES/FUNCTIONS
//////////////////////////////////////////////////////////////////////////
static UINT		s_LOD = 0;

UINT GetMaxWidthWithLOD(void)
{
	UINT level = sx::d3d::Device3D::Texture_GetMaxLevels() - 1 - s_LOD;
	return (UINT)pow( 2.0, (int)level);	
}

UINT GetFormatSize_TX(D3DFormat& format)
{
	switch (format)
	{
	case D3DFMT_R8G8B8:        return  3;
	case D3DFMT_A8R8G8B8:      return  4;
	case D3DFMT_X8R8G8B8:      return  4;
	case D3DFMT_R5G6B5:        return  2;
	case D3DFMT_X1R5G5B5:      return  2;
	case D3DFMT_A1R5G5B5:      return  2;
	case D3DFMT_A4R4G4B4:      return  2;
	case D3DFMT_R3G3B2:        return  1;
	case D3DFMT_A8:            return  1;
	case D3DFMT_A8R3G3B2:      return  2;
	case D3DFMT_X4R4G4B4:      return  2;
	case D3DFMT_A2B10G10R10:   return  4;
	case D3DFMT_A8B8G8R8:      return  4;
	case D3DFMT_X8B8G8R8:      return  4;
	case D3DFMT_G16R16:        return  4;
	case D3DFMT_A2R10G10B10:   return  4;
	case D3DFMT_A16B16G16R16:  return  8;
		//case D3DFMT_DXT1:          return  ?;
		//case D3DFMT_DXT2:          return  ?;
	case D3DFMT_DXT3:          return  1;
	case D3DFMT_DXT4:          return  1;
	case D3DFMT_DXT5:          return  1;
	case D3DFMT_R16F:          return  2;
	case D3DFMT_G16R16F:       return  4;
	case D3DFMT_A16B16G16R16F: return  8;
	case D3DFMT_R32F:          return  4;
	case D3DFMT_G32R32F:       return  8;
	case D3DFMT_A32B32G32R32F: return  16;
	}
	return 0;
}

UINT GetNearestPowerOf2(UINT i)
{
	if ( i<80 )
	{
		UINT r = 1;
		while (1)
		{
			if ( i <= r  ) 
				return r;

			r *= 2;
		}
	}
	else
	{
		UINT r = 1;
		while (1)
		{
			if ( i <= ((r + r*2) / 2) ) 
				return r;

			r *= 2;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
namespace sx { namespace d3d
{
	Texture3D::Texture3D( void ) : m_TX(NULL), m_LOD(0) {}

	Texture3D::~Texture3D( void )
	{
		Cleanup();
	}

	PDirect3DBaseTexture Texture3D::GetD3DTexture( void )
	{
		return m_TX;
	}

	bool Texture3D::CreateTextureEx( D3DResourceType type, UINT width, UINT height, UINT levels, DWORD usage, D3DFormat format, D3DPool pool )
	{
		Cleanup();

		switch (type)
		{
		case D3DRTYPE_TEXTURE:
			return Resource3D::CreateTexture2D(width, height, levels, usage, format, pool, (PDirect3DTexture&)m_TX, SX_RESOURCE_CHECKLOST );
		case D3DRTYPE_CUBETEXTURE:
			return Resource3D::CreateTextureCube(width, levels, usage, format, pool, (PDirect3DTextureCube&)m_TX, SX_RESOURCE_CHECKLOST );
		}
		return false;
	}

	bool Texture3D::CreateRenderTarget( D3DResourceType type, UINT width, UINT height, D3DFormat format )
	{
		Cleanup();

		switch (type)
		{
		case D3DRTYPE_TEXTURE:
			return Resource3D::CreateTexture2D(width, height, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, (PDirect3DTexture&)m_TX, SX_RESOURCE_CHECKLOST );
		case D3DRTYPE_CUBETEXTURE:
			return Resource3D::CreateTextureCube(width, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, (PDirect3DTextureCube&)m_TX, SX_RESOURCE_CHECKLOST );
		}
		return false;
	}

	bool Texture3D::CreateTexture( D3DResourceType type, UINT width, D3DFormat format /*= D3DFMT_DXT5*/, bool limitSize /*= true*/ )
	{
		sx_callstack_push(Texture3D::CreateTexture());

		Cleanup();

		//  limit texture size depend on device capabilities and hardware mode
		if ( limitSize && format != D3DFMT_A8R8G8B8 && width > GetMaxWidthWithLOD() )
			width = GetMaxWidthWithLOD();

		bool res = false;
		switch (type)
		{
		case D3DRTYPE_TEXTURE:		
			if		(format == D3DFMT_DXT5) res = Resource3D::GetTexture2D(	width, (PDirect3DTexture&)m_TX );
			else	res = Resource3D::CreateTexture2D( width, width, 0, 0, format, D3DPOOL_MANAGED, (PDirect3DTexture&)m_TX );
			break;
		case D3DRTYPE_CUBETEXTURE:	
			if		(format == D3DFMT_DXT5) res = Resource3D::GetTextureCube( width, (PDirect3DTextureCube&)m_TX );	
			else	res = Resource3D::CreateTextureCube( width, 0, 0, format, D3DPOOL_MANAGED, (PDirect3DTextureCube&)m_TX );
			break;
		}

		if (res) m_TX->SetLOD(m_LOD);
		return res;
	}

	FORCEINLINE void Texture3D::SetAsRenderTarget( int index, int face /*= 0*/ )
	{
		if ( m_TX )
		{
			switch ( m_TX->GetType() )
			{
			case D3DRTYPE_TEXTURE:
				{
					PDirect3DTexture tx = (PDirect3DTexture)m_TX;
					PDirect3DSurface pSurf = NULL;
					tx->GetSurfaceLevel(0, &pSurf);
					sx::d3d::Device3D::SetRenderTarget(0, pSurf);
					SEGAN_RELEASE_AND_NULL(pSurf);		
				}
				break;

			case D3DRTYPE_CUBETEXTURE:
				{
					PDirect3DTextureCube tx = (PDirect3DTextureCube)m_TX;
					PDirect3DSurface pSurf = NULL;
					tx->GetCubeMapSurface(D3DCUBEMAP_FACES(face), 0, &pSurf);
					sx::d3d::Device3D::SetRenderTarget(0, pSurf);
					SEGAN_RELEASE_AND_NULL(pSurf);	
				}
				break;
			}
		}
		else sx::d3d::Device3D::SetRenderTarget(0, NULL);
	}

	FORCEINLINE void Texture3D::SetAsDepthStencil( void )
	{
		if ( m_TX  && m_TX->GetType() == D3DRTYPE_TEXTURE )
		{
			PDirect3DTexture tx = (PDirect3DTexture)m_TX;
			PDirect3DSurface pSurf = NULL;
			tx->GetSurfaceLevel(0, &pSurf);
			sx::d3d::Device3D::SetDepthStencil(pSurf);
			SEGAN_RELEASE_AND_NULL(pSurf);
		}
		else sx::d3d::Device3D::SetDepthStencil(NULL);
	}

	bool Texture3D::LoadFromImageFile( const WCHAR* FileName, D3DFormat format /*= D3DFMT_DXT5*/, bool limitSize /*= true*/ )
	{
		if (!FileName) return false;

		D3DXIMAGE_INFO info;
		ZeroMemory(&info, sizeof(D3DXIMAGE_INFO));
		D3DXGetImageInfoFromFile(FileName, &info);
		UINT Width = GetNearestPowerOf2( sx_max_i(info.Width, info.Height) );

		//  limit texture size depend on device capabilities and hardware mode
		if ( limitSize && format == D3DFMT_DXT5 && Width > GetMaxWidthWithLOD() )
			Width = GetMaxWidthWithLOD();

		bool res = false;
		switch (info.ResourceType)
		{
		case D3DRTYPE_TEXTURE: 
			res = SUCCEEDED( 
				D3DXCreateTextureFromFileExW(
				sx::d3d::Device3D::GetDevice(),
				FileName,
				Width, Width, 0, 0,
				format,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_FILTER_NONE,
				0, NULL, NULL,
				(PDirect3DTexture*)(&m_TX)
				) );
			break;

		case D3DRTYPE_CUBETEXTURE:
			res = SUCCEEDED( 
				D3DXCreateCubeTextureFromFileExW(
				sx::d3d::Device3D::GetDevice(),
				FileName,
				Width, 0, 0,
				format,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_FILTER_TRIANGLE,
				0, NULL, NULL,
				(PDirect3DTextureCube*)(&m_TX)
				) );
			break;
		}

		if (m_TX) m_TX->SetLOD(m_LOD);
		return res;
	}

	const WCHAR* Texture3D::SaveToImageFile( const WCHAR* FileName, const WCHAR* format )
	{
		if ( !m_TX ) return false;
		D3DXIMAGE_FILEFORMAT fileFormat;
		String fileExtn = format;
		static String fileName;
		fileName = FileName;

		if ( format )
		{
			if ( fileExtn == L"bmp" )
				fileFormat = D3DXIFF_BMP;
			else if ( fileExtn == L"tga" )
				fileFormat = D3DXIFF_TGA;
			else if ( fileExtn == L"jpg" )
				fileFormat = D3DXIFF_JPG;
			else if ( fileExtn == L"png" )
				fileFormat = D3DXIFF_PNG;
			else
			{
				fileFormat = D3DXIFF_DDS;
				fileExtn = L"dds";
			}
		}
		else
		{
			TextureDesc desc;
			GetDesc(desc);

			switch ( desc.Format )
			{
			case D3DFMT_A8R8G8B8:	fileFormat = D3DXIFF_TGA;	fileExtn = L"tga";	break;
			case D3DFMT_DXT1:		fileFormat = D3DXIFF_JPG;	fileExtn = L"jpg";	break;
			case D3DFMT_DXT5:		fileFormat = D3DXIFF_DDS;	fileExtn = L"dds";	break;
			}
		}

		str512 fileext = FileName; fileext.ExtractFileExtension();
		if ( fileext != fileExtn )
			fileName << L"." << fileExtn;

		if ( SUCCEEDED( D3DXSaveTextureToFile( fileName.Text(), fileFormat, m_TX, NULL ) ) )
			return fileName;
		else
			return NULL;
	}

	bool Texture3D::GetDesc( TextureDesc& desc )
	{
		desc.rType  = (D3DResourceType)0;
		desc.Format = D3DFMT_UNKNOWN;
		desc.Width	= 0;
		desc.Levels = 0;

		if (!m_TX) return false;

		desc.rType	= m_TX->GetType();
		desc.Levels = m_TX->GetLevelCount();
		
		D3DSURFACE_DESC sd;
		switch (desc.rType)
		{
		case D3DRTYPE_TEXTURE:		PDirect3DTexture(m_TX)->GetLevelDesc(0, &sd);		break;
		case D3DRTYPE_CUBETEXTURE:	PDirect3DTextureCube(m_TX)->GetLevelDesc(0, &sd);	break;
		}

		desc.Format = sd.Format;
		desc.Width	= sd.Width;

		return true;
	}

	bool Texture3D::Lock( UINT level, PD3DLockedRect pRectArray )
	{
		if (!m_TX) return false;

		sx_assert(pRectArray);

		bool res = false;
		D3DResourceType rType = m_TX->GetType();
		switch (rType)
		{
		case D3DRTYPE_TEXTURE:	
			res = SUCCEEDED( PDirect3DTexture(m_TX)->LockRect(level, pRectArray, NULL, 0) );
			break;

		case D3DRTYPE_CUBETEXTURE:
			for (int i=0; i<6; i++)
			{
				res = SUCCEEDED( PDirect3DTextureCube(m_TX)->LockRect(D3DCUBEMAP_FACES(i), level, &pRectArray[i], NULL, 0) );
				if (!res) 
				{
					for (i--; i>=0; i--)
						PDirect3DTextureCube(m_TX)->UnlockRect(D3DCUBEMAP_FACES(i), level);

					break;
				}
			}
			break;
		}

		return res;
	}

	void Texture3D::Unlock( UINT level )
	{
		if (!m_TX) return;

		D3DResourceType rType = m_TX->GetType();
		switch (rType)
		{
		case D3DRTYPE_TEXTURE:	
			PDirect3DTexture(m_TX)->UnlockRect(level);
			m_TX->SetLOD(m_LOD);
			return;

		case D3DRTYPE_CUBETEXTURE:
			for (int i=0; i<6; i++)
				PDirect3DTextureCube(m_TX)->UnlockRect(D3DCUBEMAP_FACES(i), level);
			m_TX->SetLOD(m_LOD);
			return;
		}
	}

	FORCEINLINE void Texture3D::SetLOD( int LOD )
	{
		if (LOD != m_LOD)
		{
			m_LOD = LOD;
			if(m_TX) 
				m_TX->SetLOD(LOD);
		}
	}

	int Texture3D::GetLOD( void )
	{
		return m_LOD;
	}

	FORCEINLINE void Texture3D::SetToDevice( UINT stage /*= 0*/ )
	{
		Device3D::SetTexture(stage, m_TX);
	}

	void Texture3D::Cleanup( void )
	{
		sx_callstack_push(Texture3D::Cleanup());

		if ( !m_TX ) return;

		m_TX->SetLOD(0);
		TextureDesc desc;
		GetDesc( desc );
		
		for (UINT i=0; i<desc.Levels; i++)
			Unlock(i);

		m_LOD = 0;

		//sxLog::Log(L"start texture 3d releasing");
		Resource3D::ReleaseTexture(m_TX);
		//sxLog::Log(L"end texture 3d releasing");
	}

	//////////////////////////////////////////////////////////////////////////
	//	STATIC FUNCTIONS
	//////////////////////////////////////////////////////////////////////////
	void Texture3D::SetLoadingLOD( UINT LOD )
	{
		s_LOD = LOD;
		SEGAN_CLAMP(s_LOD, 0, Device3D::Texture_GetMaxLevels());
	}

	UINT Texture3D::GetLoadingLOD( void )
	{
		return s_LOD;
	}

	UINT Texture3D::GetMaxLevels( void )
	{
		return sx::d3d::Device3D::Texture_GetMaxLevels();
	}

	UINT Texture3D::GetMaxWidth( void )
	{
		return sx::d3d::Device3D::Texture_GetMaxWidth();
	}

} } // namespace sx { namespace d3d

