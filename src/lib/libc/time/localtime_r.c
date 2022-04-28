/*
 * libc/time/localtime_r.c
 */

#include <time.h>
#include <xboot/module.h>

struct tm * localtime_r(const time_t * t, struct tm * tm)
{
	return gmtime_r(t, tm);
}
EXPORT_SYMBOL(localtime_r);
