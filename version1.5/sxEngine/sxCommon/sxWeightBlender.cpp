#include "sxWeightBlender.h"

namespace sx { namespace cmn { 


	WeightBlender::WeightBlender( void ): m_Vel(0.02f), m_Amp(0.73f), w(0), v(0), a(0)
	{

	}

	void WeightBlender::Reset()
	{
		w=0; v=0; a=0;
	}

	float& WeightBlender::Velocity( void )
	{
		return m_Vel;
	}

	float& WeightBlender::Amplitude( void )
	{
		return m_Amp;
	}

	void WeightBlender::SetWeight( float _w )
	{
		w = _w;
	}

	float WeightBlender::GetWeight( float elpsTime )
	{
		a = (1 - w) * m_Vel;
		
		if ( abs(a) > 0.00001 || m_Amp >= 1.0f )
		{
			v += a * (elpsTime * 0.06f);
			w = (w + v) * m_Amp;
		}
		else w = 1;

		return w;
	}


} } // namespace sx { namespace cmn { 