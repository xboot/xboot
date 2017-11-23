#ifndef __CRC16_H__
#define __CRC16_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <string.h>

uint16_t crc16_sum(uint16_t crc, const uint8_t * buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* __CRC16_H__ */
