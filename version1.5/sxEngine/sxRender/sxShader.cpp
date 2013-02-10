#include "sxShader.h"
#include "sxDevice3D.h"
#include "../sxCommon/sxCommon.h"


typedef Map<UINT, sx::d3d::PShader> sxMapShader;
static 	sxMapShader ShaderMap;				//  hold the created shader objects

static ShaderQuality s_shaderQuality = SQ_HIGH;	//  use to control shader quality

// shader parameter hold shader handle and set them to the shader
class DerivedShaderParameter : public sx::d3d::ShaderParameter
{
public:
	DerivedShaderParameter(): m_Handle(0), m_pShader3D(0){}

	FORCEINLINE const PShaderAnnotation GetDesc(void)
	{
		return &m_Desc;
	}
	
	FORCEINLINE void SetFloat(float f)
	{
		m_pShader3D->SetValue(m_Handle, &f, sizeof(float));
	}
	
	FORCEINLINE void SetFloat4(float4& f)
	{
		m_pShader3D->SetValue(m_Handle, &f, sizeof(float4));
	}
	
	FORCEINLINE void SetValue(float* f)
	{
		switch (m_Desc.Type)
		{
		case SPT_FLOAT:		m_pShader3D->SetValue(m_Handle, f, sizeof(float));
		case SPT_FLOAT4:	m_pShader3D->SetValue(m_Handle, f, sizeof(float4));
		}
	}
	
	bool operator == (DerivedShaderParameter& d)
	{
		return 
			m_Desc.Type		==	d.m_Desc.Type	&&
			m_Desc.uiName	==	d.m_Desc.uiName	&&
			m_Desc.uiType	==	d.m_Desc.uiType	&& 
			m_Handle		==	d.m_Handle		&& 
			m_pShader3D		==	d.m_pShader3D	;
	}

	ShaderAnnotation	m_Desc;			//  description of parameter
	D3DShaderHandle		m_Handle;		//  handle of parameter
	sx::d3d::PShader3D	m_pShader3D;	//  pointer to shader
};


// derived shader class is the main shader class 
class DerivedShader : public sx::d3d::Shader
{
	SEGAN_STERILE_CLASS(DerivedShader);

public:
	DerivedShader(void): m_Option(0), m_RefCount(1), 
		m_hMatWorld(0), m_hMatWorldInv(0), m_hMatWorldView(0), m_hMatWorldViewProjection(0),
		m_rtech_main(0), m_rtech_normal(0), m_rtech_zdepth(0), m_rtech_deferred(0), m_rtech_shadow(0)
	{
		m_ID = sx::cmn::ID_Generate(50);
	}

	~DerivedShader(void)
	{
		for ( int i=0; i<m_Param.Count(); i++ )
			sx_delete(m_Param[i]);
		m_Param.Clear();
	}

	DWORD GetID( void )
	{
		return m_ID;
	}

	void SetSource(const WCHAR* srcAddress)
	{
		UINT id = sx::cmn::GetCRC32(srcAddress);
		if (id == m_ID) 
		{
			Compile( s_shaderQuality );
			return;
		}

		sx::d3d::PShader sh = NULL;
		if (ShaderMap.Find(m_ID, sh) && sh==this)
		{
			ShaderMap.Remove(m_ID);
			ShaderMap.Insert(id, this);
		}

		m_Src = srcAddress;
		m_ID = id;

		Compile( s_shaderQuality );
	}

	const WCHAR* GetSource(void)
	{
		return m_Src;
	}

	bool Compile(SQ_ ShaderQuality quality)
	{
		for ( int i=0; i<m_Param.Count(); i++ )
			sx_delete(m_Param[i]);
		m_Param.Clear();

		if (!m_Src.Text()) return false;

		PStream stream;
		if (sx::sys::FileManager::File_Open(m_Src, SEGAN_PACKAGENAME_EFFECT, stream))
		{
			String code;
			sx::cmn::String_Load(code, stream);
			
			String strLog = m_Src; strLog << L" : ";
			if (m_Shader3D.CompileShader(code, quality, strLog))
			{
				m_hMatWorld					= m_Shader3D.GetParameter("matW");
				m_hMatWorldInv				= m_Shader3D.GetParameter("matWI");
				m_hMatWorldView				= m_Shader3D.GetParameter("matWV");
				m_hMatWorldViewProjection	= m_Shader3D.GetParameter("matWVP");
				m_rtech_main				= m_Shader3D.GetRenderTechnique("rtech_main");
				m_rtech_zdepth				= m_Shader3D.GetRenderTechnique("rtech_zdepth");
				m_rtech_normal				= m_Shader3D.GetRenderTechnique("rtech_normal");
				m_rtech_deferred			= m_Shader3D.GetRenderTechnique("rtech_deferred");
				m_rtech_shadow				= m_Shader3D.GetRenderTechnique("rtech_shadow");
				

				D3DShaderDesc desc;
				m_Shader3D.GetDesc(desc);
				for (int i=0; i<(int)desc.Parameters; i++)
				{
					CreateParameter(i);
				}
			}

			sx::sys::FileManager::File_Close(stream);

			return true;
		}

		return false;
	}

