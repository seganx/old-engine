#include "sxPanelEx.h"

namespace sx { namespace gui {


	PanelEx::PanelEx( void ): Control(),
		m_pWVP(NULL),
		m_pColor(NULL),
		m_pTime(NULL),
		m_iCurState(0),
		m_pTexture(NULL),
		m_time(0)
	{
		m_Type = GUI_PANELEX;

		//  create one element
		SetElementCount(1);
		SetSize(float2(200.0f, 140.0f));

		m_oldState.Align	= math::Vector2(0.0f, 0.0f);
		m_oldState.Center	= math::Vector3(0.0f, 0.0f, 0.0f);
		m_oldState.Position	= math::Vector3(0.0f, 0.0f, 0.0f);
		m_oldState.Rotation	= math::Vector3(0.0f, 0.0f, 0.0f);
		m_oldState.Scale	= math::Vector3(1.0f, 1.0f, 1.0f);
		m_oldState.Blender	= math::Vector2(0.02f, 0.73f);
		m_oldState.Color	= math::Vector4(m_elements[0]->Color().r, m_elements[0]->Color().g,	m_elements[0]->Color().b, 1.0f);
		m_curState = m_oldState;

		m_States.PushBack(m_oldState);

		//  allow the extended panel to blend between states
		AddProperty(SX_GUI_PROPERTY_BLENDSTATES);
	}

	PanelEx::~PanelEx( void )
	{
		m_States.Clear();
		Shader_Clean();
	}

	void PanelEx::Save( Stream& S )
	{
		Control::Save(S);

		int ver = 1;
		SEGAN_STREAM_WRITE(S, ver);
		
		SEGAN_STREAM_WRITE(S, m_iCurState);

		int scount = m_States.Count();
		SEGAN_STREAM_WRITE(S, scount);
		for (int i=0; i<scount; i++)
			S.Write(&m_States[i], sizeof(GUIPanelState));
			
		cmn::String_Save(m_PS_part, &S);

		Save_Childes(S);
	}

	void PanelEx::Load( Stream& S )
	{
		Control::Load(S);

		int ver = 0;
		SEGAN_STREAM_READ(S, ver);

		if (ver == 1)
		{
			SEGAN_STREAM_READ(S, m_iCurState);


			//  clear current states
			m_States.Clear();

			//  load states
			int scount = 0;
			SEGAN_STREAM_READ(S, scount);
			for (int i=0; i<scount; i++)
			{
				GUIPanelState tmp;
				SEGAN_STREAM_READ(S, tmp);
				m_States.PushBack(tmp);
			}

			cmn::String_Load(m_PS_part, &S);
			Shader_Compile();
		}

		Load_Childes(S);
	}

	void PanelEx::SetSize( float2 S )
	{
		Control::SetSize(S);
		RectF rc = Control::GetRect();
		m_elements[0]->SetRect(rc);
	}

	FORCEINLINE Vector3& PanelEx::Position( void )
	{
		return m_States[m_iCurState].Position;
	}

	FORCEINLINE Vector3& PanelEx::Rotation( void )
	{
		return m_States[m_iCurState].Rotation;
	}

	FORCEINLINE Vector3& PanelEx::Scale( void )
	{
		return m_States[m_iCurState].Scale;
	}

	void PanelEx::Update( float elpsTime )
	{
		m_time += elpsTime * 0.001f;

		if ( !SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE) )
		{
			SEGAN_SET_ADD(m_Option, _SX_GUI_NOT_VISIBLE_);
			return;
		} else {
			SEGAN_SET_REM(m_Option, _SX_GUI_NOT_VISIBLE_);
		}

		BlendStates(elpsTime);

		if (!(m_Option & _SX_GUI_NOT_VISIBLE_))
			Control::Update(elpsTime);
	}

	void PanelEx::Draw( DWORD option )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		bool inSpace3D = ( m_Option & _SX_GUI_IN_3DSPACE_ ) != 0;
		bool just3D = ( option & SX_GUI_DRAW_JUST3D ) != 0;
		bool just2D = ( option & SX_GUI_DRAW_JUST2D ) != 0;
		if ( just2D || just3D )
		{
			if ( just3D != inSpace3D )
				return;
		}

		//  turn alpha blend on
		d3d::Device3D::RS_Alpha( SX_MATERIAL_ALPHABLEND );

		float alpha = ( m_Option & SX_GUI_PROPERTY_IGNOREBLEND ) ? Element::AlphaPercent() * 2 : Element::AlphaPercent();
		m_curState.Color.w *= alpha;

