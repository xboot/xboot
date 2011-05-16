#ifndef __CRC16_CCITT_H__
#define __CRC16_CCITT_H__

#include <xboot.h>
#include <types.h>


u16_t crc16_ccitt(const u8_t *buf, u32_t len);

#endif /* __CRC16_CCITT_H__ */
