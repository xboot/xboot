#ifndef __VSPRINTF_H__
#define __VSPRINTF_H__

#include <xboot.h>
#include <types.h>
#include <stdarg.h>


u32_t simple_strtou32(const s8_t *cp, s8_t **endp, u32_t base);
s32_t simple_strtos32(const s8_t *cp, s8_t **endp, u32_t base);
u64_t simple_strtou64(const s8_t *cp, s8_t **endp, u32_t base);
s64_t simple_strtos64(const s8_t *cp, s8_t **endp, u32_t base);

s32_t vsnprintf(s8_t *buf, s32_t size, const s8_t *fmt, va_list args);
s32_t vsscanf(const s8_t * buf, const s8_t * fmt, va_list args);

int sprintf(char * buf, const char * fmt, ...);
int snprintf(char * buf, size_t size, const char * fmt, ...);
s32_t sscanf(const s8_t * buf, const s8_t * fmt, ...);
s32_t ssize(char * buf, u64_t size);

#endif /* __VSPRINTF_H__ */
