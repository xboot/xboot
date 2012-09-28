/*
 * libc/ctype/isxdigit.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a hex adecimal byte
 */
int isxdigit(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & (_N|_X)));
}
EXPORT_SYMBOL(isxdigit);
