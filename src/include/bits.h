#ifndef __BITS_H__
#define __BITS_H__

#define BITS_PER_TYPE(type)		(sizeof(type) * 8)
#define BITS_PER_LONG			BITS_PER_TYPE(long)
#define BITS_PER_LONG_LONG		BITS_PER_TYPE(long long)

/**
 * BIT - create a bitmask (long)
 * @n: bit position
 */
#define BIT(n) (                               \
	(1UL) << ((n) % BITS_PER_LONG)             \
)

/**
 * BIT_ULL - create a bitmask (long long)
 * @n: bit position
 */
#define BIT_ULL(n) (                           \
	(1ULL) << ((n) % BITS_PER_LONG_LONG)       \
)

/**
 * BIT_SHIFT - create a shifted bitmask (long)
 * @shift: bitmask position
 * @val: bitmask value
 */
#define BIT_SHIFT(shift, val) (                 \
	(val) << ((shift) % BITS_PER_LONG)          \
)

/**
 * BIT_SHIFT_ULL - create a shifted bitmask (long long)
 * @shift: bitmask position
 * @val: bitmask value
 */
#define BIT_SHIFT_ULL(shift, val) (             \
	(val) << ((shift) % BITS_PER_LONG_LONG)     \
)

/**
 * BIT_RANGE - create a contiguous bitmask (long)
 * @hi: ending position
 * @lo: starting position
 */
#define BIT_RANGE(hi, lo) (                     \
	((~0UL) - (1UL << (lo)) + 1) &              \
	(~0UL >> (BITS_PER_LONG - 1 - (hi)))        \
)

/**
 * BIT_RANGE_ULL - create a contiguous bitmask (long long)
 * @hi: ending position
 * @lo: starting position
 */
#define BIT_RANGE_ULL(hi, lo) (                 \
	((~0ULL) - (1ULL << (lo)) + 1) &            \
	(~0ULL >> (BITS_PER_LONG_LONG - 1 - (hi)))  \
)

#endif  /* __BITS_H__ */
