#ifndef __MPAD_AT88SA102S_H__
#define __MPAD_AT88SA102S_H__

#include <xboot.h>

bool_t sa_read_serial_number(u8_t * buf);
bool_t sa_read_manufacture_id(u8_t * buf);
bool_t sa_read_revision(u8_t * buf);
bool_t sa_read_fuse_status(u8_t * buf);
bool_t sa_burn_secure(u8_t * key, u8_t * fuse);
bool_t sa_do_mac(u8_t mode, u16_t keyid, const u8_t * challenge, const u8_t * expected);
bool_t sa_do_auth(void);

#endif /* __MPAD_AT88SA102S_H__ */
