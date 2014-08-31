#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sandbox-linux.h>

int sandbox_linux_pm_sleep(void)
{
	return 0;
}

int sandbox_linux_pm_halt(void)
{
	sandbox_linux_exit();
	return 0;
}

int sandbox_linux_pm_reset(void)
{
	return 0;
}
