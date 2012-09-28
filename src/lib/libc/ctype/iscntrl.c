/*
 * libc/ctype/iscntrl.c
 */

#include <xboot/module.h>
#include <ctype.h>

/*
 * Checks if a byte is a control byte
 */
int iscntrl(int c)
{
	return (c == -1 ? 0 : (((const char *)__const_ctype + 1)[(unsigned char)c] & _C));
}
EXPORT_SYMBOL(iscntrl);
