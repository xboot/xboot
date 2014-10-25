#ifndef __INIT_H__
#define __INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

void do_system_rootfs(void);
void do_system_logo(void);
void do_system_autoboot(void);

#ifdef __cplusplus
}
#endif

#endif /* __INIT_H__ */
