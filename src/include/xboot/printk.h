#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <xboot.h>
#include <stdarg.h>

bool_t putcode(u32_t code);
void putch(char c);
int printk(const char * fmt, ...);

#endif /* __PRINTK_H__ */
