/********************************************************************
	created:	2011/06/25
	filename: 	ColorBox.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple color box include four 
				scroll bar as each color element.
*********************************************************************/
#ifndef GUARD_ColorBox_HEADER_FILE
#define GUARD_ColorBox_HEADER_FILE

#include "BaseForm.h"

//  use this as a simple color box
class Colorbox : public BaseForm
{

public:
	Colorbox(void);
	virtual ~Colorbox(void);

	//! update should be call after process input
	virtual void Update(float elpsTime);

	//! show the color box
	virtual void Show(float x, float y, PD3DColor color);

	//! close the form
	virtual void Close(void);

private:

	void OnScrollColor(sx::gui::PControl Sender);
	void OnEditText(sx::gui::PControl Sender);

	PD3DColor			m_pColor;			//! pointer to selected color
	sx::gui::PTrackBar	m_tr_color[4];		//! four track bar as for color elements
	sx::gui::TextEdit*	m_ed_color[4];		//! four edit text box as for color elements

	//  additional parameters
	float	m_fShowTime;
};
typedef Colorbox *PColorbox;

#endif	//	GUARD_ColorBox_HEADER_FILE
