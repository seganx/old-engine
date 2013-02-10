#include "sxMaterial.h"
#include "sxDevice3D.h"
#include "../sxCommon/sxCommon.h"

/*
NOTE:	in this class I use a member named 'reserved' to do some operation.
first byte of reserved used to store validation level contain 0,1,2 ...
second byte of reserved used to store validation count. validation count must be less that 3

the other bytes will describe later
*/
#define VALIDATION_LEVEL	SEGAN_1TH_BYTEOF(reserved)
#define VALIDATION_COUNT	SEGAN_2TH_BYTEOF(reserved)
#define VALIDATION_NONE		0xff


namespace sx { namespace d3d {

	Material::Material()
		: m_Option(SX_MATERIAL_CULLING | SX_MATERIAL_ZENABLE | SX_MATERIAL_ZWRITING), 
		m_Shader(NULL), 
		reserved(0)
	{
		for (int i=0; i<MATERIAL_FLOAT_COUNT; i++)
			m_Float[i] = FLT_MAX;

		for (int i=0; i<MATERIAL_FLOAT4_COUNT; i++)
			m_Float4[i] = math::VEC4_MAX;

		for (int i=0; i<MATERIAL_TEXTURE_COUNT; i++)
			m_Texture[i] = NULL;

		VALIDATION_LEVEL = VALIDATION_NONE;
		VALIDATION_COUNT = 0;
	}

	Material::~Material()
	{
		Invalidate();

		for (int i=0; i<MATERIAL_TEXTURE_COUNT; i++)
			d3d::Texture::Manager::Release(m_Texture[i]);

		d3d::Shader::Manager::Release( m_Shader );
	}

	void Material::Save( Stream& stream )
	{
		String str;
		int version = 3;
		
		//  write version
		SEGAN_STREAM_WRITE(stream, version);

		//  write shader
		str.Clear();
		if (m_Shader)	str = m_Shader->GetSource();
		cmn::String_Save(str, &stream);

		//  write floats
		for (int i=0; i<MATERIAL_FLOAT_COUNT; i++)
			SEGAN_STREAM_WRITE(stream, m_Float[i]);

		//  write vectors
		for (int i=0; i<MATERIAL_FLOAT4_COUNT; i++)
			SEGAN_STREAM_WRITE(stream, m_Float4[i]);

		//  write textures
		for (int i=0; i<MATERIAL_TEXTURE_COUNT; i++)
		{
			str.Clear();
			if (m_Texture[i])	str = m_Texture[i]->GetSource();
			cmn::String_Save(str, &stream);
		}

		//  write options
		SEGAN_STREAM_WRITE(stream, m_Option);
	}

	void Material::Load( Stream& stream )
	{
		String str;
		int version = 0;

		//  read version
		SEGAN_STREAM_READ(stream, version);

		if (version >= 1)
		{
			//  read shader
			str.Clear();
			cmn::String_Load(str, &stream);
			SetShader(str);
			
			//  read floats
			int maxFloat = (version>=3) ? MATERIAL_FLOAT_COUNT : 4;			//  <- RISK : changing version
			for (int i=0; i<maxFloat; i++)
				SEGAN_STREAM_READ(stream, m_Float[i]);

			//  read vectors
			for (int i=0; i<MATERIAL_FLOAT4_COUNT; i++)
				SEGAN_STREAM_READ(stream, m_Float4[i]);

			//  read textures
			for (int i=0; i<MATERIAL_TEXTURE_COUNT; i++)
			{
				str.Clear();
				cmn::String_Load(str, &stream);
				SetTexture(i, str);
			}
		}

		m_Option = SX_MATERIAL_CULLING | SX_MATERIAL_ZENABLE | SX_MATERIAL_ZWRITING;
		if (version >= 2)
		{
			//  read options
			SEGAN_STREAM_READ(stream, m_Option);
		}
	}

	void Material::SetShader( const WCHAR* src )
	{
		sx::d3d::Shader::Manager::Release(m_Shader);
		sx::d3d::Shader::Manager::Get(m_Shader, src);

		if ( m_Shader )
		{
			int iFloat=0; int iFloat4=0;
			PShaderParameter param = NULL;
			for (int i=0; i<m_Shader->GetParamCount(); i++)
			{
				param = m_Shader->GetParam(i);
				if (param)
				{
					switch (param->GetDesc()->Type)
					{
					case SPT_FLOAT:		
						if ( iFloat<MATERIAL_FLOAT_COUNT )
							m_Float[iFloat] = param->GetDesc()->uiData[2];
						iFloat++;
						break;
					case SPT_FLOAT4:
						if ( iFloat4<MATERIAL_FLOAT4_COUNT )
							m_Float4[iFloat4] = param->GetDesc()->uiData;
						iFloat4++;
						break;
					}
				}
			}
		}
	}

	const WCHAR* Material::GetShader( void )
	{
		if (m_Shader)
			return m_Shader->GetSource();
		else
			return NULL;
	}

