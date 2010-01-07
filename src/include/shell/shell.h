#ifndef __SHELL_H__
#define __SHELL_H__


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>


void run_cmdline(const x_s8 *cmdline);
void run_shell_mode(void);

#endif /* __SHELL_H__ */
