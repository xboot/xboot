/*
 * libc/stdio/system.c
 */

#include <stdio.h>
#include <shell/shell.h>
#include <xboot/module.h>

int system(const char * cmd)
{
	return shell_system(cmd);
}
EXPORT_SYMBOL(system);
