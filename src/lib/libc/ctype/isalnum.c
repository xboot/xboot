/*
 * libc/ctype/isalnum.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is alphanumeric
 */
int isalnum(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & (_U|_L|_N)));
}
EXPORT_SYMBOL(isalnum);
