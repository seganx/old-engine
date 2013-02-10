/********************************************************************
	created:	2010/12/05
	filename: 	sxProgressBar.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the ProgressBar control which has 2
				element. one for back ground and the other for show the
				progress in two mode: Linear and Circle.
*********************************************************************/
#ifndef GUARD_sxProgressBar_HEADER_FILE
#define GUARD_sxProgressBar_HEADER_FILE

#include "sxControl.h"

namespace sx { namespace gui {

	/*!
	the ProgressBar Control. this object has two elements for background and 
	the other for show the progress in two mode: Linear and Circle.
	*/	
	class SEGAN_API ProgressBar: public Control
	{
		SEGAN_STERILE_CLASS(ProgressBar);

	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();

	public:
		ProgressBar(void);
		virtual ~ProgressBar(void);

		void Save( Stream& stream );
		void Load( Stream& stream );

		void SetSize(float2 S);

		float GetMax(void);
		void SetMax(float v);

		float GetValue(void);
		void SetValue(float v);

		void Update(float elpsTime);
		void Draw(DWORD option);

	private:
		void BurnVB(void);
		void DrawProgress(DWORD option);

		bool	m_CircleMode;
		float	m_Max, m_Value, m_ValueScroll;

		// we need additional vertex buffer to create circular progress
		PDirect3DVertexBuffer		m_VB0;		//  hold the position vertex buffer
		PDirect3DVertexBuffer		m_VB1;		//  hold the other vertex buffer information
	};
	typedef ProgressBar *PProgressBar;

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxProgressBar_HEADER_FILE
