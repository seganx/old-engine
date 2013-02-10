#include "sxSurface3D.h"
#include "sxDevice3D.h"

//////////////////////////////////////////////////////////////////////////
namespace sx { namespace d3d
{
	Surface3D::Surface3D( void ) : m_SR(NULL) {}

	Surface3D::~Surface3D( void )
	{
		Cleanup();
	}

	FORCEINLINE PDirect3DSurface Surface3D::GetD3DSurface( void )
	{
		return m_SR;
	}

	bool Surface3D::CreateDepthStencil( const UINT width, const UINT height )
	{
		Cleanup();
		return Resource3D::CreateDepthStencil(width, height, m_SR);
	}

	FORCEINLINE void Surface3D::SetAsDepthStencil( void )
	{
		sx::d3d::Device3D::SetDepthStencil(m_SR);
	}

	bool Surface3D::Lock( PD3DLockedRect pRectArray )
	{
		if (!m_SR) return false;

		sx_assert(pRectArray);

		return SUCCEEDED( m_SR->LockRect( pRectArray, NULL, 0) );
	}

	void Surface3D::Unlock( void )
	{
		if (!m_SR) return;

		m_SR->UnlockRect();
	}

	void Surface3D::Cleanup( void )
	{
		Resource3D::ReleaseDepthStencil(m_SR);
	}

} } // namespace sx { namespace d3d

