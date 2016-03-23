#ifndef __BCM2837_RNG_H__
#define __BCM2837_RNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <rng/rng.h>
#include <bcm2837/reg-rng.h>

struct bcm2837_rng_data_t
{
	physical_addr_t phys;
};

#ifdef __cplusplus
}
#endif

#endif /* __BCM2837_RNG_H__ */
