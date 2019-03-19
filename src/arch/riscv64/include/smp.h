#ifndef __RISCV64_SMP_H__
#define __RISCV64_SMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfigs.h>
#include <riscv64.h>

#if defined(CONFIG_MAX_SMP_CPUS) && (CONFIG_MAX_SMP_CPUS > 1) && !defined(__SANDBOX__)
static inline int smp_processor_id(void)
{
	return csr_read(mhartid);
}
#else
static inline int smp_processor_id(void)
{
	return 0;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_SMP_H__ */
