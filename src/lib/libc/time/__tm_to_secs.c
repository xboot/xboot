/*
 * libc/time/__tm_to_secs.c
 */

#include <time.h>

static long long __year_to_secs(long long year, int * is_leap)
{
	if(year - 2ULL <= 136)
	{
		int y = year;
		int leaps = (y - 68) >> 2;
		if(!((y - 68) & 3))
		{
			leaps--;
			if(is_leap)
				*is_leap = 1;
		}
		else if(is_leap)
		{
			*is_leap = 0;
		}
		return 31536000 * (y - 70) + 86400 * leaps;
	}

	int cycles, centuries, leaps, rem;

	if(!is_leap)
		is_leap = &(int ){ 0 };
	cycles = (year - 100) / 400;
	rem = (year - 100) % 400;
	if(rem < 0)
	{
		cycles--;
		rem += 400;
	}
	if(!rem)
	{
		*is_leap = 1;
		centuries = 0;
		leaps = 0;
	}
	else
	{
		if(rem >= 200)
		{
			if(rem >= 300)
				centuries = 3, rem -= 300;
			else
				centuries = 2, rem -= 200;
		}
		else
		{
			if(rem >= 100)
				centuries = 1, rem -= 100;
			else
				centuries = 0;
		}
		if(!rem)
		{
			*is_leap = 0;
			leaps = 0;
		}
		else
		{
			leaps = rem / 4U;
			rem %= 4U;
			*is_leap = !rem;
		}
	}

	leaps += 97 * cycles + 24 * centuries - *is_leap;

	return (year - 100) * 31536000LL + leaps * 86400LL + 946684800 + 86400;
}

static int __month_to_secs(int month, int is_leap)
{
	static const int secs_through_month[] = { 0, 31 * 86400, 59 * 86400, 90 * 86400, 120 * 86400,
		151 * 86400, 181 * 86400, 212 * 86400, 243 * 86400, 273 * 86400, 304 * 86400, 334 * 86400 };
	int t = secs_through_month[month];
	if(is_leap && month >= 2)
		t += 86400;
	return t;
}

long long __tm_to_secs(const struct tm * tm)
{
	int is_leap;
	long long year = tm->tm_year;
	int month = tm->tm_mon;
	if(month >= 12 || month < 0)
	{
		int adj = month / 12;
		month %= 12;
		if(month < 0)
		{
			adj--;
			month += 12;
		}
		year += adj;
	}
	long long t = __year_to_secs(year, &is_leap);
	t += __month_to_secs(month, is_leap);
	t += 86400LL * (tm->tm_mday - 1);
	t += 3600LL * tm->tm_hour;
	t += 60LL * tm->tm_min;
	t += tm->tm_sec;
	return t;
}
