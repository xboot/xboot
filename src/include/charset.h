#ifndef __CHARSET_H__
#define __CHARSET_H__

#include <configs.h>
#include <default.h>


x_s32 utf8_to_ucs4(x_u32 * dst, x_s32 dst_size, const x_u8 * src, x_s32 src_size, const x_u8 ** src_end);
x_s32 ucs4_to_utf8(x_u32 c, x_u8 * buf);

x_s32 utf8_to_utf16(x_u16 * dst, x_s32 dst_size, const x_u8 * src, x_s32 src_size, const x_u8 ** src_end);
x_u8 * utf16_to_utf8(x_u8 * dst, x_u16 * src, x_s32 size);

x_u8 * ucs4_to_utf8_alloc(x_u32 * src, x_s32 size);
x_s32 utf8_to_ucs4_alloc(const x_u8 * src, x_u32 ** dst, x_u32 ** pos);

x_bool utf8_is_valid(const x_u8 * src, x_s32 size);
x_s32 utf8_strlen(const x_u8 * s);

#endif /* __CHARSET_H__ */
