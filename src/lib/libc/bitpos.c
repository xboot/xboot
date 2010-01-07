/*
 * lib/libc/bitpos.c
 */

#include <configs.h>
#include <default.h>
#include <bitpos.h>

/*
 * bitpos32 - get lowest set bit's position, no set bit return 32.
 * @x: value to be bit-position.
 */
x_u32 bitpos32(x_u32 x)
{
	x_u32 index = x;

	index = (index - 1) & (~index);
	index = (index & 0x55555555) + ((index >> 1) & 0x55555555);
	index = (index & 0x33333333) + ((index >> 2) & 0x33333333);
	index = (index & 0x0f0f0f0f) + ((index >> 4) & 0x0f0f0f0f);
	index = (index & 0xff) + ((index & 0xff00) >> 8) + ((index & 0xff0000) >> 16) + ((index & 0xff000000) >> 24);

	return (index);
}

/*
 * TRUE for n is a power of 2.
 */
x_bool is_power_of_2(x_u32 n)
{
	return ( (!!n) & (!(n & (n-1))) );
}