	void Material::SetTexture( const int index, const WCHAR* src )
	{
		if (index<0 || index>=MATERIAL_TEXTURE_COUNT) return;
		if (src && m_Texture[index] && (wcscmp(m_Texture[index]->GetSource(), src) == 0)) 
			return;

		PTexture TX = NULL;
		if ( d3d::Texture::Manager::Get(TX, src) )
		{
			for (int i=0; i<VALIDATION_COUNT; i++)
			{
				TX->Activate( VALIDATION_LEVEL );
			}
		}

		if (m_Texture[index])
		{
			for (int i=0; i<VALIDATION_COUNT; i++)
			{
				m_Texture[index]->Deactivate();
			}

			d3d::Texture::Manager::Release(m_Texture[index]);
		}
		m_Texture[index] = TX;

	}

	const WCHAR* Material::GetTexture( const int index )
	{
		if (index<0 || index>=MATERIAL_TEXTURE_COUNT) return NULL;

		if (m_Texture[index])
			return m_Texture[index]->GetSource();
		else
			return NULL;
	}

	FORCEINLINE void Material::SetOption( DWORD SX_MATERIAL_ option )
	{
		m_Option = option;
	}

	FORCEINLINE DWORD Material::GetOption( void )
	{
		return m_Option;
	}

	FORCEINLINE void Material::SetFloat( const int index, const float value )
	{
		if (index>=0 && index<MATERIAL_FLOAT_COUNT)
			m_Float[index] = value;
	}

	FORCEINLINE float Material::GetFloat( const int index )
	{
		if (index>=0 && index<MATERIAL_FLOAT_COUNT)
			return m_Float[index];
		else
			return 0.0f;
	}

	FORCEINLINE void Material::SetFloat4( const int index, const float4& value )
	{
		if (index>=0 && index<MATERIAL_FLOAT4_COUNT)
			m_Float4[index] = value;
	}

	FORCEINLINE float4 Material::GetFloat4( const int index )
	{
		if (index>=0 && index<MATERIAL_FLOAT4_COUNT)
			return m_Float4[index];
		else
			return float4(0,0,0,0);
	}

	void Material::Validate( int LOD /*= 0*/ )
	{
		if ( LOD < VALIDATION_LEVEL )
		{
			VALIDATION_LEVEL = LOD;
			VALIDATION_COUNT += 1;

			for (int i=0; i<MATERIAL_TEXTURE_COUNT; i++)
			{
				if (m_Texture[i]) 
					m_Texture[i]->Activate(LOD);
			}
		}
	}

	void Material::Invalidate()
	{
		if ( VALIDATION_LEVEL != VALIDATION_NONE )
		{
			VALIDATION_LEVEL = VALIDATION_NONE;

			while (VALIDATION_COUNT)
			{
				for (int i=0; i<MATERIAL_TEXTURE_COUNT; i++)
				{
					if (m_Texture[i])
					{
						//sxLog::Log(L"start texture [ %d ] invalidation", i);
						m_Texture[i]->Deactivate();
						//sxLog::Log(L"end texture [ %d ] invalidation", i);
					}
				}

				VALIDATION_COUNT -= 1;
			}
		}
	}

	void Material::SetToDevice( DWORD flag )
	{
		for (int i=0; i<MATERIAL_TEXTURE_COUNT; i++)
		{
			if (m_Texture[i])
				m_Texture[i]->SetToDevice(i);
			else
				Device3D::SetTexture(i, NULL);
		}

		if ( m_Shader )
		{
			int iFloat=0; int iFloat4=0;
			PShaderParameter param = NULL;
			for (int i=0; i<m_Shader->GetParamCount(); i++)
			{
				param = m_Shader->GetParam(i);
				if (param)
				{
					switch (param->GetDesc()->Type)
					{
					case SPT_FLOAT:		
						if ( iFloat<MATERIAL_FLOAT_COUNT )
							param->SetFloat( m_Float[iFloat] );
						iFloat++;
						break;
					case SPT_FLOAT4:
						if ( iFloat4<MATERIAL_FLOAT4_COUNT )
							param->SetFloat4( m_Float4[iFloat4] );
						iFloat4++;
						break;
					}
				}
			}

			m_Shader->SetToDevice( flag );
		}
		else
		{
			d3d::Device3D::SetEffect(NULL);
		}

		if ( flag & SX_SHADER_ZDEPTH  || flag & SX_SHADER_NORMAL )
			Device3D::RS_Alpha( 0 );
		else
			Device3D::RS_Alpha( m_Option );

		Device3D::RS_Culling(		SEGAN_SET_HAS(m_Option, SX_MATERIAL_CULLING)/*, SEGAN_SET_HAS(m_Option, flag & SX_SHADER_ZDEPTH)*/ );
		Device3D::RS_ZEnabled(		SEGAN_SET_HAS(m_Option, SX_MATERIAL_ZENABLE) );
		Device3D::RS_ZWritable(		SEGAN_SET_HAS(m_Option, SX_MATERIAL_ZWRITING) );
		
	}


}} // namespace sx { namespace d3d {