/*
 * libc/stdlib/rand.c
 */

#include <stdlib.h>


static int hold_rand = 1;

void srand(int seed)
{
	hold_rand = seed;
}

int rand(void)
{
	return (((hold_rand = hold_rand * 214013L + 2531011L) >> 16) & 0x7fff);
}
