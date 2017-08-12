/*
 * cpu-profiler.c
 */

#include <pmu.h>

void cpu_profiler_start(int event, int data)
{
	pmn_config(data, event);
	pmn_enable(data);
}

void cpu_profiler_stop(int event, int data)
{
	pmn_disable(data);
}

uint64_t cpu_profiler_read(int event, int data)
{
	return pmn_read(data);
}

void cpu_profiler_reset(void)
{
	pmu_enable();
	pmu_user_enable();
	pmn_reset();
	ccnt_reset();
	ccnt_divider(0);
}
