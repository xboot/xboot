/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_MACRO_H__
#define __CSKY_MACRO_H__

#define BITS_PER_LONG (32)
#define BITS_PER_LONG_LONG (64)

/**
 * BIT - create a bitmask (long)
 * @nr: bit position
 * For example:
 * BIT(31) gives us the vector 0x80000000
 */
#define BIT(nr) (                               \
    (1UL) << ((nr) % BITS_PER_LONG)             \
)

/**
 * BIT_ULL - create a bitmask (long long)
 * @nr: bit position
 */
#define BIT_ULL(nr) (                           \
    (1ULL) << ((nr) % BITS_PER_LONG_LONG)       \
)

/**
 * BIT_SHIFT - create a shifted bitmask (long)
 * @shift: bitmask position
 * @val: bitmask value
 * For example:
 * BIT_SHIFT(30, 3) gives us the vector 0xc0000000
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
 * For example:
 * BIT_RANGE(31, 21) gives us the vector 0xffe00000
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
    ((~0UL) - (1UL << (lo)) + 1) &              \
    (~0UL >> (BITS_PER_LONG_LONG - 1 - (hi)))   \
)

#endif  /* __MACRO_H__ */
