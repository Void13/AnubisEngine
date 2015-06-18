#include "EnginePCH.h"

#include "HRTimer.h"

//--------------------------------------------------------------------------------------
CHRTimer::CHRTimer()
{
	m_bTimerStopped = true;
	m_llQPFTicksPerSec = 0;

	m_llStopTime = 0;
	m_llLastElapsedTime = 0;
	m_llBaseTime = 0;

	// Use QueryPerformanceFrequency to get the frequency of the counter
	LARGE_INTEGER qwTicksPerSec = { 0 };
	QueryPerformanceFrequency(&qwTicksPerSec);
	m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
}


//--------------------------------------------------------------------------------------
void CHRTimer::Reset()
{
	LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

	m_llBaseTime = qwTime.QuadPart;
	m_llLastElapsedTime = qwTime.QuadPart;
	m_llStopTime = 0;
	m_bTimerStopped = FALSE;
}


//--------------------------------------------------------------------------------------
void CHRTimer::Start()
{
	// Get the current time
	LARGE_INTEGER qwTime = { 0 };
	QueryPerformanceCounter(&qwTime);

	if (m_bTimerStopped)
		m_llBaseTime += qwTime.QuadPart - m_llStopTime;
	m_llStopTime = 0;
	m_llLastElapsedTime = qwTime.QuadPart;
	m_bTimerStopped = FALSE;
}


//--------------------------------------------------------------------------------------
void CHRTimer::Stop()
{
	if (!m_bTimerStopped)
	{
		LARGE_INTEGER qwTime = { 0 };
		QueryPerformanceCounter(&qwTime);
		m_llStopTime = qwTime.QuadPart;
		m_llLastElapsedTime = qwTime.QuadPart;
		m_bTimerStopped = TRUE;
	}
}


//--------------------------------------------------------------------------------------
void CHRTimer::Advance()
{
	m_llStopTime += m_llQPFTicksPerSec / 10;
}


//--------------------------------------------------------------------------------------
// in ms
double CHRTimer::GetAbsoluteTime()
{
	LARGE_INTEGER qwTime = { 0 };
	QueryPerformanceCounter(&qwTime);

	double fTime = qwTime.QuadPart / (double)m_llQPFTicksPerSec;

	return fTime * 1000.0;
}


//--------------------------------------------------------------------------------------
// in ms
double CHRTimer::GetTime()
{
	LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

	double fAppTime = (double)(qwTime.QuadPart - m_llBaseTime) / (double)m_llQPFTicksPerSec;

	return fAppTime * 1000.0;
}

//--------------------------------------------------------------------------------------
// in ms
double CHRTimer::GetElapsedTime()
{
	LARGE_INTEGER qwTime = GetAdjustedCurrentTime();

	double fElapsedTime = (float)((double)(qwTime.QuadPart - m_llLastElapsedTime) / (double)
		m_llQPFTicksPerSec);
	m_llLastElapsedTime = qwTime.QuadPart;

	// See the explanation about clamping in CDXUTTimer::GetTimeValues()
	if (fElapsedTime < 0.0f)
		fElapsedTime = 0.0f;

	return fElapsedTime * 1000.0;
}


//--------------------------------------------------------------------------------------
// If stopped, returns time when stopped otherwise returns current time
//--------------------------------------------------------------------------------------
LARGE_INTEGER CHRTimer::GetAdjustedCurrentTime()
{
	LARGE_INTEGER qwTime;
	if (m_llStopTime != 0)
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter(&qwTime);
	return qwTime;
}

//--------------------------------------------------------------------------------------
bool CHRTimer::IsStopped()
{
	return m_bTimerStopped;
}

//--------------------------------------------------------------------------------------
// Limit the current thread to one processor (the current one). This ensures that timing code 
// runs on only one processor, and will not suffer any ill effects from power management.
// See "Game Timing and Multicore Processors" for more details
//--------------------------------------------------------------------------------------
void CHRTimer::LimitThreadAffinityToCurrentProc()
{
	HANDLE hCurrentProcess = GetCurrentProcess();

	// Get the processor affinity mask for this process
	DWORD_PTR dwProcessAffinityMask = 0;
	DWORD_PTR dwSystemAffinityMask = 0;

	if (GetProcessAffinityMask(hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask) != 0 &&
		dwProcessAffinityMask)
	{
		// Find the lowest processor that our process is allows to run against
		DWORD_PTR dwAffinityMask = (dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1));

		// Set this as the processor that our thread must always run against
		// This must be a subset of the process affinity mask
		HANDLE hCurrentThread = GetCurrentThread();
		if (INVALID_HANDLE_VALUE != hCurrentThread)
		{
			SetThreadAffinityMask(hCurrentThread, dwAffinityMask);
			CloseHandle(hCurrentThread);
		}
	}

	CloseHandle(hCurrentProcess);
}