/*
 * libc/ctype/isspace.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a space byte
 */
int isspace(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _S));
}
EXPORT_SYMBOL(isspace);
