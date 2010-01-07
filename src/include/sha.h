#ifndef __SHA_H__
#define __SHA_H__

#include <configs.h>
#include <default.h>


/*
 * compute SHA1 (160-bit) message digest
 */
const x_u8 * sha1(const void *data, x_s32 len, x_u8 * digest);

#endif /* __SHA_H__ */
