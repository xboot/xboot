/*
 * libc/ctype/isgraph.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a graphical byte
 */
int isgraph(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & (_P|_U|_L|_N)));
}
EXPORT_SYMBOL(isgraph);
