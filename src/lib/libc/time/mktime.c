/*
 * libc/time/mktime.c
 */

#include <time.h>
#include <xboot/module.h>

time_t mktime(struct tm * tm)
{
	return __tm_to_secs(tm);
}
EXPORT_SYMBOL(mktime);
