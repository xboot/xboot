#ifndef __CRC32_H__
#define __CRC32_H__

#include <xboot.h>
#include <types.h>

u32_t crc32(u32_t crc, const u8_t * buf, size_t len);

#endif /* __CRC32_H__ */
