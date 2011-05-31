#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <xboot.h>
#include <stdarg.h>

bool_t putcode(u32_t code);
void putch(char c);
int printk(const char * fmt, ...);

bool_t getcode(u32_t * code);
bool_t getcode_with_timeout(u32_t * code, u32_t timeout);

#endif /* __PRINTK_H__ */
