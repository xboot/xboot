/*
 * libc/stdlib/rand.c
 */

#include <runtime.h>
#include <stdlib.h>

static long jrand48(unsigned short xsubi[3])
{
	u64_t x;

	/*
	 * The xsubi[] array is little endian by spec
	 */
	x = (u64_t) (u16_t)xsubi[0] +
	    ((u64_t) (u16_t)xsubi[1] << 16) +
	    ((u64_t) (u16_t)xsubi[2] << 32);

	x = (0x5deece66dULL * x) + 0xb;

	xsubi[0] = (unsigned short)(u16_t)x;
	xsubi[1] = (unsigned short)(u16_t)(x >> 16);
	xsubi[2] = (unsigned short)(u16_t)(x >> 32);

	return (long)(s32_t)(x >> 16);
}

static long lrand48(void)
{
	return (u32_t)jrand48(runtime_get()->__seed) >> 1;
}

static void srand48(long seedval)
{
	unsigned short * seed = runtime_get()->__seed;

	seed[0] = 0x330e;
	seed[1] = (unsigned short)seedval;
	seed[2] = (unsigned short)((u32_t)seedval >> 16);
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
