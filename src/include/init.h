#ifndef __INIT_H__
#define __INIT_H__

#include <xboot.h>
#include <types.h>
#include <string.h>

void do_system_rootfs(void);
void do_system_cfg(void);
void do_system_fonts(void);
void do_system_wait(void);

#endif /* __INIT_H__ */