		if (m_Shader.Exist()) //  set scene shader
		{
			
			static sx::math::Matrix mat_proj_last, mat_view_last;
			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Get(mat_proj_last);
				d3d::Device3D::Matrix_View_Get(mat_view_last);
			}	
			if (m_pTexture) m_pTexture->SetToDevice(1);
			
			if ( inSpace3D )
			{
				d3d::Device3D::RS_ZEnabled(true);

				//  set parameters
				math::Matrix matWVP;
				matWVP.Multiply(m_Mtrx, mat_view_last);
				matWVP.Multiply(matWVP, mat_proj_last);
				m_Shader.SetValue(m_pWVP, &matWVP, sizeof(math::Matrix));
				m_Shader.SetValue(m_pTime, &m_time, sizeof(m_time));
				m_Shader.SetValue(m_pColor, &m_curState.Color, sizeof(math::Vector4));
				m_Shader.SetToDevice();

				for (int i=0; i<m_elements.Count(); i++) m_elements[i]->Draw(m_Option | option);
			}
			else
			{
				math::Matrix matView = math::MTRX_IDENTICAL;
				math::Matrix matProj(0.0f,0.0f,0.0f,0.0f,0.0f,1.9203779f,0.0f,0.0f,0.0f,0.0f,1.0001251f,1.0f,0.0f,0.0f,-0.50006253f,0.0f);
				matView._43 = Globals::Golden_FOV() * (float)d3d::Device3D::Viewport()->Height;
				matProj._11 = 1.9203779f * ((float)d3d::Device3D::Viewport()->Height / (float)d3d::Device3D::Viewport()->Width);

				d3d::Device3D::RS_ZEnabled(false);

				//  set parameters
				math::Matrix matWVP;
				matWVP.Multiply(m_Mtrx, matView);
				matWVP.Multiply(matWVP, matProj);
				m_Shader.SetValue(m_pWVP, &matWVP, sizeof(math::Matrix));
				m_Shader.SetValue(m_pTime, &m_time, sizeof(m_time));
				m_Shader.SetValue(m_pColor, &m_curState.Color, sizeof(math::Vector4));
				m_Shader.SetToDevice();

				for (int i=0; i<m_elements.Count(); i++) m_elements[i]->Draw(m_Option | option);
			}

