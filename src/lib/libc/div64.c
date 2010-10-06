/*
 * lib/libc/div64.c
 *
 * Generic C version of 64bit/32bit division and modulo, with
 * 64bit result and 32bit remainder.
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
#include <div64.h>

/*
 * the div zero handle.
 */
extern void __div0(void);

/*
 * the internal funtion for 64-bit div and mod.
 */
static x_u64 div_mod_64(x_u64 num, x_u64 den, x_u64 * rem_p)
{
	x_u64 quot = 0, qbit = 1;

	if (den == 0)
	{
		return 0;
	}

	while ((x_s64) den >= 0)
	{
		den <<= 1;
		qbit <<= 1;
	}

	while (qbit)
	{
		if (den <= num)
		{
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}

	if (rem_p)
		*rem_p = num;

	return quot;
}

/*
 * 64-bit unsigned integer divide.
 */
x_u64 div64(x_u64 num, x_u64 den)
{
	return div_mod_64(num, den, 0);
}

/*
 * 64-bit unsigned integer modulo.
 */
x_u64 mod64(x_u64 num, x_u64 den)
{
	x_u64 v;

	div_mod_64(num, den, &v);
	return v;
}

/*
 * div64_64
 */
x_u64 div64_64(x_u64 * num, x_u64 den)
{
	x_u64 v;

	*num = div_mod_64(*num, den, &v);
	return v;
}
