#ifndef __CHARSET_H__
#define __CHARSET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

ssize_t utf8_to_ucs4(u32_t * dst, size_t dsz, const char * src, size_t ssz, const char ** end);
char * ucs4_to_utf8(u32_t * src, size_t ssz, char * dst, size_t dsz);

ssize_t utf8_to_utf16(u16_t * dst, size_t dsz, const char * src, size_t ssz, const char ** end);
char * utf16_to_utf8(char * dst, u16_t * src, size_t size);

ssize_t utf8_to_ucs4_alloc(const char * src, u32_t ** dst, u32_t ** pos);
char * ucs4_to_utf8_alloc(u32_t * src, size_t size);

int ucs4_width(u32_t uc);
size_t utf8_width(const char * s);

bool_t utf8_is_valid(const char * src, size_t size);
size_t utf8_strlen(const char * s);

#ifdef __cplusplus
}
#endif

#endif /* __CHARSET_H__ */
