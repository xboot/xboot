/*
 * libc/stdio/system.c
 */

#include <shell/shell.h>
#include <stdio.h>

int system(const char * cmd)
{
	return shell_system(cmd);
}
EXPORT_SYMBOL(system);
