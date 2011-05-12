#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <configs.h>
#include <default.h>
#include <stdarg.h>


bool_t putcode(u32_t code);
void putch(char c);
s32_t printk(const char * fmt, ...);

#endif /* __PRINTK_H__ */
