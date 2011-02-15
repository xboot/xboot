#ifndef __MPAD_PMU_H__
#define __MPAD_PMU_H__

#include <configs.h>
#include <default.h>

x_bool pmu_write(x_u8 reg, x_u8 value);
x_bool pmu_read(x_u8 reg, x_u8 * value);

#endif /* __MPAD_PMU_H__ */
