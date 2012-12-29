#ifndef __SHA_H__
#define __SHA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>

/*
 * compute SHA1 (160-bit) message digest
 */
const u8_t * sha1(const void *data, s32_t len, u8_t * digest);

#ifdef __cplusplus
}
#endif

#endif /* __SHA_H__ */
