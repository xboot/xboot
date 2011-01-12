/*
 * lib/libc/hash.c
 *
 * hash function for public.
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <string.h>
#include <hash.h>


/*
 * calc hash value for a string, copyed from mysql
 */
x_u32 string_hash(const char * s)
{
	x_u32 nr=1, nr2=4;
	x_s32 len = strlen((x_s8*)s);

	while(len--)
	{
		nr ^= ( ( (nr & 63) + nr2 ) * ( (x_u32)(x_u8)*s++) ) + (nr << 8);
		nr2 += 3;
	}
	return( (x_u32)nr);
}

/*
 * Fast hashing routine for a long.
 *
 * Knuth recommends primes in approximately golden ratio to the maximum
 * integer representable by a machine word for multiplicative hashing.
 * Chuck Lever verified the effectiveness of this technique:
 * http://www.citi.umich.edu/techreports/reports/citi-tr-00-1.pdf
 *
 * These primes are chosen to be bit-sparse, that is operations on
 * them can use shifts and additions instead of multiplications for
 * machines where multiplications are slow.
 *
 * 2^31 + 2^29 - 2^25 + 2^22 - 2^19 - 2^16 + 1
 */
inline x_u32 long_hash(x_u32 val, x_u32 bits)
{
	x_u32 hash = val;

	/* On some cpus multiply is faster, on others gcc will do shifts */
	hash *= 0x9e370001UL;

	/* High bits are more random, so use them. */
	return hash >> (32 - bits);
}

inline x_u32 ptr_hash(void *ptr, x_u32 bits)
{
	return long_hash((x_u32)ptr, bits);
}
