#ifndef __DIV_H__
#define __DIV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define do_abs(x)	((x) < 0 ? -(x) : (x))

extern u32_t do_udiv32(u32_t dividend, u32_t divisor, u32_t * remainder);
extern u64_t do_udiv64(u64_t dividend, u64_t divisor, u64_t * remainder);

static inline u32_t udiv32(u32_t value, u32_t divisor)
{
	u32_t r;
	return do_udiv32(value, divisor, &r);
}

static inline u32_t umod32(u32_t value, u32_t divisor)
{
	u32_t r;

	do_udiv32(value, divisor, &r);
	return r;
}

static inline s32_t sdiv32(s32_t value, s32_t divisor)
{
	u32_t r;

	if((value * divisor) < 0)
		return -do_udiv32(do_abs(value), do_abs(divisor), &r);
	else
		return do_udiv32(do_abs(value), do_abs(divisor), &r);
}

static inline s32_t smod32(s32_t value, s32_t divisor)
{
	u32_t r;

	do_udiv32(do_abs(value), do_abs(divisor), &r);
	if(value < 0)
		return -r;
	else
		return r;
}

static inline u64_t udiv64(u64_t value, u64_t divisor)
{
	u64_t r;
	return do_udiv64(value, divisor, &r);
}

static inline u64_t umod64(u64_t value, u64_t divisor)
{
	u64_t r;

	do_udiv64(value, divisor, &r);
	return r;
}

static inline s64_t sdiv64(s64_t value, s64_t divisor)
{
	u64_t r;

	if((value * divisor) < 0)
		return -do_udiv64(do_abs(value), do_abs(divisor), &r);
	else
		return do_udiv64(do_abs(value), do_abs(divisor), &r);
}

static inline s64_t smod64(s64_t value, s64_t divisor)
{
	u64_t r;

	do_udiv64(do_abs(value), do_abs(divisor), &r);
	if(value < 0)
		return -r;
	else
		return r;
}

#ifdef __cplusplus
}
#endif

#endif /* __DIV_H__ */
