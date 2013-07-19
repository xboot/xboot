#ifndef __INIT_H__
#define __INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

void do_system_rootfs(void);
void do_system_cfg(void);
void do_system_fonts(void);
void do_system_battery(void);
void do_system_wait(void);

#ifdef __cplusplus
}
#endif

#endif /* __INIT_H__ */
