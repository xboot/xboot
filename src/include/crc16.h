#ifndef __CRC16_H__
#define __CRC16_H__

#include <xboot.h>
#include <types.h>

u16_t crc16(u16_t crc, const u8_t *buffer, u32_t len);

#endif /* __CRC16_H__ */
