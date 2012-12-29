#ifndef __EXEC_H__
#define __EXEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>

void exec_cmdline(const char * cmdline);

#ifdef __cplusplus
}
#endif

#endif /* __EXEC_H__ */
