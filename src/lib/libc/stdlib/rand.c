/*
 * libc/stdlib/rand.c
 */

#include <clocksource/clocksource.h>
#include <stdlib.h>

/*
 * Pseudo random seed - 48bits
 */
static unsigned short __seed[3] = {1, 1, 1};

static long jrand48(unsigned short * xsubi)
{
	u64_t x;

	x = (u64_t)(u16_t)xsubi[0] + ((u64_t)(u16_t)xsubi[1] << 16) + ((u64_t)(u16_t)xsubi[2] << 32);
	x = (0x5deece66dULL * x) + 0xb;

	xsubi[0] = (unsigned short)(u16_t)x;
	xsubi[1] = (unsigned short)(u16_t)(x >> 16);
	xsubi[2] = (unsigned short)(u16_t)(x >> 32);

	return (long)(s32_t)(x >> 16);
}

static long lrand48(void)
{
	return (u32_t)jrand48(__seed) >> 1;
}

static void srand48(long val)
{
	ktime_t kt = ktime_get();

	val ^= (long)(ktime_to_ns(kt) >> 32);
	val ^= (long)(ktime_to_ns(kt) >> 0);
	__seed[0] = 0x330e;
	__seed[1] = (unsigned short)val;
	__seed[2] = (unsigned short)((u32_t)val >> 16);
}

int rand(void)
{
	return (int)lrand48();
}
EXPORT_SYMBOL(rand);

void srand(unsigned int seed)
{
	srand48(seed);
}
EXPORT_SYMBOL(srand);
