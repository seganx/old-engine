#include "sxTypesGUI.h"

#include "sxPanel.h"
#include "sxButton.h"
#include "sxCheckBox.h"
#include "sxTrackBar.h"
#include "sxProgressBar.h"
#include "sxFont.h"
#include "sxLabel.h"
#include "sxEditBox.h"
#include "sxPanelEx.h"

namespace sx { namespace gui {

	//////////////////////////////////////////////////////////////////////////
	//	GLOBAL VARIABLES : INTERNAL CLASS
	//////////////////////////////////////////////////////////////////////////
	class Globals_internal
	{
	public:
		static float		Golden_FOV;
		static float		Draw_Line_Offset;
		static D3DColor		Draw_Line_Color;
		static bool			Editor_Mode;
	};
	float		Globals_internal::Golden_FOV			= PI / 3.271848798f; //0.96018882520187695986024699643822
	float		Globals_internal::Draw_Line_Offset		= 5.0f;
	D3DColor	Globals_internal::Draw_Line_Color		= D3DColor(1, 0, 0, 1);
	bool		Globals_internal::Editor_Mode			= false;

	float Globals::Golden_FOV( void )
	{
		return Globals_internal::Golden_FOV;
	}

	float Globals::Golden_Z( void )
	{
		return - Globals_internal::Golden_FOV * (float)d3d::Device3D::Viewport()->Height;
	}

	float& Globals::Draw_Line_Offset()
	{
		return Globals_internal::Draw_Line_Offset;
	}

	D3DColor& Globals::Draw_Line_Color()
	{
		return Globals_internal::Draw_Line_Color;
	}

	bool& Globals::Editor_Mode( void )
	{
		return Globals_internal::Editor_Mode;
	}

	PControl Create( GUIControlType guiType )
	{
		switch (guiType)
		{
		case GUI_PANEL:			return sx_new( Panel );
		case GUI_BUTTON:		return sx_new( Button );
		case GUI_CHECKBOX:		return sx_new( CheckBox );
		case GUI_TRACKBAR:		return sx_new( TrackBar );
		case GUI_PROGRESSBAR:	return sx_new( ProgressBar );
		case GUI_LABEL:			return sx_new( Label );
		case GUI_TEXTEDIT:		return sx_new( TextEdit );
		case GUI_PANELEX:		return sx_new( PanelEx );
		}

		return NULL;
	}

	void Destroy( PControl gui )
	{
		sx_delete(gui);
	}

}} // namespace sx { namespace gui {


//! object structure used to assign gui member functions
GUICallbackFuncPtr::GUICallbackFuncPtr(): m_pForm(NULL), m_pFunc(NULL){}
GUICallbackFuncPtr::GUICallbackFuncPtr( sx::gui::PForm pForm, GUICallbackEvent pFunc ): m_pForm(pForm), m_pFunc(pFunc){}
void GUICallbackFuncPtr::operator()( sx::gui::PControl pSender )
{
	if (m_pForm && m_pFunc)
		(m_pForm->*m_pFunc)(pSender);
}