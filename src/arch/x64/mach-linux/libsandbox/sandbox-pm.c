#include <x.h>
#include <sandbox.h>

void sandbox_pm_shutdown(void)
{
	sync();
	reboot(RB_POWER_OFF);
}

void sandbox_pm_reboot(void)
{
	sync();
	reboot(RB_AUTOBOOT);
}

void sandbox_pm_sleep(void)
{
	sync();
	sandbox_sysfs_write_string("/sys/power/state", "mem");
}
