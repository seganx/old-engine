#include "sxProgressBar.h"

#define CLEANUP_BUFFER() {d3d::Resource3D::ReleaseVertexBuffer(m_vertexBuffer0); d3d::Resource3D::ReleaseVertexBuffer(m_vertexBuffer1);}

namespace sx { namespace gui {

	ProgressBar::ProgressBar( void ): Control(),
		m_Max(1.0f), 
		m_Value(0.0f), 
		m_ValueScroll(1.0f), 
		m_vertexBuffer0(NULL),
		m_vertexBuffer1(NULL)
	{
		m_Type = GUI_PROGRESSBAR;

		//  create two element
		SetElementCount(2);
		SetSize(float2(80.0f, 20.0f));
		SetValue(0.0f);
	}

	ProgressBar::~ProgressBar( void )
	{
		CLEANUP_BUFFER();
	}

	void ProgressBar::Save( Stream& stream )
	{
		Control::Save(stream);

		int ver = 1;
		SEGAN_STREAM_WRITE(stream, ver);
		SEGAN_STREAM_WRITE(stream, m_Max);
		SEGAN_STREAM_WRITE(stream, m_Value);

		Save_Childes(stream);
	}

	void ProgressBar::Load( Stream& stream )
	{
		Control::Load(stream);

		int ver = 0;
		SEGAN_STREAM_READ(stream, ver);

		if ( ver == 1 )
		{
			SEGAN_STREAM_READ(stream, m_Max);
			SEGAN_STREAM_READ(stream, m_Value);
		}

		SetMax(m_Max);
		SetValue(m_Value);

		Load_Childes(stream);
	}

	void ProgressBar::SetSize( float2 S )
	{
		Control::SetSize(S);
		RectF rc = Control::GetRect();
		m_elements[0]->SetRect(rc);
		m_elements[1]->SetRect(rc);
		//m_elements[1]->Matrix()._43 = SX_GUI_Z_BIAS;
	}

	float ProgressBar::GetMax( void )
	{
		return m_Max;
	}

	void ProgressBar::SetMax( float v )
	{
		if (v<0)
			return;

		m_Max = v;
		if (m_Max == 0)	m_Max = EPSILON;

		if (m_Value > m_Max)
			SetValue(m_Max);
	}

	float ProgressBar::GetValue( void )
	{
		return m_Value;
	}

	void ProgressBar::SetValue( float v )
	{
		m_Value = v;
		if (m_Value > m_Max)
			m_Value = m_Max;
	}

	void ProgressBar::Update( float elpsTime )
	{
		Control::Update(elpsTime);

		m_ValueScroll += (m_Value - m_ValueScroll) * (elpsTime * 0.06f) * 0.15f;
		SEGAN_CLAMP(m_ValueScroll, 0, m_Max);

		if ( !(m_Option & SX_GUI_PROPERTY_PROGRESSCIRCLE) && sx_abs_f( m_ValueScroll - m_Value) > EPSILON )
		{
			float val;
			if (m_Max)
				val = (float)(m_ValueScroll / m_Max);
			else
				val = SX_GUI_MINIMUM_SCALE;
			if (val < SX_GUI_MINIMUM_SCALE) val = SX_GUI_MINIMUM_SCALE;

			if ( m_Option & SX_GUI_PROPERTY_PROGRESSUV )
			{
				RectF rc = m_elements[1]->GetRect();
				rc.x2 = rc.x1 + m_Size.x * val;
				m_elements[1]->SetRect( rc, float2(0, 0), float2(val, 0), float2(0, 1), float2(val, 1) );
			}
			else
			{
				m_elements[1]->Matrix().Scale( val, 1.0f, 1.0f );
				m_elements[1]->Matrix()._41 = (val - 1) * m_Size.x * 0.5f;
			}
		}
	}

	void ProgressBar::Draw( DWORD option )
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

		if ( inSpace3D )
		{
			d3d::Device3D::RS_ZEnabled(true);
			DrawProgress(option);
		}
		else
		{
			sx::math::Matrix mat_proj_last, mat_view_last;
			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Get(mat_proj_last);
				d3d::Device3D::Matrix_View_Get(mat_view_last);
			}			

			static math::Matrix matView = math::MTRX_IDENTICAL;
			static math::Matrix matProj(0.0f,0.0f,0.0f,0.0f,0.0f,1.9203779f,0.0f,0.0f,0.0f,0.0f,1.0001251f,1.0f,0.0f,0.0f,-0.50006253f,0.0f);

