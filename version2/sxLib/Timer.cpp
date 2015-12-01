#include "Timer.h"
#include <windows.h>

Timer::Timer(void): m_time(0), m_elpsTime(0), m_elpsTime_smoothed(0), m_speed(1), m_speedFactor(1), m_lastTime(INFINITY) 
{ 
	Update();
}

void Timer::Update(void)
{
	double tickCount = GetCurrTime() * 0.000001;
	double elpsTime = (tickCount - m_lastTime) * m_speed * m_speedFactor;
	m_lastTime = tickCount;
	if (elpsTime > 0)
	{
		m_time += elpsTime;
		m_elpsTime = elpsTime;
		m_elpsTime_smoothed += (elpsTime - m_elpsTime_smoothed) * 0.1f;		
	}
}

uint64 Timer::GetCurrTime( void )
{
	return sx_time_counter();
}

SEGAN_LIB_API uint64 sx_time_counter(void)
{
#if _WIN32 || _WIN64
	static LARGE_INTEGER ferquency;
	static LARGE_INTEGER counter;
	static BOOL useHighPerformance = QueryPerformanceFrequency(&ferquency);
	if (useHighPerformance && QueryPerformanceCounter(&counter))
		return (1000000 * counter.QuadPart) / ferquency.QuadPart;
	else
		return GetTickCount();
#endif
}
