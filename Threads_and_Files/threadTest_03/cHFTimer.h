#ifndef _cHFTimer_HG_
#define _cHFTimer_HG_

#include <Windows.h>

class cHFTimer
{
public:
	cHFTimer();
	double getElapsedTimeMicroSeconds(void);
	double getElapsedTimeMilliSeconds(void);
	double getElapsedTimeSeconds(void);

private:
	LARGE_INTEGER m_TimerFrequency;	// Can't set to 0 (is a union)
	LARGE_INTEGER m_LastSampleTime;	// Can't set to 0 (is a union)
};

#endif // _cHFTimer_HG_
