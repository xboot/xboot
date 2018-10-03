#ifndef _SYS_CLOCK_H
#define _SYS_CLOCK_H
#include "stdint.h"

#define PLL0_OUTPUT_FREQ 320000000UL
#define PLL1_OUTPUT_FREQ 160000000UL
#define PLL2_OUTPUT_FREQ 45158400UL
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Init PLL freqency
 */
void sys_clock_inita();

/**
 * @brief       Set frequency of CPU
 * @param[in]   frequency       The desired frequency in Hz
 *
 * @return      The actual frequency of CPU after set
 */
uint32_t system_set_cpu_frequency(uint32_t frequency);

#ifdef __cplusplus
}
#endif

#endif
