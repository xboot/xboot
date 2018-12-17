#ifndef __CRC32_H__
#define __CRC32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t crc32_sum(uint32_t crc, const uint8_t * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __CRC32_H__ */
