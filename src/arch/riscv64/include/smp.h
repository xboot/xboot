#ifndef __RISCV64_SMP_H__
#define __RISCV64_SMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <riscv64.h>

static inline int smp_processor_id(void)
{
	return csr_read(mhartid);
}

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_SMP_H__ */
