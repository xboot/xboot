#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <configs.h>
#include <default.h>
#include <stdarg.h>

x_bool putcode(x_u32 code);
void putch(char c);
x_s32 printk(const char * fmt, ...);

void refresh(void);

#endif /* __PRINTK_H__ */
