#include "sxTrackBar.h"

namespace sx { namespace gui {


	TrackBar::TrackBar( void ): Control(), 
		m_Min(0.0f), 
		m_Max(1.0f), 
		m_Value(0.0f), 
		m_ValueScroll(0.0f), 
		m_LastMouse(-1.0f, -1.0f), 
		m_OnScroll(NULL, NULL)
	{
		m_Type = GUI_TRACKBAR;

		//  create two element
		SetElementCount(2);
		SetSize(float2(150.0f, 50.0f));
		SetValue(0.0f);

		AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	}

	TrackBar::~TrackBar( void )
	{

	}

	void TrackBar::Save( Stream& stream )
	{
		Control::Save(stream);

		SEGAN_STREAM_WRITE(stream, m_Min);
		SEGAN_STREAM_WRITE(stream, m_Max);
		SEGAN_STREAM_WRITE(stream, m_Value);

		Save_Childes(stream);
	}

	void TrackBar::Load( Stream& stream )
	{
		Control::Load(stream);

		SEGAN_STREAM_READ(stream, m_Min);
		SEGAN_STREAM_READ(stream, m_Max);
		SEGAN_STREAM_READ(stream, m_Value);

		SetMin(m_Min);
		SetMax(m_Max);
		SetValue(m_Value);

		Load_Childes(stream);
	}

	void TrackBar::SetSize( float2 S )
	{
		Control::SetSize(S);
		RectF rc = Control::GetRect();
		m_elements[0]->SetRect(rc);

		m_elements[1]->SetRect( RectF(-rc.y1, rc.y1, -rc.y2, rc.y2) );
		//m_elements[1]->Matrix()._43 = SX_GUI_Z_BIAS;

		SetValue(m_Value);
	}

	int TrackBar::MouseOver( float absX, float absY )
	{
		int res = Control::MouseOver(absX, absY);

		if ( m_Option & SX_GUI_PROPERTY_AUTOSIZE )
		{
			math::Vector2 uv;
			if ( m_SelectedElement >= 0 && Control::MouseOver(absX, absY, uv, 0) > -1 )
			{
				float mX = uv.x * m_Size.x;
				RectF rc = Control::GetRect();
				float W = m_Size.y * 0.5f;	//  size of movable object
				if (mX > rc.x2-W) 
					mX = rc.x2-W;
				else
				{
					if (mX < rc.x1+W) 
						mX = rc.x1+W;
					else
						m_LastMouse = float2(absX, absY);
				}

				//  Setup new value ...
				mX -= (rc.x1 + W);
				W = m_Size.x - W * 2.0f;
				float v = m_Min + (mX / W) * (m_Max - m_Min);
				SetValue( sx::cmn::Round( v ) );
			}
			return res;
		}

		if (m_SelectedElement == 1)
		{
			//if (res == 0)
			//{
			//	float2 uv;
			//	static math::Matrix matView, matProjection;
			//	if (SEGAN_SET_HAS(m_Option, _SX_GUI_IN_3DSPACE_))
			//	{
			//		d3d::Device3D::Matrix_View_Get(matView);
			//		d3d::Device3D::Matrix_Project_Get(matProjection);
			//		m_elements[0]->CheckCursor(absX, absY, matView, matProjection, uv);
			//	}
			//	else
			//	{
			//		matView.LookAtLH(float3(0.0f, 0.0f, - Globals::Golden_FOV() * d3d::Device3D::Viewport()->Height), math::VEC3_ZERO, float3(0.0f, 1.0f, 0.0f));
			//		matProjection.PerspectiveFovLH(	Globals::Golden_FOV(), (float)d3d::Device3D::Viewport()->Width / (float)d3d::Device3D::Viewport()->Height, 0.1f, 1000.0f);
			//		m_elements[0]->CheckCursor(absX, absY, matView, matProjection, uv);	
			//	}

			//	float mX = uv.x * m_Size.x;
			//	RectF rc = Control::GetRect();
			//	float W = m_Size.y * 0.5f;	//  size of movable object
			//	if (mX > rc.x2-W) 
			//		mX = rc.x2-W;
			//	else
			//	{
			//		if (mX < rc.x1+W) 
			//			mX = rc.x1+W;
			//		else
			//			m_LastMouse = float2(absX, absY);
			//	}

			//	//  Setup new value ...
			//	mX -= (rc.x1 + W);
			//	W = m_Size.x - W * 2.0f;
			//	SetValue( m_Min + (mX / W) * (m_Max - m_Min) );

			//	m_LastMouse = float2(-1.0f, -1.0f);
			//}
			//else
			{
				if (m_LastMouse.x < -0.5f) 
					m_LastMouse = float2(absX, absY);

				//  Setup mouse position in 3d space ...
				math::Vector3 mV;
				mV = math::Vector3(absX - m_LastMouse.x, m_LastMouse.y - absY, 0);

				static math::Matrix mat, matV;
				if (m_Option & _SX_GUI_IN_3DSPACE_)
					d3d::Device3D::Matrix_View_Get(matV);
				else
					matV.LookAtLH(float3(0.0f, 0.0f, Globals::Golden_Z()), math::VEC3_ZERO, math::VEC3_UP);
				mat.Multiply(m_Mtrx, matV);

				if (m_Option & _SX_GUI_IN_3DSPACE_)
					mV *=  0.002f * mat._43;

				mat.Inverse(mat);
				mV.Transform_Norm(mV, mat);

				//  Change position of movable object ...
				float mX = m_elements[1]->Matrix()._41;
				mX += mV.x;

				RectF rc = Control::GetRect();
				float W = m_Size.y * 0.5f;	//  size of movable object
				if (mX > rc.x2-W) 
					mX = rc.x2-W;
				else
				{
					if (mX < rc.x1+W) 
						mX = rc.x1+W;
					else
						m_LastMouse = float2(absX, absY);
				}

				//  Setup new value ...
				mX -= (rc.x1 + W);
				W = m_Size.x - W * 2.0f;
				m_ValueScroll = m_Min + (mX / W) * (m_Max - m_Min);

// 				if ( m_Option & SX_GUI_PROPERTY_AUTOSIZE )
// 					SetValue( sx::cmn::Round( m_ValueScroll ) );
// 				else
					SetValue( m_ValueScroll );
			}
		}
		else  m_LastMouse = float2(-1.0f, -1.0f);

		return res;
	}

