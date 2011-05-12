#ifndef __CRC32_H__
#define __CRC32_H__

#include <configs.h>
#include <default.h>
#include <types.h>

u32_t crc32(u32_t crc, const u8_t *buf, u32_t len);

#endif /* __CRC32_H__ */
