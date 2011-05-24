#ifndef __TIME_H__
#define __TIME_H__

struct tm {
	int tm_sec;				/* Seconds		[0-60] */
	int tm_min;				/* Minutes		[0-59] */
	int tm_hour;			/* Hours		[0-23] */
	int tm_mday;			/* Day			[1-31] */
	int tm_mon;				/* Month		[0-11] */
	int tm_year;			/* Year - 1900 */
	int tm_wday;			/* Day of week	[0-6] */
	int tm_yday;			/* Days in year [0-365]	*/
	int tm_isdst;			/* DST			[-1/0/1]*/

	long int tm_gmtoff;		/* Seconds east of UTC */
	const char * tm_zone;	/* Time zone abbreviation */
};

size_t strftime(char * s, size_t max, const char * fmt, const struct tm * t);

#endif /* __TIME_H__ */
