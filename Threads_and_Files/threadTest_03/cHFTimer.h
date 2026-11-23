#ifndef _cHFTimer_HG_
#define _cHFTimer_HG_

#include <Windows.h>
#include <string>

class cHFTimer
{
public:
	cHFTimer();
	double getElapsedTimeMicroSeconds(void);
	double getElapsedTimeMilliSeconds(void);
	double getElapsedTimeSeconds(void);

	// If numDecimalPlaces < 0, then doesn't limit them
	std::string getElapsedTimeAppropriateDurationString(int numDecimalPlaces = -1);
	std::string getElapsedTimeAppropriateDurationString(double elapsedTimeMicroSeconds, int numDecimalPlaces = -1);

private:
	LARGE_INTEGER m_TimerFrequency;	// Can't set to 0 (is a union)
	LARGE_INTEGER m_LastSampleTime;	// Can't set to 0 (is a union)
};

#endif // _cHFTimer_HG_
