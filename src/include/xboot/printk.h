#ifndef __PRINTK_H__
#define __PRINTK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <console/console.h>

int printk(const char * fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __PRINTK_H__ */