	void CreateParameter(int index)
	{
		ShaderAnnotation annot;
		D3DShaderParamDesc desc;
		ZeroMemory(&annot, sizeof(ShaderAnnotation));

		if (m_Shader3D.GetParameterDesc(index, desc))
		{
			//  verify that the parameter has annotation
			if (!desc.Annotations) return;

			switch (desc.Type)
			{
			case D3DXPT_SAMPLER:
			case D3DXPT_SAMPLER1D:
			case D3DXPT_SAMPLER2D:
			case D3DXPT_SAMPLER3D:
			case D3DXPT_SAMPLERCUBE:	annot.Type = SPT_SAMPLER;	break;
			case D3DXPT_FLOAT:
				if (desc.Class == D3DXPC_SCALAR)
					annot.Type = SPT_FLOAT;
				else if (desc.Class == D3DXPC_VECTOR && desc.Columns == 4)
					annot.Type = SPT_FLOAT4;
				break;
			default: return;
			}			
		}
		else return;

		int i=0;
		D3DShaderHandle	hndl;
		while ( (hndl = m_Shader3D.GetAnnotation(index, desc, i)) != NULL )
		{
			if (desc.Name)
			{
				if (desc.Type == D3DXPT_STRING)
				{
					char* value = NULL;
					if ( m_Shader3D.GetValue(hndl, &value, sizeof(char*)) && value )
					{
						int len = 0;
						len = sx_min_i(31, (int)strlen(value));
						if (strcmp(desc.Name, "uiType")==0)
							memcpy(annot.uiType, value, len);
						else if (strcmp(desc.Name, "uiName")==0)
							memcpy(annot.uiName, value, len);
					}
				}
				else if (desc.Type == D3DXPT_FLOAT && desc.Class == D3DXPC_VECTOR)
					m_Shader3D.GetValue(hndl, &annot.uiData, sizeof(float4));
			}
			i++;
		}

		if ( annot.Type != SPT_UNKNOWN && annot.uiType[0] && annot.uiName[0] )
		{
			DerivedShaderParameter* newParam = sx_new( DerivedShaderParameter );
			newParam->m_Desc = annot;
			newParam->m_pShader3D = &m_Shader3D;
			newParam->m_Handle = m_Shader3D.GetParameter(index);
			m_Param.PushBack( newParam );

// 			String str = L"Parameter ";
// 			str << index << L" : " << annot.uiType << L" , " << annot.uiName << L" , value = " 
// 				<< annot.uiData.x<<','<<annot.uiData.y<<','<<annot.uiData.z<<','<<annot.uiData.w;
// 			sxLog::Log(str);
		}
	}

	FORCEINLINE int GetParamCount(void)
	{
		return m_Param.Count();
	}

	FORCEINLINE const sx::d3d::PShaderParameter GetParam(int index)
	{
		if (index<0 || index>=m_Param.Count())
			return NULL;
		else
			return m_Param[index];
	}

	void SetToDevice( DWORD flag )
	{
		Matrix mat, matW, matV, matP;
		sx::d3d::Device3D::Matrix_World_Get(matW);
		sx::d3d::Device3D::Matrix_View_Get(matV);
		sx::d3d::Device3D::Matrix_Project_Get(matP);

		if (m_hMatWorld)
			m_Shader3D.SetValue(m_hMatWorld, &matW, sizeof(Matrix));
		if (m_hMatWorldInv)
		{
			mat.Inverse( matW );
			m_Shader3D.SetValue(m_hMatWorldInv, &mat, sizeof(Matrix));
		}
		if (m_hMatWorldView)
		{			
			mat.Multiply( matW, matV );
			m_Shader3D.SetValue(m_hMatWorldView, &mat, sizeof(Matrix));
		}
		if (m_hMatWorldViewProjection)
		{
			mat.Multiply( matW, matV );
			mat.Multiply(mat, matP);
			m_Shader3D.SetValue(m_hMatWorldViewProjection, &mat, sizeof(Matrix));
		}

		//  set render technique
		if		(flag & SX_SHADER_DEFAULT)			m_Shader3D.SetRenderTechnique(m_rtech_main);
		else if (flag & SX_SHADER_ZDEPTH)			m_Shader3D.SetRenderTechnique(m_rtech_zdepth);
		else if (flag & SX_SHADER_NORMAL)			m_Shader3D.SetRenderTechnique(m_rtech_normal);
		else if (flag & SX_SHADER_DEFERRED)			m_Shader3D.SetRenderTechnique(m_rtech_deferred);
		else if (flag & SX_SHADER_SHADOW)			m_Shader3D.SetRenderTechnique(m_rtech_shadow);
		else										m_Shader3D.SetRenderTechnique(m_rtech_main);

		//  apply the shader
		m_Shader3D.SetToDevice();
	}

public:
	sx::d3d::Shader3D	m_Shader3D;				//  main shader object
	String				m_Src;					//  Address of resource file in *.FX format
	DWORD				m_ID;					//  ID of the shader use for fast search
	DWORD				m_Option;				//  I just feel that it will be useful :)
	int					m_RefCount;				//  number of references objects

