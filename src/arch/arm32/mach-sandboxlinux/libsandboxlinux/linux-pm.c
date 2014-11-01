#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sandboxlinux.h>

int sandbox_linux_pm_shutdown(void)
{
	sandbox_linux_exit();
	return 0;
}

int sandbox_linux_pm_reset(void)
{
	return 0;
}

int sandbox_linux_pm_sleep(void)
{
	return 0;
}
