/*
 * libc/ctype/isalpha.c
 */

#include <ctype.h>

/*
 * Checks if a byte is alphabetic
 */
int isalpha(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & (_U|_L)));
}
