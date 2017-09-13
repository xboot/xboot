#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>
#include <string.h>

uint32_t crc32(uint32_t crc, const uint8_t * buf, size_t len);

#endif /* __CRC32_H__ */
