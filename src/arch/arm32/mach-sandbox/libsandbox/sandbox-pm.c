#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sandbox.h>

int sandbox_pm_poweron(void)
{
	return 0;
}

int sandbox_pm_poweroff(void)
{
	sandbox_exit();
	return 0;
}

int sandbox_pm_reboot(void)
{
	return 0;
}

int sandbox_pm_sleep(void)
{
	return 0;
}
