#include "cHFTimer.h"
#include <sstream>
#include <iomanip>	// for setprecision()

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
	
std::string cHFTimer::getElapsedTimeAppropriateDurationString(int numDecimalPlaces /*=-1*/)
{
	double elapseTime = this->getElapsedTimeMicroSeconds();

	return this->getElapsedTimeAppropriateDurationString(elapseTime, numDecimalPlaces);
}

std::string cHFTimer::getElapsedTimeAppropriateDurationString(double elapsedTimeMicroSeconds, int numDecimalPlaces /*=-1*/)
{
	std::stringstream ssElapsedTimeString;

	double elapsedTime = elapsedTimeMicroSeconds;

	if (elapsedTime < 1000.0)
	{
		// Is microseconds

		if (numDecimalPlaces >= 0)
		{
			ssElapsedTimeString << std::fixed << std::setprecision(numDecimalPlaces);
		}
		ssElapsedTimeString << elapsedTime << " us (microseconds)";
		return ssElapsedTimeString.str();
	}

	// Greater than miroseconds
	elapsedTime /= 1000.0;	// now milliseconds
	if (elapsedTime < 1000.0)
	{
		// Is milliseconds
		if (numDecimalPlaces >= 0)
		{
			ssElapsedTimeString << std::fixed << std::setprecision(numDecimalPlaces);
		}
		ssElapsedTimeString << elapsedTime << " ms (milliseconds)";
		return ssElapsedTimeString.str();
	}

	// Greater than milliseconds
	elapsedTime /= 1000.0;	// now seconds
	if (elapsedTime < 60.0)
	{
		// Is seconds
		if (numDecimalPlaces >= 0)
		{
			ssElapsedTimeString << std::fixed << std::setprecision(numDecimalPlaces);
		}
		ssElapsedTimeString << elapsedTime << " seconds";
		return ssElapsedTimeString.str();
	}

	// More than one second
	elapsedTime /= 60.0;	// now minutes
	if (elapsedTime < 60.0)
	{
		// Is minutes
		if (numDecimalPlaces >= 0)
		{
			ssElapsedTimeString << std::fixed << std::setprecision(numDecimalPlaces);
		}
		ssElapsedTimeString << elapsedTime << " minutes";
		return ssElapsedTimeString.str();
	}

	// More than one minutes
	elapsedTime /= 60.0;	// now hours
	// Is hours
	if (numDecimalPlaces >= 0)
	{
		ssElapsedTimeString << std::fixed << std::setprecision(numDecimalPlaces);
	}
	ssElapsedTimeString << elapsedTime << " hours";
	return ssElapsedTimeString.str();
}

	
cHFTimer::cHFTimer()
{
	QueryPerformanceFrequency( &(this->m_TimerFrequency) );
	QueryPerformanceCounter( &(this->m_LastSampleTime) );
}	
