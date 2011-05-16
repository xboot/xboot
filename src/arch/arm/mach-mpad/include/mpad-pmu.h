#ifndef __MPAD_PMU_H__
#define __MPAD_PMU_H__

#include <xboot.h>

bool_t pmu_write(u8_t reg, u8_t value);
bool_t pmu_read(u8_t reg, u8_t * value);

#endif /* __MPAD_PMU_H__ */
