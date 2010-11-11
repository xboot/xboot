#ifndef __SCANK_H__
#define __SCANK_H__

#include <configs.h>
#include <default.h>
#include <stdarg.h>


x_s32 scank(const char * fmt, ...);
x_bool getch(char * c);
x_bool getch_with_timeout(char * c, x_u32 timeout);

x_bool getcode(x_u32 * code);
x_bool getcode_with_timeout(x_u32 * code, x_u32 timeout);

#endif /* __SCANK_H__ */
