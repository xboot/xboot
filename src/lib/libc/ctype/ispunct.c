/*
 * libc/ctype/ispunct.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a punctuation byte
 */
int ispunct(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _P));
}
EXPORT_SYMBOL(ispunct);
