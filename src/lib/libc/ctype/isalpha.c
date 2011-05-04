/*
 * libc/ctype/isalpha.c
 */

#include <ctype.h>

int isalpha(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & (_U|_L)));
}
