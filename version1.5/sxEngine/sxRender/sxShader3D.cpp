#include "sxShader3D.h"
#include "sxDevice3D.h"
#include "sxResource3D.h"

namespace sx { namespace d3d {


	Shader3D::Shader3D( void ): m_Effect(NULL), m_curTech(NULL)
	{
	}

	Shader3D::~Shader3D( void )
	{
		//Resource3D::ReleaseEffect(m_Effect);
	}

	bool Shader3D::Exist( void )
	{
		return m_Effect != NULL;
	}

	bool Shader3D::CompileShader( const WCHAR* code, SQ_ ShaderQuality quality, const WCHAR* userLog /*= NULL*/ )
	{
		sx_callstack_push(Shader3D::CompileShader(%s, quality, %s), code, userLog);

		if ( !Device3D::GetDevice() || !code ) return false;

		//  create macro for shader compiler
		char* preproc = NULL;
		switch (quality)
		{
		case SQ_HIGH:	preproc = "#define QUALITY_HIGH\r\n";		break;
		case SQ_MEDIUM:	preproc = "#define QUALITY_MEDIUM\r\n";		break;
		case SQ_LOW:	preproc = "#define QUALITY_LOW\r\n";		break;
		}
		int preproclen = (int)strlen(preproc);

		//  prepare source code for compiling
		int srcCodeLen = (int)wcslen(code) + preproclen;
		if (srcCodeLen<30) return false;
		
		char* srcCode = (char*)sx_mem_alloc(srcCodeLen+1);
		ZeroMemory(srcCode, srcCodeLen+1);
		for (int i=0; preproc[i]; srcCode[i]=preproc[i], i++);
		for (int i=0; code[i]; srcCode[i+preproclen]=(int)code[i], i++);
		
		//  create and compile the effect
		Resource3D::ReleaseEffect(m_Effect);
		bool result = Resource3D::CreateEffect(srcCode, m_Effect, userLog);

		//  free allocated buffers
		sx_mem_free(srcCode);

		m_curTech = NULL;
		return result;
	}

	bool Shader3D::GetDesc( D3DShaderDesc& desc )
	{
		ZeroMemory(&desc, sizeof(D3DShaderDesc));
		if (!m_Effect) return false;

		return SUCCEEDED ( m_Effect->GetDesc(&desc) );
	}

	D3DShaderHandle Shader3D::GetRenderTechnique( int index )
	{
		if (!m_Effect) return NULL;

		return m_Effect->GetTechnique(index);
	}

	D3DShaderHandle Shader3D::GetRenderTechnique( const char* name )
	{
		if (!m_Effect) return NULL;
		
		return m_Effect->GetTechniqueByName(name);
	}

	void Shader3D::SetRenderTechnique( D3DShaderHandle rTech )
	{
		if (!m_Effect || m_curTech == rTech) return;

		m_curTech = rTech;

		//  force to change effect
		Device3D::SetEffect(NULL);
		m_Effect->SetTechnique(rTech);
	}

	FORCEINLINE D3DShaderHandle Shader3D::GetParameter( int index )
	{
		if (!m_Effect) return NULL;

		return m_Effect->GetParameter(NULL, index);
	}

	FORCEINLINE D3DShaderHandle Shader3D::GetParameter( const char* name )
	{
		if (!m_Effect) return NULL;

		return m_Effect->GetParameterByName(NULL, name);
	}

	int Shader3D::GetParameter( const char* semantic, PD3DShaderHandle paramHandle /*= NULL*/ )
	{
		if (!m_Effect) return -1;
		
		D3DShaderHandle tmp = NULL;
		tmp = m_Effect->GetParameterBySemantic(NULL, semantic);
		if (tmp)
		{
			if (paramHandle)
				*paramHandle = tmp;
		}
		else return -1;

		D3DShaderDesc desc;
		m_Effect->GetDesc(&desc);
		for (int i=0; i<(int)desc.Parameters; i++)
		{
			if (m_Effect->GetParameter(NULL, i) == tmp)
				return i;
		}

		return -1;
	}

	FORCEINLINE void Shader3D::SetValue( D3DShaderHandle hParam, const void* pData, UINT numBytes )
	{
		sx_callstack_push(Shader3D::SetValue());

		if (!m_Effect || !hParam) return;

		m_Effect->SetValue(hParam, pData, numBytes);
	}

	FORCEINLINE bool Shader3D::GetValue( D3DShaderHandle hParam, void* pData, UINT numBytes )
	{
		if (!m_Effect || !hParam) return false;

		return SUCCEEDED( m_Effect->GetValue(hParam, pData, numBytes) );
	}

	FORCEINLINE void Shader3D::SetMatrixArray( D3DShaderHandle hParam, const PMatrix pMatrix, UINT Count )
	{
		if (!m_Effect || !hParam) return;

		m_Effect->SetMatrixArray(hParam, pMatrix, Count);
	}

	bool Shader3D::GetParameterDesc( int index, OUT D3DShaderParamDesc& desc )
	{
		if (!m_Effect) return false;

		D3DShaderHandle tmp = NULL;
		tmp = m_Effect->GetParameter(NULL, index);
		if (!tmp) return false;
		
		return SUCCEEDED( m_Effect->GetParameterDesc(tmp, &desc) );
	}

	D3DShaderHandle Shader3D::GetAnnotation( int paramIndex, OUT D3DShaderParamDesc& anno, int annoIndex )
	{
		if (!m_Effect) return NULL;

		D3DShaderHandle tmp = NULL;
		tmp = m_Effect->GetParameter(NULL, paramIndex);
		if (!tmp) return NULL;

		D3DShaderHandle annotmp = NULL;
		annotmp = m_Effect->GetAnnotation(tmp, annoIndex);
		if (!annotmp) return NULL;

		if ( SUCCEEDED ( m_Effect->GetParameterDesc(annotmp, &anno) ) )
			return annotmp;
		else
			return NULL;
	}

	FORCEINLINE void Shader3D::SetToDevice( void )
	{
		sx_callstack_push(Shader3D::SetToDevice());

		d3d::Device3D::SetEffect( m_Effect );
	}


}}  //  namespace sx { namespace d3d {
