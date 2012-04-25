/*
 * libc/locale/locale.c
 */

#include <locale.h>

char * setlocale(int category, const char * locale)
{
	/* no support */
	return 0;
}

struct lconv * localeconv(void)
{
	/* no support */
	return 0;
}
