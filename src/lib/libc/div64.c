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

#include <div64.h>

/*
 * the internal funtion for 64-bit div and mod.
 */
static u64_t div_mod_64(u64_t num, u64_t den, u64_t * rem_p)
{
	u64_t quot = 0, qbit = 1;

	if (den == 0)
	{
		return 0;
	}

	while ((s64_t) den >= 0)
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
u64_t div64(u64_t num, u64_t den)
{
	return div_mod_64(num, den, 0);
}

/*
 * 64-bit unsigned integer modulo.
 */
u64_t mod64(u64_t num, u64_t den)
{
	u64_t v = 0;

	div_mod_64(num, den, &v);
	return v;
}

/*
 * div64_64
 */
u64_t div64_64(u64_t * num, u64_t den)
{
	u64_t v = 0;

	*num = div_mod_64(*num, den, &v);
	return v;
}
