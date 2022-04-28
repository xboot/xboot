/*
 * libc/time/ctime_r.c
 */

#include <time.h>
#include <xboot/module.h>

char * ctime_r(const time_t * t, char * buf)
{
	struct tm tm, * ptm = localtime_r(t, &tm);
	return ptm ? asctime_r(ptm, buf) : NULL;
}
EXPORT_SYMBOL(ctime_r);
