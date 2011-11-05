#ifndef __MPAD_PMU_H__
#define __MPAD_PMU_H__

#include <xboot.h>

bool_t pmu_init(void);
bool_t pmu_write(u8_t reg, u8_t val);
bool_t pmu_read(u8_t reg, u8_t * val);
bool_t pmu_nread(u8_t reg, u8_t * buf, u32_t cnt);

#endif /* __MPAD_PMU_H__ */
