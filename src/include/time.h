#ifndef __TIME_H__
#define __TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

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
	const char *__tm_zone;
};

clock_t clock(void);
time_t time(time_t * t);
double difftime (time_t, time_t);
time_t mktime(struct tm * tm);
size_t strftime(char * s, size_t max, const char * fmt, const struct tm * t);
struct tm * gmtime(const time_t * t);
struct tm * localtime(const time_t * t);
char * asctime(const struct tm * tm);
char * ctime(const time_t * t);

struct tm * __time_to_tm(time_t t, struct tm * tm);
time_t __tm_to_time(struct tm * tm);

#ifdef __cplusplus
}
#endif

#endif /* __TIME_H__ */
