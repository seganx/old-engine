#include "sxCheckBox.h"


namespace sx { namespace gui {

	CheckBox::CheckBox( void ): Control(), m_Checked(false)
	{
		m_Type = GUI_CHECKBOX;

		//  Create 2 element for button
		SetElementCount(2);

		SetSize( float2(75.0f, 50.0f) );

		AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	}

	CheckBox::~CheckBox( void )
	{
		
	}

	void CheckBox::Save( Stream& S )
	{
		Control::Save(S);

		SEGAN_STREAM_WRITE(S, m_Checked);

		Save_Childes(S);
	}

	void CheckBox::Load( Stream& S )
	{
		Control::Load(S);

		SEGAN_STREAM_READ(S, m_Checked);

		Load_Childes(S);
	}

	void CheckBox::SetSize( float2 S )
	{
		Control::SetSize(S);
		RectF rc = GetRect();

		m_Elements[0]->SetRect(rc);

		m_Elements[1]->SetRect(rc);
		//m_Elements[1]->Matrix()._43 = SX_GUI_Z_BIAS;
	}

	int CheckBox::MouseUp( float absX, float absY )
	{
		int result = m_SelectedElement;
		
		if (result != -1 && MouseOver(absX, absY)>-1)
		{
			m_Checked = !m_Checked;
			m_OnClick(this);
		}

		m_SelectedElement = -1;
		return result;
	}

	void CheckBox::Draw( DWORD option )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		if (!m_Checked)
			m_Elements[1]->Matrix().Scale(SX_GUI_MINIMUM_SCALE, SX_GUI_MINIMUM_SCALE, 1.0f);
		else
			m_Elements[1]->Matrix().Scale(1.0f, 1.0f, 1.0f);

		Control::Draw(option);

		Control::Draw_Childes(option);
	}

	FORCEINLINE bool& CheckBox::Checked( void )
	{
		return m_Checked;
	}

}}	//	namespace sx { namespace gui {

