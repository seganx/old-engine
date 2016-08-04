/********************************************************************
	created:	2015/11/22
	filename: 	Timer.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class and functions to handle times
*********************************************************************/
#ifndef SX_TIMER_DEFINED
#define SX_TIMER_DEFINED

#include "Def.h"

//! simple timer to use in application
class SEGAN_LIB_API Timer
{
public:
	Timer( void );

	//! update the timer. this function should be in loop
	void update( void );

	//! return OS timer in high frequency
	uint64 get_curr_time( void );

public:
	double m_time;					//! time from starting the timer in seconds
	double m_elpsTime;				//! elapsed time in the loop in seconds
	double m_elpsTime_smoothed;		//! smoothed elapsed time in the loop in seconds
	double m_speed;					//! speed of time. default is 1
	double m_speedFactor;			//! a simple factor of the speed. default is 1

private:
	double m_lastTime;
};


//////////////////////////////////////////////////////////////////////////
//	HELPER FUNCTION
//////////////////////////////////////////////////////////////////////////

//! return OS timer in high frequency
SEGAN_LIB_API uint64 sx_time_counter( void );

//! return OS time in milliseconds
SEGAN_LIB_API uint sx_time_milliseconds( void );

//! return OS time in seconds
SEGAN_LIB_API uint sx_time_seconds( void );

#endif	//	SX_TIMER_DEFINED