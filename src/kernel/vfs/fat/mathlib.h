
#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#include "fat_common.h"

#define do_abs(x)  ((x) < 0 ? -(x) : (x))

extern u64 do_udiv64(u64 dividend, u64 divisor, u64 * remainder);

static inline u64 udiv64(u64 value, u64 divisor)
{
	u64 r;
	return do_udiv64(value, divisor, &r);
}

static inline u64 umod64(u64 value, u64 divisor)
{
	u64 r;
	do_udiv64(value, divisor, &r);
	return r;
}

static inline s64 sdiv64(s64 value, s64 divisor)
{
	u64 r;
	if ((value * divisor) < 0) {
		return -do_udiv64(do_abs(value), do_abs(divisor), &r);
	} else { /* positive value */
		return do_udiv64(do_abs(value), do_abs(divisor), &r);
	}
}

static inline s64 smod64(s64 value, s64 divisor)
{
	u64 r;
	do_udiv64( do_abs(value), do_abs(divisor), &r);
	if (value < 0) {
		return -r;
	} else { /* positive value */
		return r;
	}
}

extern u32 do_udiv32(u32 dividend, u32 divisor, u32 * remainder);

static inline u32 udiv32(u32 value, u32 divisor)
{
	u32 r;
	return do_udiv32(value, divisor, &r);
}

static inline u32 umod32(u32 value, u32 divisor)
{
	u32 r;
	do_udiv32(value, divisor, &r);
	return r;
}

static inline s32 sdiv32(s32 value, s32 divisor)
{
	u32 r;
	if ((value * divisor) < 0) {
		return -do_udiv32(do_abs(value), do_abs(divisor), &r);
	} else { /* positive value */
		return do_udiv32(do_abs(value), do_abs(divisor), &r);
	}
}

static inline s32 smod32(s32 value, s32 divisor)
{
	u32 r;
	do_udiv32( do_abs(value), do_abs(divisor), &r );
	if (value < 0) {
		return -r;
	} else { /* positive value */
		return r;
	}
}

#endif /* __MATHLIB_H__ */
