#include "cHFTimer.h"

double cHFTimer::getElapsedTimeMicroSeconds(void)
{
	// https://learn.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps
	LARGE_INTEGER currentSampleTime;	// Can't set to 0 (is a union)
	QueryPerformanceCounter( &currentSampleTime );
	
	LARGE_INTEGER elapsedTicks;
	elapsedTicks.QuadPart = currentSampleTime.QuadPart - this->m_LastSampleTime.QuadPart;
	
	this->m_LastSampleTime = currentSampleTime;
	
	double elapsed_ms = (double)(elapsedTicks.QuadPart) * 1000000;
	elapsed_ms /= (double)(m_TimerFrequency.QuadPart);
	
	return elapsed_ms;
}

double cHFTimer::getElapsedTimeMilliSeconds(void)
{
	double elapsedMicroSeconds = this->getElapsedTimeMicroSeconds();
	
	double elapsed = (double)elapsedMicroSeconds;
	elapsed /= 1000.0;
	
	return elapsed;
}


double cHFTimer::getElapsedTimeSeconds(void)
{
	double elapsedTime = this->getElapsedTimeMilliSeconds();
	
	return (double)elapsedTime/1000.0;
}
	
	
cHFTimer::cHFTimer()
{
	QueryPerformanceFrequency( &(this->m_TimerFrequency) );
	QueryPerformanceCounter( &(this->m_LastSampleTime) );
}	
