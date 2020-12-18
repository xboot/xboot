#ifndef __STDINT_H__
#define __STDINT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

typedef s8_t			int8_t;
typedef u8_t			uint8_t;

typedef s16_t			int16_t;
typedef u16_t			uint16_t;

typedef s32_t			int32_t;
typedef u32_t			uint32_t;

typedef s64_t			int64_t;
typedef u64_t			uint64_t;

#define INT8_MIN		(-1 - 0x7f)
#define INT16_MIN		(-1 - 0x7fff)
#define INT32_MIN		(-1 - 0x7fffffff)
#define INT64_MIN		(-1 - 0x7fffffffffffffff)

#define INT8_MAX		(0x7f)
#define INT16_MAX		(0x7fff)
#define INT32_MAX		(0x7fffffff)
#define INT64_MAX		(0x7fffffffffffffff)

#define UINT8_MAX		(0xff)
#define UINT16_MAX		(0xffff)
#define UINT32_MAX		(0xffffffffU)
#define UINT64_MAX		(0xffffffffffffffffU)

#ifdef __cplusplus
}
#endif

#endif /* __STDINT_H__ */
