/********************************************************************
	created:	2010/11/29
	filename: 	sxCheckBox.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the CheckBox Control. this control has
				two elements for usual style and checked
*********************************************************************/
#ifndef GUARD_sxCheckBox_HEADER_FILE
#define GUARD_sxCheckBox_HEADER_FILE

#include "sxControl.h"

namespace sx { namespace gui {

	//! Button Control. this control has three elements for usual style, move over and click.
	class SEGAN_API CheckBox: public Control
	{
		SEGAN_STERILE_CLASS(CheckBox);
		SEGAN_GUI_CLASS_IMPLEMENT();

	public:
		CheckBox(void);
		virtual ~CheckBox(void);

		void Save(Stream& S);
		void Load(Stream& S);

		void SetSize(float2 S);

		int MouseUp(float absX, float absY);

		void Draw(DWORD option);

		//! reference to the boolean checked
		bool& Checked(void);

	private:
		bool	m_Checked;

	};
	typedef CheckBox *PCheckBox;

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxCheckBox_HEADER_FILE