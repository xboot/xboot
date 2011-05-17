#ifndef __CRC_CCITT_H__
#define __CRC_CCITT_H__

#include <xboot.h>
#include <types.h>

u16_t crc_ccitt(u16_t crc, const u8_t * buf, size_t len);

#endif /* __CRC_CCITT_H__ */
