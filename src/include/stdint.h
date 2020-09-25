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

#define UINT8_MAX		(0xff)
#define UINT16_MAX		(0xffff)
#define UINT32_MAX		(0xffffffff)
#define UINT64_MAX		(0xffffffffffffffffULL)

#ifdef __cplusplus
}
#endif

#endif /* __STDINT_H__ */
