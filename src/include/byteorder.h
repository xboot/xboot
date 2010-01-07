#ifndef __BYTEORDER_H__
#define __BYTEORDER_H__

#include <configs.h>
#include <default.h>


static inline x_u16 __swab16(x_u16 x)
{
	return ( (x<<8) | (x>>8) );
}

static inline x_u32 __swab32(x_u32 x)
{
	return ( (x<<24) | (x>>24) | \
		((x & (x_u32)0x0000ff00UL)<<8) | \
		((x & (x_u32)0x00ff0000UL)>>8) );
}

static inline x_u64 __swab64(x_u64 x)
{
	return ( (x<<56) | (x>>56) | \
		((x & (x_u64)0x000000000000ff00ULL)<<40) | \
		((x & (x_u64)0x0000000000ff0000ULL)<<24) | \
		((x & (x_u64)0x00000000ff000000ULL)<< 8) | \
		((x & (x_u64)0x000000ff00000000ULL)>> 8) | \
		((x & (x_u64)0x0000ff0000000000ULL)>>24) | \
		((x & (x_u64)0x00ff000000000000ULL)>>40) );
}

/*
 * swap bytes bizarrely.
 * swahw32 - swap 16-bit half-words in a 32-bit word
 */
static inline x_u32 __swahw32(x_u32 x)
{
	return ( ((x & (x_u32)0x0000ffffUL)<<16) | ((x & (x_u32)0xffff0000UL)>>16) );
}

/*
 * swap bytes bizarrely.
 * swahb32 - swap 8-bit halves of each 16-bit half-word in a 32-bit word
 */
static inline x_u32 __swahb32(x_u32 x)
{
	return ( ((x & (x_u32)0x00ff00ffUL)<<8) | ((x & (x_u32)0xff00ff00UL)>>8) );
}

#if	defined(__LITTLE_ENDIAN)
#define cpu_to_le64(x)	((x_u64)(x))
#define le64_to_cpu(x)	((x_u64)(x))
#define cpu_to_le32(x)	((x_u32)(x))
#define le32_to_cpu(x)	((x_u32)(x))
#define cpu_to_le16(x)	((x_u16)(x))
#define le16_to_cpu(x)	((x_u16)(x))
#define cpu_to_be64(x)	(__swab64((x_u64)(x)))
#define be64_to_cpu(x)	(__swab64((x_u64)(x)))
#define cpu_to_be32(x)	(__swab32((x_u32)(x)))
#define be32_to_cpu(x)	(__swab32((x_u32)(x)))
#define cpu_to_be16(x)	(__swab16((x_u16)(x)))
#define be16_to_cpu(x)	(__swab16((x_u16)(x)))
#elif defined(__BIG_ENDIAN)
#define cpu_to_le64(x)	(__swab64((x_u64)(x)))
#define le64_to_cpu(x)	(__swab64((x_u64)(x)))
#define cpu_to_le32(x)	(__swab32((x_u32)(x)))
#define le32_to_cpu(x)	(__swab32((x_u32)(x)))
#define cpu_to_le16(x)	(__swab16((x_u16)(x)))
#define le16_to_cpu(x)	(__swab16((x_u16)(x)))
#define cpu_to_be64(x)	((x_u64)(x))
#define be64_to_cpu(x)	((x_u64)(x))
#define cpu_to_be32(x)	((x_u32)(x))
#define be32_to_cpu(x)	((x_u32)(x))
#define cpu_to_be16(x)	((x_u32)(x))
#define be16_to_cpu(x)	((x_u32)(x))
#endif

#endif /* __BYTEORDER_H__ */
