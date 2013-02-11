#ifndef __BYTEORDER_H__
#define __BYTEORDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

static inline u16_t __swab16(u16_t x)
{
	return ( (x<<8) | (x>>8) );
}

static inline u32_t __swab32(u32_t x)
{
	return ( (x<<24) | (x>>24) | \
		((x & (u32_t)0x0000ff00UL)<<8) | \
		((x & (u32_t)0x00ff0000UL)>>8) );
}

static inline u64_t __swab64(u64_t x)
{
	return ( (x<<56) | (x>>56) | \
		((x & (u64_t)0x000000000000ff00ULL)<<40) | \
		((x & (u64_t)0x0000000000ff0000ULL)<<24) | \
		((x & (u64_t)0x00000000ff000000ULL)<< 8) | \
		((x & (u64_t)0x000000ff00000000ULL)>> 8) | \
		((x & (u64_t)0x0000ff0000000000ULL)>>24) | \
		((x & (u64_t)0x00ff000000000000ULL)>>40) );
}

/*
 * swap bytes bizarrely.
 * swahw32 - swap 16-bit half-words in a 32-bit word
 */
static inline u32_t __swahw32(u32_t x)
{
	return ( ((x & (u32_t)0x0000ffffUL)<<16) | ((x & (u32_t)0xffff0000UL)>>16) );
}

/*
 * swap bytes bizarrely.
 * swahb32 - swap 8-bit halves of each 16-bit half-word in a 32-bit word
 */
static inline u32_t __swahb32(u32_t x)
{
	return ( ((x & (u32_t)0x00ff00ffUL)<<8) | ((x & (u32_t)0xff00ff00UL)>>8) );
}

#if (BYTE_ORDER == BIG_ENDIAN)
#define cpu_to_le64(x)	(__swab64((u64_t)(x)))
#define le64_to_cpu(x)	(__swab64((u64_t)(x)))
#define cpu_to_le32(x)	(__swab32((u32_t)(x)))
#define le32_to_cpu(x)	(__swab32((u32_t)(x)))
#define cpu_to_le16(x)	(__swab16((u16_t)(x)))
#define le16_to_cpu(x)	(__swab16((u16_t)(x)))
#define cpu_to_be64(x)	((u64_t)(x))
#define be64_to_cpu(x)	((u64_t)(x))
#define cpu_to_be32(x)	((u32_t)(x))
#define be32_to_cpu(x)	((u32_t)(x))
#define cpu_to_be16(x)	((u32_t)(x))
#define be16_to_cpu(x)	((u32_t)(x))
#else
#define cpu_to_le64(x)	((u64_t)(x))
#define le64_to_cpu(x)	((u64_t)(x))
#define cpu_to_le32(x)	((u32_t)(x))
#define le32_to_cpu(x)	((u32_t)(x))
#define cpu_to_le16(x)	((u16_t)(x))
#define le16_to_cpu(x)	((u16_t)(x))
#define cpu_to_be64(x)	(__swab64((u64_t)(x)))
#define be64_to_cpu(x)	(__swab64((u64_t)(x)))
#define cpu_to_be32(x)	(__swab32((u32_t)(x)))
#define be32_to_cpu(x)	(__swab32((u32_t)(x)))
#define cpu_to_be16(x)	(__swab16((u16_t)(x)))
#define be16_to_cpu(x)	(__swab16((u16_t)(x)))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BYTEORDER_H__ */
