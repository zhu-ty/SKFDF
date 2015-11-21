//TimeLogger
//Log Time.
//Author:ShadowK
//email:zhu.shadowk@gmail.com
//2015.11.15
//Use Ctrl+M,Ctrl+O to fold the code.
#ifndef TIME_LOGGER
#define TIME_LOGGER
#pragma once
#pragma warning (disable:4018)

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <Windows.h>

class TimeLogger
{
private:
	long gettime()
	{
		//clock_t clockaaa = clock();
		//long time_elapsed = long(clockaaa);
		//return time_elapsed;
		LARGE_INTEGER re;
		QueryPerformanceCounter(&re);
		return (long)re.QuadPart;
	}
	long long Frequency()
	{
		LARGE_INTEGER re;
		QueryPerformanceFrequency(&re);
		return re.QuadPart;
	}
	std::vector<unsigned int> timeelapsed;
	std::vector<long> start;
public:

	void restart(int n)
	{
		if (n < timeelapsed.size())
			timeelapsed[n] = 0;
	}
	void starttimer(int n)
	{
		if (n >= start.size())
			start.push_back(gettime());
		else
			start[n] = gettime();
		return;
	}
	void recordtimer(int n)
	{
		if (n >= timeelapsed.size())
		{
			if (n < start.size())
				timeelapsed.push_back(gettime() - start[n]);
		}
		else
			timeelapsed[n] += (gettime() - start[n]);
	}

	void s(int n)
	{
		return starttimer(n);
	}
	void r(int n)
	{
		return recordtimer(n);
	}
	double get_n_elapsed(int n)
	{
		if (n < timeelapsed.size())
			return 1.0*1e3*(timeelapsed[n]) / Frequency();
		return -1;
	}
	int get_num()
	{
		return timeelapsed.size();
	}
};
#pragma warning (default:4018)
#endif