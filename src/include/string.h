#ifndef __STRING_H__
#define __STRING_H__

#include <configs.h>
#include <default.h>

x_s32 strnicmp(const x_s8 *s1, const x_s8 *s2, x_s32 len);
x_s8 * strcpy(x_s8 *dest, const x_s8 *src);
x_s8 * strdup(const x_s8 * src);
x_s8 * strncpy(x_s8 *dest, const x_s8 *src, x_s32 count);
x_s32 strlcpy(x_s8 *dest, const x_s8 *src, x_s32 size);
x_s8 * strcat(x_s8 *dest, const x_s8 *src);
x_s8 * strncat(x_s8 *dest, const x_s8 *src, x_s32 count);
x_s32 strlcat(x_s8 *dest, const x_s8 *src, x_s32 count);
x_s8 * strchr(const x_s8 *s, x_s32 c);
x_s8 * strrchr(const x_s8 *s, x_s32 c);
x_s8 * strnchr(const x_s8 *s, x_s32 count, x_s32 c);
x_s8 * strstrip(x_s8 *s);
x_s32 strlen(const x_s8 *s);
x_s32 strnlen(const x_s8 *s, x_s32 count);
x_s32 strspn(const x_s8 *s, const x_s8 *accept);
x_s32 strcspn(const x_s8 *s, const x_s8 *reject);
x_s8 * strpbrk(const x_s8 *cs, const x_s8 *ct);
x_s8 * strsep(x_s8 **s, const x_s8 *ct);
void * memscan(void *addr, x_s32 c, x_s32 size);
x_s8 * strstr(const x_s8 *s1, const x_s8 *s2);
void * memchr(const void *s, x_s32 c, x_s32 n);

x_s32 strcmp(const x_s8 *cs, const x_s8 *ct);
x_s32 strncmp(const x_s8 *cs, const x_s8 *ct, x_s32 count);
void * memset(void *s, x_s32 c, x_s32 count);
void * memcpy(void *dest, const void *src, x_s32 count);
void * memmove(void *dest, const void *src, x_s32 count);
x_s32  memcmp(const void *cs, const void *ct, x_s32 count);

#endif /* __STRING_H__ */
