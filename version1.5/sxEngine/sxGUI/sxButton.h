/********************************************************************
	created:	2010/11/28
	filename: 	sxButton.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the Button Control. this control has
				three elements for usual style, move over and click.
*********************************************************************/
#ifndef GUARD_sxButton_HEADER_FILE
#define GUARD_sxButton_HEADER_FILE

#include "sxControl.h"

namespace sx { namespace gui {

	//! Button Control. this control has three elements for usual style, move over and click.
	class SEGAN_API Button: public Control
	{
		SEGAN_STERILE_CLASS(Button);

	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();

	public:
		Button(void);
		virtual ~Button(void);

		void Save(Stream& S);
		void Load(Stream& S);

		void SetSize(float2 S);

	protected:
		int MouseOver(float absX, float absY);

	public:
		void Update(float elpsTime);
		void Draw(DWORD option);

	private:
		float	m_Alpha;
		int		m_Over;

	};
	typedef Button *PButton;

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxButton_HEADER_FILE