/*
 * libc/time/ctime.c
 */

#include <time.h>
#include <xboot/module.h>

char * ctime(const time_t * t)
{
	struct tm * tm = localtime(t);
	if(!tm)
		return NULL;
	return asctime(tm);
}
EXPORT_SYMBOL(ctime);
