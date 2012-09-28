/*
 * libc/ctype/isupper.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is an upper case byte
 */
int isupper(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _U));
}
EXPORT_SYMBOL(isupper);
