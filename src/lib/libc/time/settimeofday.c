/*
 * libc/time/settimeofday.c
 */

#include <time.h>
#include <rtc/rtc.h>
#include <clocksource/clocksource.h>
#include <xboot/module.h>

extern int64_t time_of_day_adjust;

#define LEAPS_THRU_END(y)	((y) / 4 - (y) / 100 + (y) / 400)
#define LEAP_YEAR(year)		((!(year % 4) && (year % 100)) || !(year % 400))

static int rtc_month_days(int year, int month)
{
	const unsigned char rtc_days_in_month[13] = {
		0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	return rtc_days_in_month[month] + ((LEAP_YEAR(year) && (month == 2)) ? 1 : 0);
}

static void secs_to_rtc_time(uint64_t time, struct rtc_time_t * rt)
{
	int year;
	int month;
	int days;
	int newdays;

	days = time / 86400;
	time -= (uint64_t)days * 86400;

	rt->week = (days + 4) % 7;
	year = 1970 + days / 365;
	days -= (year - 1970) * 365	+ LEAPS_THRU_END(year - 1) - LEAPS_THRU_END(1970 - 1);

	if(days < 0)
	{
		year -= 1;
		days += 365 + LEAP_YEAR(year);
	}
	rt->year = year;
	rt->day = days + 1;

	for(month = 1; month < 12; month++)
	{
		newdays = days - rtc_month_days(year, month);
		if(newdays < 0)
			break;
		days = newdays;
	}
	rt->month = month;
	rt->day = days + 1;
	rt->hour = time / 3600;
	time -= rt->hour * 3600;
	rt->minute = time / 60;
	rt->second = time - rt->minute * 60;
}

int settimeofday(struct timeval * tv, void * tz)
{
	if(!tv)
		return -1;
	int64_t ns = tv->tv_sec * 1000000000ULL + tv->tv_usec * 1000;
	time_of_day_adjust = ns - ktime_to_ns(ktime_get());
	struct rtc_t * rtc = search_first_rtc();
	if(rtc)
	{
		struct rtc_time_t t;
		secs_to_rtc_time((uint64_t)tv->tv_sec, &t);
		rtc_settime(rtc, &t);
	}
	return 0;
}
EXPORT_SYMBOL(settimeofday);
