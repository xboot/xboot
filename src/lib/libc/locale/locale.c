/*
 * libc/locale/locale.c
 */

#include <xboot/module.h>
#include <locale.h>

char * setlocale(int category, const char * locale)
{
	/* no support */
	return 0;
}
EXPORT_SYMBOL(setlocale);

struct lconv * localeconv(void)
{
	/* no support */
	return 0;
}
EXPORT_SYMBOL(localeconv);
