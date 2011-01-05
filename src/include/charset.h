#ifndef __CHARSET_H__
#define __CHARSET_H__

#include <configs.h>
#include <default.h>


x_s32 utf8_to_ucs4(x_u32 * dst, x_s32 dst_size, const x_s8 * src, x_s32 src_size, const x_s8 ** src_end);
x_s32 ucs4_to_utf8(x_u32 c, x_s8 * buf);

x_s32 utf8_to_utf16(x_u16 * dst, x_s32 dst_size, const x_s8 * src, x_s32 src_size, const x_s8 ** src_end);
x_s8 * utf16_to_utf8(x_s8 * dst, x_u16 * src, x_s32 size);

x_s8 * ucs4_to_utf8_alloc(x_u32 * src, x_s32 size);
x_s32 utf8_to_ucs4_alloc(const x_s8 * src, x_u32 ** dst, x_u32 ** pos);

x_s32 ucs4_width(x_u32 uc);
x_s32 utf8_width(const char * str);

x_bool utf8_is_valid(const x_s8 * src, x_s32 size);
x_s32 utf8_strlen(const x_s8 * s);
x_s8 * utf8_strcpy(x_s8 * dest, const x_s8 * src);
x_s8 * utf8_strdup(const x_s8 * s);
x_s32 utf8_strcmp(const x_s8 * cs, const x_s8 * ct);

#endif /* __CHARSET_H__ */
