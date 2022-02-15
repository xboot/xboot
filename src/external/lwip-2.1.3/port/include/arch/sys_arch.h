#ifndef __LWIP_SYS_ARCH_H__
#define __LWIP_SYS_ARCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct sys_sem;
typedef struct sys_sem * sys_sem_t;
#define sys_sem_valid(sem)					(((sem) != NULL) && (*(sem) != NULL))
#define sys_sem_valid_val(sem)				((sem) != NULL)
#define sys_sem_set_invalid(sem)			do { if((sem) != NULL) { *(sem) = NULL; } } while(0)
#define sys_sem_set_invalid_val(sem)		do { (sem) = NULL; } while(0)

typedef struct sys_mutex_t {
	struct mutex_t * m;
} sys_mutex_t;
#define sys_mutex_valid(mutex)				(((mutex) != NULL) && ((mutex)->m != NULL))
#define sys_mutex_valid_val(mutex)			((mutex).m != NULL)
#define sys_mutex_set_invalid(mutex)		do { if((mutex) != NULL) { (mutex)->m = NULL; } } while(0)
#define sys_mutex_set_invalid_val(mutex)	do { (mutex).m = NULL; } while(0)

typedef struct sys_mbox_t {
	struct channel_t * ch;
} sys_mbox_t;
#define sys_mbox_valid(mbox)				(((mbox) != NULL) && ((mbox)->ch != NULL))
#define sys_mbox_valid_val(mbox)			((mbox).ch != NULL)
#define sys_mbox_set_invalid(mbox)			do { if((mbox) != NULL) { (mbox)->ch = NULL; } } while(0)
#define sys_mbox_set_invalid_val(mbox)		do { (mbox).ch = NULL; } while(0)

typedef struct sys_thread {
	struct task_t * task;
} sys_thread_t;

#define sys_msleep						msleep

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_SYS_ARCH_H__ */
