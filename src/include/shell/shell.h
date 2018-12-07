#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

void shell_task(struct task_t * task, void * data);

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_H__ */
