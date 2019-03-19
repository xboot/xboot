#include <x.h>
#include <sandbox.h>

void sandbox_pm_shutdown(void)
{
	sandbox_exit();
}

void sandbox_pm_reboot(void)
{
}

void sandbox_pm_sleep(void)
{
}
