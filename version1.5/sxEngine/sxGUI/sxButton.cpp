#include "sxButton.h"


namespace sx { namespace gui {

	Button::Button( void ): Control(), m_Alpha(0.0f), m_Over(-1)
	{
		m_Type = GUI_BUTTON;

		//  Create 3 element for button
		SetElementCount(3);

		SetSize( float2(100.0f, 75.0f) );

		AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	}

	Button::~Button( void )
	{
		
	}

	void Button::Save( Stream& S )
	{
		Control::Save(S);
		Save_Childes(S);
	}

	void Button::Load( Stream& S )
	{
		Control::Load(S);
		Load_Childes(S);
	}

	void Button::SetSize( float2 S )
	{
		Control::SetSize(S);
		RectF rc = GetRect();

		m_Elements[0]->SetRect(rc);

		m_Elements[1]->SetRect(rc);
		//m_Elements[1]->Matrix()._43 = SX_GUI_Z_BIAS;

		m_Elements[2]->SetRect(rc);
		//m_Elements[2]->Matrix()._43 = SX_GUI_Z_BIAS;
	}

	int Button::MouseOver( float absX, float absY )
	{
		m_Over = Control::MouseOver(absX, absY);
		return m_Over;
	}

	void Button::Update( float elpsTime )
	{
		Control::Update(elpsTime);

		//  Move is over on button
		m_Alpha += (0.18f*(float)(m_Over==2) - 0.09f) * (elpsTime * 0.06f);
		if (m_Alpha>1)	m_Alpha = 1.0f;
		if (m_Alpha<0)	m_Alpha = 0.0f;
	}

	void Button::Draw( DWORD option )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		float b0 = m_Elements[0]->Color().a;
		float b1 = m_Elements[1]->Color().a;
		float b2 = m_Elements[2]->Color().a;

		m_Elements[0]->Color().a = (1.0f - m_Alpha) * b0;
		m_Elements[1]->Color().a = m_Alpha * b1;
		m_Elements[2]->Color().a = 0.0f;

		if (m_SelectedElement==2) //  Clicked ....
		{
			m_Elements[0]->Color().a = 0.0f;
			m_Elements[1]->Color().a = 0.0f;
			m_Elements[2]->Color().a = b2;
		};

		Control::Draw(option);

		m_Elements[0]->Color().a = b0;
		m_Elements[1]->Color().a = b1;
		m_Elements[2]->Color().a = b2;

		Control::Draw_Childes(option);
	}

}}	//	namespace sx { namespace gui {

