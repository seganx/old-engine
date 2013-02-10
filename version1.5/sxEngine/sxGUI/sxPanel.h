/********************************************************************
	created:	2010/11/26
	filename: 	sxPanel.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the Panel Control. this object has
				only one element as background
*********************************************************************/
#ifndef GUARD_sxPanel_HEADER_FILE
#define GUARD_sxPanel_HEADER_FILE

#include "sxControl.h"

namespace sx { namespace gui {

	//! the Panel Control. this object has only one element as background
	class SEGAN_API Panel: public Control
	{
		SEGAN_STERILE_CLASS(Panel);

	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();

	public:
		Panel(void);
		virtual ~Panel(void);

		void Save(Stream& S);
		void Load(Stream& S);

		void SetSize(float2 S);

		void Draw(DWORD option);
	};
	typedef Panel *PPanel;

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxPanel_HEADER_FILE