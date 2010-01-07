#ifndef __PRINTK_H__
#define __PRINTK_H__


#include <configs.h>
#include <default.h>
#include <stdarg.h>


x_s32 printk(const char * fmt, ...);
void putch(char c);


#endif /* __PRINTK_H__ */
