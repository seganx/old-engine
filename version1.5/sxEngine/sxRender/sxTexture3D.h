/********************************************************************
	created:	2010/09/11
	filename: 	sxTexture3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Texture3D class that holds texture's
				data. Texture3D is an advanced management of texture 
				for SeganX Engine.
*********************************************************************/
#ifndef GUARD_sxTexture3D_HEADER_FILE
#define GUARD_sxTexture3D_HEADER_FILE

#include "sxResource3D.h"

namespace sx { namespace d3d
{
	//! a texture class that represent Direct3D texture resource
	class SEGAN_API Texture3D
	{
		SEGAN_STERILE_CLASS(Texture3D);
	
	public:
		Texture3D(void);
		~Texture3D(void);

		//! return Direct3D texture object
		PDirect3DBaseTexture GetD3DTexture(void);

		//! create texture with extended parameters
		bool CreateTextureEx(D3DResourceType type, UINT width, UINT height, UINT levels, DWORD usage, D3DFormat format, D3DPool pool);

		//! create standard render target texture
		bool CreateRenderTarget(D3DResourceType type, UINT width, UINT height, D3DFormat format);

		//! create standard texture. use limitSize to limit the size of texture to hardware support and LOD
		bool CreateTexture(D3DResourceType type, UINT width, D3DFormat format = D3DFMT_DXT5, bool limitSize = true);

		//! set this texture as render target
		void SetAsRenderTarget(int index, int cubeFace = 0);

		//! set this texture as depth buffer
		void SetAsDepthStencil(void);

		//! load texture from image file '*.bmp, *.jpg, *.tga, ...'.  use limitSize to limit the size of texture to hardware support and LOD
		bool LoadFromImageFile(const WCHAR* FileName, D3DFormat format = D3DFMT_DXT5, bool limitSize = true);

		/*! 
		save texture to image file. use  'bmp', 'jpg', 'tga', ... to choose texture format or use NULL to automatic choosing
		return saved path if operation was successful
		*/
		const WCHAR* SaveToImageFile(const WCHAR* FileName, const WCHAR* format);

		//! return the description of this texture
		bool GetDesc(TextureDesc& desc);

		//!  Lock all surfaces in specified level
		bool Lock(UINT level, PD3DLockedRect pRectArray);

		//!  Unlock all surfaces in specified level
		void Unlock(UINT level);

		//! set level of details
		void SetLOD(int LOD);

		//! return level of details
		int  GetLOD(void);

		//! set this texture to the rendering device
		void SetToDevice(UINT stage = 0);

		//! clean up resource and release occupied memory
		void Cleanup(void);

	protected:
		PDirect3DBaseTexture	m_TX;	//  Texture interface
		UINT					m_LOD;	//	Store Texture LOD

	public:
		/*!
		set LOD of textures for loading resource. LOD can be 0=high 1=mid 2=low 3=lower ...
		NOTE: this settings will apply on creating/loading textures
		*/
		static void SetLoadingLOD(UINT LOD);

		//! return LOD of textures for loading resources. return value is 0=high 1=mid 2=low 3=lower ...
		static UINT GetLoadingLOD(void);

		//! return the maximum texture levels supported by hardware device
		static UINT GetMaxLevels(void);

		//! return the maximum texture width supported by hardware device
		static UINT GetMaxWidth(void);

	};
	typedef Texture3D *PTexture3D;

} } // namespace sx { namespace d3d

#endif	//	GUARD_sxTexture3D_HEADER_FILE
