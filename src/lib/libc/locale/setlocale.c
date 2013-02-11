/*
 * libc/locale/setlocale.c
 */

#include <xboot/module.h>
#include <locale.h>

char * setlocale(int category, const char * locale)
{
	return "C.UTF-8";
}
EXPORT_SYMBOL(setlocale);
