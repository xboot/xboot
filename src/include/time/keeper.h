#ifndef __KEEPER_H__
#define __KEEPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

ktime_t ktime_get(void);
void subsys_init_keeper(void);

#ifdef __cplusplus
}
#endif

#endif /* __KEEPER_H__ */
