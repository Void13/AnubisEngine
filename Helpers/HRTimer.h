#ifndef _H__HRTIMER
#define _H__HRTIMER

#include <Windows.h>
#include <assert.h>

class CHRTimer
{
public:
	CHRTimer();

	void			Reset();			// resets the timer
	void			Start();			// starts the timer
	void			Stop();				// stop (or pause) the timer
	void			Advance();			// advance the timer by 0.1 seconds

	// in seconds
	double			GetAbsoluteTime();	// get the absolute system time
	// in seconds
	double			GetTime();			// get the current time
	// in seconds
	double			GetElapsedTime();	// get the time that elapsed between Get*ElapsedTime() calls
	bool			IsStopped();		// returns true if timer stopped

	// Limit the current thread to one processor (the current one). This ensures that timing code runs
	// on only one processor, and will not suffer any ill effects from power management.
	void			LimitThreadAffinityToCurrentProc();

protected:
	LARGE_INTEGER	GetAdjustedCurrentTime();

	bool		m_bUsingQPF;
	bool		m_bTimerStopped;
	LONGLONG	m_llQPFTicksPerSec;

	LONGLONG	m_llStopTime;
	LONGLONG	m_llLastElapsedTime;
	LONGLONG	m_llBaseTime;
};

#endif