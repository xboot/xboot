/*
 * libc/ctype/isdigit.c
 */

#include <ctype.h>

int isdigit(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _N));
}