	int TrackBar::MouseDown( float absX, float absY )
	{
		int res = Control::MouseDown(absX, absY);

		if (res == 0)
		{
			math::Vector2 uv;
			Control::MouseOver(absX, absY, uv);

			float mX = uv.x * m_Size.x;
			RectF rc = Control::GetRect();
			float W = m_Size.y * 0.5f;	//  size of movable object
			if (mX > rc.x2-W) 
				mX = rc.x2-W;
			else
			{
				if (mX < rc.x1+W) 
					mX = rc.x1+W;
				else
					m_LastMouse = float2(absX, absY);
			}

			//  Setup new value ...
			mX -= (rc.x1 + W);
			W = m_Size.x - W * 2.0f;
			float v = m_Min + (mX / W) * (m_Max - m_Min);
			if ( m_Option & SX_GUI_PROPERTY_AUTOSIZE )
				SetValue( sx::cmn::Round( v ) );
			else
				SetValue( v );
		}

		return res;
	}

	float TrackBar::GetMax( void )
	{
		return m_Max;
	}

	void TrackBar::SetMax( float v )
	{
		if (v<m_Min)
			v = m_Min;

		if (v<m_Value)
			SetValue(v);

		m_Max = v;
	}

	float TrackBar::GetMin( void )
	{
		return m_Min;
	}

	void TrackBar::SetMin( float v )
	{
		if (v>m_Max)
			v = m_Max;

		if (v>m_Value)
			SetValue(v);

		m_Min = v;
	}

	float TrackBar::GetValue( void )
	{
		return m_Value;
	}

	void TrackBar::SetValue( float v )
	{
		m_Value = v;
		if (v>m_Max)
			m_Value = m_Max;

		if (v<m_Min)
			m_Value = m_Min;

		m_OnScroll(this);
	}

	float TrackBar::GetBlendingValue( void )
	{
		return m_ValueScroll;
	}

	void TrackBar::SetOnScroll( PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnScroll.m_pForm = pForm;
		m_OnScroll.m_pFunc = pFunc;
	}

	void TrackBar::Update( float elpsTime )
	{
		Control::Update(elpsTime);

		m_ValueScroll += (m_Value - m_ValueScroll) * 0.15f * (elpsTime * 0.06f);
		
		SEGAN_CLAMP(m_ValueScroll, m_Min, m_Max);
		float W = m_Size.y / 2.0f;
		float P = m_ValueScroll - m_Min;
		float M = m_Max - m_Min; if (M<EPSILON) M=EPSILON;
		m_elements[1]->Matrix()._41 = GetRect().x1 + W + (P / M) * ( m_Size.x - W * 2.0f );
	}

	void TrackBar::Draw( DWORD option )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		Control::Draw(option);
		Control::Draw_Childes(option);
	}

}}	//  namespace sx { namespace gui {