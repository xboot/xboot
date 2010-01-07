/*
 * lib/libc/rand.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <rand.h>


/*
 * the rand poll.
 */
static x_s32 hold_rand = 1;

/*
 * seed rand
 */
void srand(x_s32 seed)
{
	hold_rand = seed;
}

/*
 * rand
 */
x_s32 rand(void)
{
	return (((hold_rand = hold_rand * 214013L + 2531011L) >> 16) & 0x7fff);
}
