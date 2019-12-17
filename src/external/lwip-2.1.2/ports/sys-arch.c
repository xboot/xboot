#include "lwip/sys.h"
#include "arch/sys-arch.h"

u32_t sys_now(void)
{
	return (u32_t)ktime_to_ms(ktime_get());
}
