/*
 * libc/ctype/isupper.c
 */

#include <ctype.h>

int isupper(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _U));
}