			matView._43 = Globals::Golden_FOV() * (float)d3d::Device3D::Viewport()->Height;
			matProj._11 = 1.9203779f * ((float)d3d::Device3D::Viewport()->Height / (float)d3d::Device3D::Viewport()->Width);

			d3d::Device3D::Matrix_View_Set(matView);
			d3d::Device3D::Matrix_Project_Set(matProj);

			d3d::Device3D::RS_ZEnabled(false);
			DrawProgress(option);

			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Set(mat_proj_last);
				d3d::Device3D::Matrix_View_Set(mat_view_last);
			}
		}

		Control::Draw_Childes(option);
	}

	void ProgressBar::BurnVB( void )
	{
		if ( !m_vertexBuffer0 && !d3d::Resource3D::CreateVertexBuffer(182 * SEGAN_SIZE_VERTEX_0, m_vertexBuffer0) ) return;
		if ( !m_vertexBuffer1 && !d3d::Resource3D::CreateVertexBuffer(182 * SEGAN_SIZE_VERTEX_1, m_vertexBuffer1) ) return;
		
		PD3DVertex0 v0;
		if ( SUCCEEDED( m_vertexBuffer0->Lock(0, 0, (void**)&v0, 0) ) )
		{
			PD3DVertex1 v1;
			if ( SUCCEEDED ( m_vertexBuffer1->Lock(0, 0, (void**)&v1, 0) ) )
			{
				v0[0].pos	= Vector3(0.0f, 0.0f, 0.0f);
				v1[0].txc	= Vector2(0.5f, 0.5f);
				v1[0].nrm	= Vector3(0.0f, 1.0f, 0.0f);
				v1[0].col0	= 0xFFFFFFFF;
				v1[0].col1	= 0xFFFFFFFF;

				float W = m_Size.x * 0.5f;
				float H = m_Size.y * 0.5f;
				float D = PI / 90.0f;
				float sinD , cosD;

				for (int i=1; i<182; i++)
				{
					cosD = cos( (float)(i-1) * D + math::PIDIV2 );
					sinD = sin( (float)(i-1) * D + math::PIDIV2 );

					v0[i].pos.x = - W * cosD;
					v0[i].pos.y = + H * sinD;
					v0[i].pos.z = 0.0f;

					v1[i].txc.x = 0.5f - cosD * 0.5f;
					v1[i].txc.y = 0.5f - sinD * 0.5f;
					v1[i].nrm	= Vector3(0.0f, 1.0f, 0.0f);
					v1[i].col0	= 0xFFFFFFFF;
					v1[i].col1	= 0xFFFFFFFF;
				}

				m_vertexBuffer1->Unlock();
			}
			else d3d::Resource3D::ReleaseVertexBuffer(m_vertexBuffer1);

			m_vertexBuffer0->Unlock();
		} else d3d::Resource3D::ReleaseVertexBuffer(m_vertexBuffer0);
	}

	void ProgressBar::DrawProgress( DWORD option )
	{
		if ( m_Option & SX_GUI_PROPERTY_PROGRESSCIRCLE )
		{
			if ( !m_vertexBuffer0 || !m_vertexBuffer1 )
				BurnVB();

			//  draw background
			m_elements[0]->Draw(option);

			//  draw progress 
			if (m_Max)
			{
				m_elements[1]->SetTextureToDevice(0);
				
				d3d::Device3D::Matrix_World_Set(m_Mtrx);
				d3d::Device3D::SetMaterialColor( m_elements[1]->Color() );
				d3d::Device3D::SetIndexBuffer(NULL);
				d3d::Device3D::SetVertexBuffer(0, m_vertexBuffer0,	SEGAN_SIZE_VERTEX_0);
				d3d::Device3D::SetVertexBuffer(1, m_vertexBuffer1,	SEGAN_SIZE_VERTEX_1);
				d3d::Device3D::DrawPrimitive(D3DPT_TRIANGLEFAN, 0, (UINT)int(181 * (m_ValueScroll / m_Max)));	
			}
		}
		else
		{
			if ( m_vertexBuffer0 || m_vertexBuffer1 )
			{
				CLEANUP_BUFFER();
			}

			for (int i=0; i<m_elements.Count(); i++) m_elements[i]->Draw(option);
		}

	}


}}	//  namespace sx { namespace gui {