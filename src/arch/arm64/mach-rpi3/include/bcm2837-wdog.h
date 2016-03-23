#ifndef __BCM2837_WDOG_H__
#define __BCM2837_WDOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <watchdog/watchdog.h>
#include <bcm2837/reg-pm.h>

struct bcm2837_wdog_data_t
{
	physical_addr_t phys;
};

#ifdef __cplusplus
}
#endif

#endif /* __BCM2837_WDOG_H__ */
