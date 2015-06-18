#pragma once

#include "EnginePCH.h"

class HRTimer
{
public:
	HRTimer() : 
		frequency(GetFrequency())
	{
	}

	LONGLONG GetFrequency()
	{
		LARGE_INTEGER proc_freq;
		QueryPerformanceFrequency(&proc_freq);

		return proc_freq.QuadPart;
	}

	void Reset()
	{
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);

		QueryPerformanceCounter(&start);
		SetThreadAffinityMask(GetCurrentThread(), oldmask);
	}

	// get elapsed from Reset() in ms
	double GetElapsed()
	{
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);

		QueryPerformanceCounter(&stop);
		SetThreadAffinityMask(GetCurrentThread(), oldmask);

		return ((stop.QuadPart - start.QuadPart) / (double)frequency) * 1000.0;
	}
private:
	LARGE_INTEGER start;
	LARGE_INTEGER stop;
	LONGLONG frequency;
};