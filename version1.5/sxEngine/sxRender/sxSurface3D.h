/********************************************************************
	created:	2011/06/08
	filename: 	sxSurface3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Surface3D class that holds surface's
				data. Surface3D is an advanced management of surface 
				for SeganX Engine.
*********************************************************************/
#ifndef GUARD_sxSurface3D_HEADER_FILE
#define GUARD_sxSurface3D_HEADER_FILE

#include "sxResource3D.h"

namespace sx { namespace d3d
{
	//! a texture class that represent Direct3D texture resource
	class SEGAN_API Surface3D
	{
		SEGAN_STERILE_CLASS(Surface3D);
	
	public:
		Surface3D(void);
		~Surface3D(void);

		//! return Direct3D surface object
		PDirect3DSurface GetD3DSurface(void);

		//! create depth stencil buffer
		bool CreateDepthStencil(const UINT width, const UINT height);

		//! set this surface as depth stencil buffer
		void SetAsDepthStencil(void);

		//!  Lock surface
		bool Lock(PD3DLockedRect pRectArray);

		//!  Unlock surface
		void Unlock(void);

		//! clean up resource and release occupied memory
		void Cleanup(void);

	protected:
		PDirect3DSurface	m_SR;	//  surface interface
	};
	typedef Surface3D *PSurface3D;

} } // namespace sx { namespace d3d

#endif	//	GUARD_sxSurface3D_HEADER_FILE