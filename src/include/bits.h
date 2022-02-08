/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __BITS_H__
#define __BITS_H__

#define BITS_PER_BYTE       8
#define BITS_PER_TYPE(type) (sizeof(type) * BITS_PER_BYTE)

#define BITS_PER_U8         BITS_PER_TYPE(uint8_t)
#define BITS_PER_U16        BITS_PER_TYPE(uint16_t)
#define BITS_PER_U32        BITS_PER_TYPE(uint32_t)
#define BITS_PER_U64        BITS_PER_TYPE(uint64_t)
#define BITS_PER_SHORT      BITS_PER_TYPE(short)
#define BITS_PER_INT        BITS_PER_TYPE(int)
#define BITS_PER_LONG       BITS_PER_TYPE(long)
#define BITS_PER_LONG_LONG  BITS_PER_TYPE(long long)

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

#endif  /* __BITS_H__ */