	D3DShaderHandle		m_hMatWorld;				//  handle of world transformation
	D3DShaderHandle		m_hMatWorldInv;				//  handle of inverse of world transformation
	D3DShaderHandle		m_hMatWorldView;			//  handle of world * view transformation
	D3DShaderHandle		m_hMatWorldViewProjection;	//  handle of world * view * projection transformation

	D3DShaderHandle		m_rtech_main;			//	main render technique
	D3DShaderHandle		m_rtech_zdepth;			//	render technique use to draw ZDepth
	D3DShaderHandle		m_rtech_normal;			//  render technique use to draw Normal
	D3DShaderHandle		m_rtech_deferred;		//  render technique use to deferred lighting
	D3DShaderHandle		m_rtech_shadow;			//  render technique use to draw objects shadow
	
	Array<DerivedShaderParameter*>	
		m_Param;								//  the parameter list represented by shader object
};


namespace sx { namespace d3d {

	//////////////////////////////////////////////////////////////////////////
	//  TEXTURE INTERNAL MANAGER
	//////////////////////////////////////////////////////////////////////////
	bool Shader::Manager::Exist( OUT PShader& pShdr, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);
		return ShaderMap.Find(key, pShdr);
	}

	bool Shader::Manager::Get( OUT PShader& pShdr, const WCHAR* src )
	{
		if (!src) 
		{
			pShdr = NULL;
			return false;
		}

		UINT key = sx::cmn::GetCRC32(src);

		if (ShaderMap.Find(key, pShdr))
		{
			DerivedShader* g = (DerivedShader*)pShdr;
			g->m_RefCount++;
		}
		else
		{
			if (Create(pShdr, src))
			{
				ShaderMap.Insert(key, pShdr);
			}
			else return false;
		}

		return true;	
	}

	bool Shader::Manager::Create( OUT PShader& pShdr, const WCHAR* src )
	{
		pShdr = sx_new( DerivedShader );
		if (pShdr)
		{
			pShdr->SetSource(src);
			return true;
		}
		else return false;
	}

	void Shader::Manager::Release( PShader& pShdr )
	{
		if (!pShdr) return;
		DerivedShader* g = (DerivedShader*)pShdr;

		pShdr = NULL;
		g->m_RefCount--;
		if (g->m_RefCount>0) return;

		//  remove from shader map
		PShader sh = NULL;
		if (ShaderMap.Find(g->m_ID, sh) && sh==g)
			ShaderMap.Remove(g->m_ID);

		sx_delete_and_null(g);
	}

	void Shader::Manager::ClearAll( void )
	{
		if (ShaderMap.IsEmpty()) return;

		String str = L"Releasing remains Shader(s) :\r\n\t\t\t";

		for (sxMapShader::Iterator it = ShaderMap.First(); !it.IsLast(); it++)
		{
			DerivedShader* g = (DerivedShader*)(*it);
			str << (g->GetSource()!=NULL ? g->GetSource() : L" ??? no name ???") << L"\r\n\t\t\t";
			//<< L" [ " << g->m_RefCount << L" refCount ]\r\n\t\t\t";

			sx_delete_and_null(g);
		}
		ShaderMap.Clear();

		sxLog::Log(str);
	}

	void Shader::Manager::SetShaderQuality( ShaderQuality quality )
	{
		if (s_shaderQuality == quality) return;
		s_shaderQuality = quality;

		for (sxMapShader::Iterator it = ShaderMap.First(); !it.IsLast(); it++)
		{
			(*it)->Compile(s_shaderQuality);
		}

	}

	FORCEINLINE ShaderQuality Shader::Manager::GetShaderQuality( void )
	{
		return s_shaderQuality;
	}


}} // namespace sx { namespace d3d {
