#ifndef __SHA_H__
#define __SHA_H__

#include <xboot.h>
#include <types.h>

/*
 * compute SHA1 (160-bit) message digest
 */
const u8_t * sha1(const void *data, s32_t len, u8_t * digest);

#endif /* __SHA_H__ */
