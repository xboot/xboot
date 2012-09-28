/*
 * libc/ctype/isprint.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a printing byte
 */
int isprint(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & (_P|_U|_L|_N|_B)));
}
EXPORT_SYMBOL(isprint);
