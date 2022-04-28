#ifndef __TIME_H__
#define __TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef	uint64_t		clock_t;
typedef	int64_t			time_t;

#define CLOCKS_PER_SEC	(1000000000ULL)

struct tm {
	int tm_sec;			/* second [0 - 59] */
	int tm_min;			/* minute [0 - 59] */
	int tm_hour;		/* hour [0 - 23] */
	int tm_mday;		/* day [1 - 31] */
	int tm_mon;			/* month [0 - 11] */
	int tm_year;		/* year based of 1900 */
	int tm_wday;		/* day of week [0 - 6] */
	int tm_yday;		/* day in year [0 - 365] */
	int tm_isdst;		/* dst [0 / 1] */
};

struct timeval {
	long tv_sec;
	long tv_usec;
};

clock_t clock(void);
time_t time(time_t * t);
time_t timegm(struct tm * tm);
time_t mktime(struct tm * tm);
double difftime (time_t, time_t);
struct tm * gmtime(const time_t * t);
struct tm * gmtime_r(const time_t * t, struct tm * tm);
struct tm * localtime(const time_t * t);
struct tm * localtime_r(const time_t * t, struct tm * tm);
char * asctime(const struct tm * tm);
char * asctime_r(const struct tm * tm, char * buf);
char * ctime(const time_t * t);
char * ctime_r(const time_t * t, char * buf);
size_t strftime(char * s, size_t max, const char * fmt, const struct tm * t);
int gettimeofday(struct timeval * tv, void * tz);
int settimeofday(struct timeval * tv, void * tz);
int timezone(const char * tz);

int __secs_to_tm(long long t, struct tm * tm);
long long __tm_to_secs(const struct tm * tm);

#ifdef __cplusplus
}
#endif

#endif /* __TIME_H__ */
