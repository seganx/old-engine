/********************************************************************
	created:	2010/12/02
	filename: 	sxTrackBar.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the TrackBar control which has 2
				element. one for back ground and the other for scroll.
*********************************************************************/
#ifndef GUARD_sxTrackBar_HEADER_FILE
#define GUARD_sxTrackBar_HEADER_FILE

#include "sxControl.h"

namespace sx { namespace gui {

	//! the TrackBar Control. this object has two elements for background and scroll
	class SEGAN_API TrackBar: public Control
	{
		SEGAN_STERILE_CLASS(TrackBar);

	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();
	
	public:
		TrackBar(void);
		virtual ~TrackBar(void);

		void Save( Stream& stream );
		void Load( Stream& stream );

		void SetSize(float2 S);

	protected:
		int MouseOver(float absX, float absY);
		int MouseDown(float absX, float absY);
	public:
		float GetMax(void);
		void SetMax(float v);

		float GetMin(void);
		void SetMin(float v);

		float GetValue(void);
		void SetValue(float v);

		float GetBlendingValue(void);

		void SetOnScroll(PForm pForm, GUICallbackEvent pFunc);

		void Update(float elpsTime);
		void Draw(DWORD option);

	private:
		float m_Min, m_Max, m_Value, m_ValueScroll;
		float2 m_LastMouse;

		GUICallbackFuncPtr	m_OnScroll;

	};
	typedef TrackBar *PTrackBar;

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxTrackBar_HEADER_FILE
