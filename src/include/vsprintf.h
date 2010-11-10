#ifndef __VSPRINTF_H__
#define __VSPRINTF_H__

#include <configs.h>
#include <default.h>
#include <stdarg.h>


x_u32 simple_strtou32(const x_s8 *cp, x_s8 **endp, x_u32 base);
x_s32 simple_strtos32(const x_s8 *cp, x_s8 **endp, x_u32 base);
x_u64 simple_strtou64(const x_s8 *cp, x_s8 **endp, x_u32 base);
x_s64 simple_strtos64(const x_s8 *cp, x_s8 **endp, x_u32 base);

x_s32 vsnprintf(x_s8 *buf, x_s32 size, const x_s8 *fmt, va_list args);
x_s32 vsscanf(const x_s8 * buf, const x_s8 * fmt, va_list args);

x_s32 sprintf(x_s8 * buf, const x_s8 * fmt, ...);
x_s32 snprintf(x_s8 * buf, x_s32 size, const x_s8 * fmt, ...);
x_s32 sscanf(const x_s8 * buf, const x_s8 * fmt, ...);
x_s32 ssize(x_s8 * buf, x_u64 size);

#endif /* __VSPRINTF_H__ */
