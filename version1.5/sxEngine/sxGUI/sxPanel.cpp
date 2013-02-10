#include "sxPanel.h"

namespace sx { namespace gui {


	Panel::Panel( void ): Control()
	{
		m_Type = GUI_PANEL;

		//  create one element
		SetElementCount(1);

		SetSize(float2(100.0f, 80.0f));
	}

	Panel::~Panel( void )
	{

	}

	void Panel::Save( Stream& S )
	{
		Control::Save(S);
		Save_Childes(S);
	}

	void Panel::Load( Stream& S )
	{
		Control::Load(S);
		Load_Childes(S);
	}

	void Panel::SetSize( float2 S )
	{
		Control::SetSize(S);
		RectF rc = Control::GetRect();
		m_Elements[0]->SetRect(rc);
	}

	void Panel::Draw( DWORD option )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		Control::Draw(option);
		Control::Draw_Childes(option);
	}

}}	//  namespace sx { namespace gui {