			d3d::Device3D::SetEffect(NULL);
			d3d::Device3D::SetTexture(0, NULL);
			d3d::Device3D::SetTexture(1, NULL);
			d3d::Device3D::SetTexture(2, NULL);
			
			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Set(mat_proj_last);
				d3d::Device3D::Matrix_View_Set(mat_view_last);
			}
		}
		else
		{
			d3d::Device3D::SetEffect(NULL);
			Control::Draw(option);
		}

		if (m_Option & SX_GUI_PROPERTY_BLENDCHILDS)
		{
			Element::AlphaPercent() = m_curState.Color.w;
			Control::Draw_Childes(option);
		} 
		else Control::Draw_Childes(option);

		Element::AlphaPercent() = alpha;
	}

	void PanelEx::State_Clear( void )
	{
		for (int i=m_States.Count()-1; i>0; i--)
			m_States.RemoveByIndex(i);

		State_SetIndex(0);
	}

	FORCEINLINE int PanelEx::State_Count( void )
	{
		return m_States.Count();
	}

	FORCEINLINE int PanelEx::State_Add( void )
	{
		GUIPanelState newone = m_States[m_iCurState];
		m_States.PushBack( newone );
		return m_States.Count() - 1;
	}

	void PanelEx::State_Remove( int index )
	{
		if (index<0 || index>=m_States.Count() || m_States.Count()<2) return;

		m_States.RemoveByIndex(index);
		if (m_iCurState >= m_States.Count()) m_iCurState = m_States.Count()-1;
	}

	FORCEINLINE int PanelEx::State_GetIndex( void )
	{
		return m_iCurState;
	}

	void PanelEx::State_SetIndex( int index )
	{
		if (index<0 || index>=m_States.Count()) return;

		if (!(m_Option & SX_GUI_PROPERTY_BLENDSTATES))
		{
			m_WBlend.SetWeight(1.0f);
			m_iCurState = index;
			m_curState	= m_States[m_iCurState];
			m_oldState	= m_curState;
			return;
		}

		if (index != m_iCurState)
		{
			m_WBlend.Velocity()	= m_States[index].Blender[0];
			m_WBlend.Amplitude()= m_States[index].Blender[1];
			m_WBlend.Reset();
			if ( m_WBlend.Amplitude() > 0.99 )
			{
				m_oldState = m_States[m_iCurState];
				//m_curState = m_oldState;
			}
			else m_oldState = m_curState;

			m_iCurState = index;
		}
	}

	FORCEINLINE GUIPanelState& PanelEx::State_GetCurrent( void )
	{
		return m_States[m_iCurState];
	}

	FORCEINLINE GUIPanelState& PanelEx::State_GetByIndex( int index )
	{
		if (index<0 || index>=m_States.Count()) index = 0;

		return m_States[index];
	}

	FORCEINLINE GUIPanelState& PanelEx::State_GetBlended( void )
	{
		return m_curState;
	}

	bool PanelEx::State_IsBlending( void )
	{
		return m_WBlend.GetWeight(0) != 1.0f;
	}

	FORCEINLINE void PanelEx::Shader_Set( const WCHAR* strShader )
	{
		m_PS_part = strShader;
	}

	FORCEINLINE const WCHAR* PanelEx::Shader_Get( void )
	{
		return m_PS_part;
	}

	void PanelEx::Shader_Compile( void )
	{
		Shader_Clean();

		if ( !d3d::Device3D::Shader_Supported() ) return;

		String p1 = 
			L"matrix WVP;"\
			L"float4 clDiffuse;\n"\
			L"float fTime;\n"\

			L"void VS(in  float4 vPos : POSITION,\n"\
			L"in  float2 vTex : TEXCOORD0,\n"\

			L"out float4 oPos : POSITION,\n"\
			L"out float2 oTex : TEXCOORD0,\n"\
			L"out float4 oTes : TEXCOORD1,\n"\
			L"out float  oTim : TEXCOORD2,\n"\
			L"out float4 oDif : TEXCOORD3){\n"\
			L"oPos = mul(vPos , WVP);\n"\
			L"oTex = vTex;\n"\
			L"oTes = oPos;\n"\
			L"oDif = clDiffuse;\n"\
			L"oTim = fTime;\n"\
			L"}\n"\
			
			L"sampler2D samp0 : register(s0) = sampler_state {\n"\
			L"AddressU=Wrap;\n"\
			L"AddressV=Wrap;\n"\
			L"MipFilter=LINEAR;\n"\
			L"MagFilter=LINEAR;};\n"\

			L"sampler2D samp1 : register(s1) = sampler_state {\n"\
			L"AddressU=Wrap;\n"\
			L"AddressV=Wrap;\n"\
			L"MipFilter=LINEAR;\n"\
			L"MagFilter=LINEAR;};\n"\

			L"sampler2D samp2 : register(s2) = sampler_state {\n"\
			L"AddressU=Wrap;\n"\
			L"AddressV=Wrap;\n"\
			L"MipFilter=LINEAR;\n"\
			L"MagFilter=LINEAR;};\n"\


			L"float4 PS(  float2 tex0   : TEXCOORD0,\n"\
			L"float4 tex1   : TEXCOORD1,\n"\
			L"float  time  : TEXCOORD2,\n"\
			L"float4 color : TEXCOORD3) : COLOR\n{\n"\
			L"tex1.x = (0.5*tex1.x/tex1.w)+0.5;\n"\
			L"tex1.y =-(0.5*tex1.y/tex1.w)+0.5;\n";


		String p2 =   L"\n }\n"\

			L"technique RenderGUI{\n"\
			L"pass P0{\n"\
			L"VertexShader = compile vs_2_0 VS();\n"\
			L"PixelShader  = compile ps_2_0 PS();\n"\
			L"}}\n";

		p1 << m_PS_part << p2;

		p2 = m_Name; p2<<L" > pixel shader ";
		m_Shader.CompileShader(p1, SQ_HIGH, p2);
		m_pWVP		= m_Shader.GetParameter(0);
		m_pColor	= m_Shader.GetParameter(1);
		m_pTime		= m_Shader.GetParameter(2);

	}

	void PanelEx::Shader_Clean( void )
	{
		m_pWVP		= NULL;
		m_pColor	= NULL;
		m_pTime		= NULL;
		m_Shader.CompileShader(NULL, SQ_HIGH);
	}

	FORCEINLINE void PanelEx::SetSceneTexture( d3d::PTexture3D pSceneTexture )
	{
		m_pTexture = pSceneTexture;
	}

	void PanelEx::BlendStates( float elpsTime )
	{
		if ( elpsTime > -1 && m_Option & SX_GUI_PROPERTY_BLENDSTATES )
		{
			float w = m_WBlend.GetWeight(elpsTime);
			m_curState.Align.Lerp(		m_oldState.Align,		m_States[m_iCurState].Align,	w);
			m_curState.Center.Lerp(		m_oldState.Center,		m_States[m_iCurState].Center,	w);
			m_curState.Position.Lerp(	m_oldState.Position,	m_States[m_iCurState].Position, w);
			m_curState.Rotation.Lerp(	m_oldState.Rotation,	m_States[m_iCurState].Rotation, w);
			m_curState.Scale.Lerp(		m_oldState.Scale,		m_States[m_iCurState].Scale,	w);
			m_curState.Color.Lerp(		m_oldState.Color,		m_States[m_iCurState].Color,	w);
		}
		else
		{
			m_curState = m_States[m_iCurState];
			m_oldState = m_curState;
		}

		if ((m_curState.Color[3]<SX_GUI_MINIMUM_ALPHA) || 
			(m_curState.Scale[0]<SX_GUI_MINIMUM_SCALE) || 
			(m_curState.Scale[1]<SX_GUI_MINIMUM_SCALE) || 
			(m_curState.Scale[2]<SX_GUI_MINIMUM_SCALE) )
		{
			SEGAN_SET_ADD(m_Option, _SX_GUI_NOT_VISIBLE_);
		}
		else
		{
			//  Apply color ...
			m_elements[0]->Color().r = m_curState.Color[0];
			m_elements[0]->Color().g = m_curState.Color[1];
			m_elements[0]->Color().b = m_curState.Color[2];
			m_elements[0]->Color().a = m_curState.Color[3];
		}
	}

	void PanelEx::BuildMatrix( void )
	{
		if (m_Option & _SX_GUI_NOT_VISIBLE_) return;

		//  check and set visibility 
		float sclvalue = m_Option & _SX_GUI_IN_3DSPACE_ ? 0.01f : 1.0f;
		math::Vector3 scl(
			m_curState.Scale[0] * m_SclOffset[0] * sclvalue,
			m_curState.Scale[1] * m_SclOffset[1] * sclvalue,
			m_curState.Scale[2] * m_SclOffset[2] * sclvalue);

		if (scl.x<SX_GUI_MINIMUM_SCALE || scl.y<SX_GUI_MINIMUM_SCALE || scl.z<SX_GUI_MINIMUM_SCALE)
		{
			SEGAN_SET_ADD(m_Option, _SX_GUI_NOT_VISIBLE_);
			return;
		}

		math::Matrix mat, matTmp;

		//  Apply center
		mat.Translation(m_curState.Center[0]*m_Size[0], m_curState.Center[1]*m_Size[1], m_curState.Center[2]);

		//  apply scale to matrix
		matTmp.Scale( scl.x, scl.y, scl.z );
		mat.Multiply(mat, matTmp);

		//  Apply rotation
		matTmp.RotationPYR(
			m_curState.Rotation[0] + m_RotOffset[0],
			m_curState.Rotation[1] + m_RotOffset[1],
			m_curState.Rotation[2] + m_RotOffset[2]);
		m_Mtrx.Multiply(mat, matTmp);

		//  Apply aligned position
		if ( m_Option & SX_GUI_PROPERTY_BILLBOARD && !m_Parent )
		{
			if ( m_Option & _SX_GUI_IN_3DSPACE_ )
			{
				Matrix matView, matViewInv;
				d3d::Device3D::Matrix_View_Get( matView );
				matViewInv.Inverse( matView );

				m_Mtrx.Multiply( m_Mtrx, matViewInv );
			}
			else
			{
				Matrix matView, matProj;
				d3d::Device3D::Matrix_View_Get( matView );
				d3d::Device3D::Matrix_Project_Get( matProj );

				D3DViewport VP = *d3d::Device3D::Viewport();

				float3 viewPos = m_Pos + m_PosOffset;
				viewPos.ProjectToScreen(viewPos, math::MTRX_IDENTICAL, matView, matProj, VP);

				m_Mtrx._41 = viewPos.x - VP.Width*0.5f;
				m_Mtrx._42 = - viewPos.y + VP.Height*0.5f;;
				m_Mtrx._43 = m_Pos.z + m_PosOffset.z;
			}
		}
		else
		{
			m_Mtrx._41 += sclvalue * (m_curState.Position[0] + m_PosOffset[0] + d3d::Device3D::Viewport()->Width  * m_curState.Align[0]);
			m_Mtrx._42 += sclvalue * (m_curState.Position[1] + m_PosOffset[1] + d3d::Device3D::Viewport()->Height * m_curState.Align[1]);
			m_Mtrx._43 += sclvalue * (m_curState.Position[2] + m_PosOffset[2]);
		}

	}



}} //  namespace sx { namespace gui {