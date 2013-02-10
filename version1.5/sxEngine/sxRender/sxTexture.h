/********************************************************************
	created:	2010/09/21
	filename: 	sxTexture.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Texture interface that holds texture's
				data plus connection texture data to I/O stream and also
				contain simple internal manager.
				The Texture abstract class can't be initialized or create 
				directly. Use PTexture type and Texture::Manager to create
				or release usable textures.
*********************************************************************/
#ifndef GUARD_sxTexture_HEADER_FILE
#define GUARD_sxTexture_HEADER_FILE

#include "sxRender_def.h"
#include "sxTexture3D.h"
#include "../sxSystem/sxSystem.h"

namespace sx { namespace d3d
{
	/*
	Texture abstract class can't be initialized or create directly. 
	Use PTexture type and Texture::Manager to create or release usable textures.
	*/
	typedef class Texture *PTexture;

	/*
	Texture abstract class can't be initialized or create directly. 
	Use PTexture type and Texture::Manager to create or release usable textures.
	*/
	class SEGAN_API Texture
	{
	public:
		//! return Direct3D texture object
		virtual PDirect3DBaseTexture GetD3DTexture(void) = 0;

		//! create texture with extended parameters
		virtual bool CreateTextureEx(D3DResourceType type, UINT width, UINT height, UINT levels, DWORD usage, D3DFormat format, D3DPool pool) = 0;

		//! create standard render target texture
		virtual bool CreateRenderTarget(D3DResourceType type, UINT width, UINT height, D3DFormat format) = 0;

		//! create standard texture
		virtual bool CreateTexture(D3DResourceType type, UINT width, D3DFormat format = D3DFMT_DXT5) = 0;

		//! set this texture as render target
		virtual void SetAsRenderTarget(int index, int cubeFace = 0) = 0;

		//! set this texture as render target
		virtual void SetAsDepthStencil(void) = 0;

		//! load texture from image file '*.bmp, *.jpg, *.tga, ...'
		virtual bool LoadFromImageFile(const WCHAR* FileName, D3DFormat format = D3DFMT_DXT5) = 0;

		/*! 
		save texture to image file. use  'bmp', 'jpg', 'tga', ... to choose texture format or use NULL to automatic choosing
		return saved path if operation was successful
		*/
		virtual const WCHAR* SaveToImageFile(const WCHAR* FileName, const WCHAR* format) = 0;

		//! return the description of this texture
		virtual bool GetDesc(TextureDesc& desc) = 0;

		//!  Lock all surfaces in specified level
		virtual bool Lock(UINT level, PD3DLockedRect pRectArray) = 0;

		//!  Unlock all surfaces in specified level
		virtual void Unlock(UINT level) = 0;

		//! set LOD
		virtual void SetLOD(int LOD) = 0;

		//! get the current LOD
		virtual int  GetLOD(void) = 0;

		//! set the current resources to the device
		virtual void SetToDevice(UINT stage = 0) = 0;

		//! return the ID of this resource
		virtual DWORD GetID(void) = 0;

		//! set the source address of the resource. this call will update ID by CRC32 algorithm
		virtual void  SetSource(const WCHAR* srcAddress) = 0;

		//! return the source address
		virtual const WCHAR* GetSource(void) = 0;

		//! set additional options
		virtual void  SetOption(DWORD op) = 0;

		//! get current option
		virtual DWORD GetOption(void) = 0;

		/*!
		Set resource valid and increase activated counter.
		LOD = 0; load top level LOD. if sub levels was empty then load all sub levels
		LOD = n; load specifyed LOD of level n. if sub levels was empty then load all sub levels
		*/
		virtual void Activate(int LOD = 0) = 0;

		//! decrease activated counter and cleanup resources when activated counter became zero
		virtual void Deactivate(void) = 0;

		//! cleanup any external resources
		virtual void Cleanup(void) = 0;

		//! save the current object to the stream
		virtual void Save(Stream& stream, PResourceFileDetails pDetails = NULL) = 0;

		//! load the current object from stream. use limitSize to limit the size of texture to hardware support and LOD
		virtual void Load(Stream& stream, bool limitSize) = 0;

		//! reload the current object from it's source
		virtual void Reload( void ) = 0;

		//! return true if it's activated
		virtual bool Activated( void ) = 0;

		//////////////////////////////////////////////////////////////////////////
		//  INTERNAL MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:

			/*! 
			return true if texture of the 'src' is exist in the manager and throw out founded one.
			NOTE: this function DO NOT increase the internal reference counter of the object. so releasing
			the object after the work finished may cause to destroy the object.
			*/
			static bool Exist(OUT PTexture& pTxur, const WCHAR* src);

			/*!
			search for texture of the src. if not exist in manager, create and return new one.
			NOTE: this function increase the internal reference counter of the object. so release
			the object after the work finished.
			*/
			static bool Get(OUT PTexture& pTxur, const WCHAR* src);

			//! create a new texture independent of the others. with reference zero
			static bool Create(OUT PTexture& pTxur, const WCHAR* src);

			/*! 
			increase the internal reference counter.
			NOTE: increasing reference counter may prevent to release resource and lead to accretion memory leak
			*/
			static void AddRef(PTexture pTxur);

			//! decrease the internal reference counter and destroy the texture if reference counter became zero.
			static void Release(PTexture& pTxur);

			//! clear all textures in the manager
			static void ClearAll(void);

			/*!
			set LOD of texture resources. LOD can be 0=high 1=mid 2=low 3=lower ...
			*/
			static void SetLOD(UINT LOD);

			//! return LOD of textures for loading resources. return value is 0=high 1=mid 2=low 3=lower ...
			static UINT GetLOD(void);

			//! return the maximum texture levels supported by hardware device
			static UINT GetMaxLevels(void);

			//! return the maximum texture width supported by hardware device
			static UINT GetMaxWidth(void);

			//! specify to load textures in other threads
			static bool& LoadInThread(void);

			//! get first iterator of texture container
			static Map<UINT, sx::d3d::PTexture>::Iterator& GetFirst(void);
		};
	};

} } // namespace sx { namespace d3d


//////////////////////////////////////////////////////////////////////////
//	SOME HELPER CLASSES
//////////////////////////////////////////////////////////////////////////
class DerivedTexture;

//! use to load texture data in multi threaded mode
class DerivedTaskTextureLoader : public sx::sys::TaskBase
{
public:
	DerivedTaskTextureLoader();
	void Execute(void);
	int				m_LOD;
	DerivedTexture*	m_texture;
};

//! use to cleanup texture data in multi threaded mode
class DerivedTaskTextureCleanup : public sx::sys::TaskBase
{
public:
	DerivedTaskTextureCleanup();
	void Execute(void);
	DerivedTexture*	m_texture;
};

//! use to destroy texture data in multi threaded mode
class DerivedTaskTextureDestroy : public sx::sys::TaskBase
{
public:
	DerivedTaskTextureDestroy();
	void Execute(void);
	DerivedTexture*	m_texture;
};

#endif	//	GUARD_sxTexture_HEADER_FILE
