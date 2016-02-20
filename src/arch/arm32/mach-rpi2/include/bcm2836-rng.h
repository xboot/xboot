#ifndef __BCM2836_RNG_H__
#define __BCM2836_RNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <rng/rng.h>
#include <bcm2836/reg-rng.h>

struct bcm2836_rng_data_t
{
	physical_addr_t phys;
};

#ifdef __cplusplus
}
#endif

#endif /* __BCM2836_RNG_H__ */
