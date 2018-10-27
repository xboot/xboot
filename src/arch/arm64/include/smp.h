#ifndef __ARM64_SMP_H__
#define __ARM64_SMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <arm64.h>

static inline int smp_processor_id(void)
{
	return arm64_read_sysreg(tpidrro_el0);
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_SMP_H__ */
