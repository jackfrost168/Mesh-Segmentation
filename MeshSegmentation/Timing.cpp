// Timing.cpp

#include "StdAfx.h"
#include "Timing.h"

float LI2f(const LARGE_INTEGER &li)
{
	float f = float(unsigned(li.u.HighPart) >> 16);  f *= 65536.0f;
	
	f += unsigned(li.u.HighPart) & 0xffff;    f *= 65536.0f;
	f += unsigned(li.u.LowPart) >> 16;        f *= 65536.0f;
	f += unsigned(li.u.LowPart) & 0xffff;
	
	return f;
}

void get_timestamp(timestamp &now)
{
	QueryPerformanceCounter(&now);
}

float operator - (const timestamp &t1, const timestamp &t2)
{
	static LARGE_INTEGER PerformanceFrequency;
	static int status = QueryPerformanceFrequency(&PerformanceFrequency);
	if (status == 0) return 1.0f;

	return (LI2f(t1) - LI2f(t2)) / LI2f(PerformanceFrequency);
}