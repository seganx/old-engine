#include "sxShaderPool.h"
#include "sxShader3D.h"
#include "sxDevice3D.h"

//////////////////////////////////////////////////////////////////////////
//  use this class to apply shared variables
class ShaderPool_internal
{
public:
	// in necessary times this function will call to recreate shader pool
	static void CreateShaderPool(void)
	{
		sx_callstack_push(ShaderPool_internal::CreateShaderPool());

		sx_delete_and_null(pShaderPool);

		pShaderPool = sx_new( sx::d3d::Shader3D );
		String str =	\
			L"shared matrix matAW[50];"	\
			L"shared matrix matV;"		\
			L"shared matrix matVI;"		\
			L"shared matrix matVP;"		\
			L"shared matrix matP;"		\
			L"shared matrix matLight;"	\
			L"shared matrix sunLight;"	\
			L"shared float fTime;"		\
			L"technique ShaderPool{}"	;

		pShaderPool->CompileShader(str, SQ_HIGH, L"Shader pool : ");

		m_hMatWorld					= pShaderPool->GetParameter(0);
		m_hMatView					= pShaderPool->GetParameter(1);
		m_hMatViewInv				= pShaderPool->GetParameter(2);
		m_hMatViewProjection		= pShaderPool->GetParameter(3);
		m_hMatProjection			= pShaderPool->GetParameter(4);
		m_hMatLight					= pShaderPool->GetParameter(5);
		m_hSunLight					= pShaderPool->GetParameter(6);
		m_hTime						= pShaderPool->GetParameter(7);
	}

	// delete shader pool
	static void DeleteShaderPool(void)
	{
		sx_callstack_push(ShaderPool_internal::DeleteShaderPool());
		sx_delete_and_null(pShaderPool);
	}

	// return true if shader pool is exist
	static bool Exist(void)
	{
		return (pShaderPool != NULL);
	}

	// update shared variables
	static void Update(float elpsTime)
	{
		sx_callstack_push(ShaderPool_internal::Update());

		if (!pShaderPool) return;

		Matrix mat, matV, matP;

		sx::d3d::Device3D::Matrix_View_Get(matV);
		pShaderPool->SetValue(m_hMatView, &matV, sizeof(Matrix));
		mat.Inverse(matV);
		pShaderPool->SetValue(m_hMatViewInv, &mat, sizeof(Matrix));

		sx::d3d::Device3D::Matrix_Project_Get(matP);
		pShaderPool->SetValue(m_hMatProjection, &matP, sizeof(Matrix));

		mat.Multiply(matV, matP);
		pShaderPool->SetValue(m_hMatViewProjection, &mat, sizeof(Matrix));

		static float ftime = 0;
		ftime += elpsTime * 0.001f;
		pShaderPool->SetValue(m_hTime, &ftime, sizeof(float));

		pShaderPool->SetToDevice();
	}

	static	sx::d3d::PShader3D	pShaderPool;
	static	D3DShaderHandle		m_hMatWorld;
	static	D3DShaderHandle		m_hMatView;
	static	D3DShaderHandle		m_hMatViewInv;
	static	D3DShaderHandle		m_hMatViewProjection;
	static	D3DShaderHandle		m_hMatProjection;
	static	D3DShaderHandle		m_hMatLight;
	static	D3DShaderHandle		m_hSunLight;
	static	D3DShaderHandle		m_hTime;
};
sx::d3d::PShader3D	ShaderPool_internal::pShaderPool			= NULL;
D3DShaderHandle		ShaderPool_internal::m_hMatWorld			= NULL;
D3DShaderHandle		ShaderPool_internal::m_hMatView				= NULL;
D3DShaderHandle		ShaderPool_internal::m_hMatViewInv			= NULL;
D3DShaderHandle		ShaderPool_internal::m_hMatViewProjection	= NULL;
D3DShaderHandle		ShaderPool_internal::m_hMatProjection		= NULL;
D3DShaderHandle		ShaderPool_internal::m_hMatLight			= NULL;
D3DShaderHandle		ShaderPool_internal::m_hSunLight			= NULL;
D3DShaderHandle		ShaderPool_internal::m_hTime				= NULL;

namespace sx { namespace d3d {

	void ShaderPool::Initialize( void )
	{
		sx_callstack_push(ShaderPool::Initialize());

		ShaderPool_internal::DeleteShaderPool();
		ShaderPool_internal::CreateShaderPool();
	}

	void ShaderPool::Finalize( void )
	{
		ShaderPool_internal::DeleteShaderPool();
	}

	void ShaderPool::Update( float elpsTime )
	{
		if ( !ShaderPool_internal::Exist() )
			ShaderPool_internal::CreateShaderPool();

		ShaderPool_internal::Update(elpsTime);
	}

	void ShaderPool::SetWorldMatrices( PMatrix arrayMatrix, UINT count )
	{
		if ( !ShaderPool_internal::Exist() )
			ShaderPool_internal::CreateShaderPool();

		ShaderPool_internal::pShaderPool->SetMatrixArray(ShaderPool_internal::m_hMatWorld, arrayMatrix, count );
		ShaderPool_internal::pShaderPool->SetToDevice();
	}

	void ShaderPool::SetLight( const PMatrix sunLight /*= 0*/, const PMatrix matLight /*= 0*/ )
	{
		if ( !ShaderPool_internal::Exist() )
			ShaderPool_internal::CreateShaderPool();

		if (sunLight)
			ShaderPool_internal::pShaderPool->SetValue( ShaderPool_internal::m_hSunLight, sunLight, sizeof(Matrix) );

		if (matLight)
			ShaderPool_internal::pShaderPool->SetValue( ShaderPool_internal::m_hMatLight, matLight, sizeof(Matrix) );

		if (sunLight || matLight)
			ShaderPool_internal::pShaderPool->SetToDevice();
	}

}} // namespace sx { namespace core {