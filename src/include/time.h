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
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;

	long __tm_gmtoff;
	const char * __tm_zone;
};

struct timeval {
	long tv_sec;
	long tv_usec;
};

clock_t clock(void);
time_t time(time_t * t);
time_t mktime(struct tm * tm);
double difftime (time_t, time_t);
struct tm * gmtime(const time_t * t);
struct tm * localtime(const time_t * t);
char * asctime(const struct tm * tm);
char * ctime(const time_t * t);
size_t strftime(char * s, size_t max, const char * fmt, const struct tm * t);
int gettimeofday(struct timeval * tv, void * tz);

int __secs_to_tm(long long t, struct tm * tm);
long long __tm_to_secs(const struct tm * tm);

#ifdef __cplusplus
}
#endif

#endif /* __TIME_H__ */
