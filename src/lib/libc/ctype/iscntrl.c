/*
 * libc/ctype/iscntrl.c
 */

#include <ctype.h>

int iscntrl(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _C));
}
