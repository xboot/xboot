/*
 * lib/libc/abs.c
 */

#include <configs.h>
#include <default.h>
#include <abs.h>


/*
 * compute absolute value of an x_s32.
 */
x_s32 abs_s32(x_s32 val)
{
	return (val < 0 ? -val : val);
}

/*
 * compute absolute value of an x_s64.
 */
x_s64 abs_s64(x_s64 val)
{
	return (val < 0 ? -val : val);
}
