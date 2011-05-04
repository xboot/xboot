/*
 * libc/ctype/isgraph.c
 */

#include <ctype.h>

int isgraph(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & (_P|_U|_L|_N)));
}
