/*
 * libc/locale/setlocale.c
 */

#include <locale.h>
#include <xboot/module.h>

char * setlocale(int category, const char * locale)
{
	return "C.UTF-8";
}
EXPORT_SYMBOL(setlocale);
