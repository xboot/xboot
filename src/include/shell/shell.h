#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

int shell_realpath(const char * path, char * fpath);
const char * shell_getcwd(void);
int shell_setcwd(const char * path);
int shell_system(const char * cmdline);
void shell_task(struct task_t * task, void * data);

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_H__ */
