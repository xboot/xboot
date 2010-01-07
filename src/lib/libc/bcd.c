/*
 * lib/libc/bcd.c
 */

#include <configs.h>
#include <default.h>
#include <bcd.h>

x_u32 bcd2bin(x_u8 val)
{
	return (val & 0x0f) + (val >> 4) * 10;
}

x_u8 bin2bcd(x_u32 val)
{
	return ((val / 10) << 4) + val % 10;
}
