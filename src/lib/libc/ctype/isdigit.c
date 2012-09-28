/*
 * libc/ctype/isdigit.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a digit
 */
int isdigit(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _N));
}
EXPORT_SYMBOL(isdigit);
