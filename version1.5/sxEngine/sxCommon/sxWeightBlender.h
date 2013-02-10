/********************************************************************
	created:	2011/01/22
	filename: 	sxWeightBlender.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class to blend weight coefficient
				This class use two parameters velocity and amplitude to blend weight
*********************************************************************/
#ifndef GUARD_sxWeightBlender_HEADER_FILE
#define GUARD_sxWeightBlender_HEADER_FILE

#include "sxCommon_def.h"

namespace sx { namespace cmn { 

	class SEGAN_API WeightBlender
	{
	public:
		WeightBlender(void);

		void Reset();

		float& Velocity(void);
		float& Amplitude(void);

		void  SetWeight(float _w);

		float GetWeight(float elpsTime);

	private:
		float m_Vel;
		float m_Amp;
		float w, a, v;
	};

} } // namespace sx { namespace cmn { 

#endif	//	GUARD_sxWeightBlender_HEADER_FILE