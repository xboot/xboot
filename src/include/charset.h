#ifndef __CHARSET_H__
#define __CHARSET_H__

#include <xboot.h>


s32_t utf8_to_ucs4(u32_t * dst, s32_t dst_size, const s8_t * src, s32_t src_size, const s8_t ** src_end);
s32_t ucs4_to_utf8(u32_t c, s8_t * buf);

s32_t utf8_to_utf16(u16_t * dst, s32_t dst_size, const s8_t * src, s32_t src_size, const s8_t ** src_end);
s8_t * utf16_to_utf8(s8_t * dst, u16_t * src, s32_t size);

s8_t * ucs4_to_utf8_alloc(u32_t * src, s32_t size);
s32_t utf8_to_ucs4_alloc(const s8_t * src, u32_t ** dst, u32_t ** pos);

s32_t ucs4_width(u32_t uc);
s32_t utf8_width(const char * str);

bool_t utf8_is_valid(const s8_t * src, s32_t size);
s32_t utf8_strlen(const s8_t * s);

#endif /* __CHARSET_H__ */
