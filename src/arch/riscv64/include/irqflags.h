#ifndef __RISCV64_IRQFLAGS_H__
#define __RISCV64_IRQFLAGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

static inline void arch_local_irq_enable(void)
{
}

static inline void arch_local_irq_disable(void)
{
}

static inline irq_flags_t arch_local_irq_save(void)
{
	return 0;
}

static inline void arch_local_irq_restore(irq_flags_t flags)
{
}

#define local_irq_enable()			do { arch_local_irq_enable(); } while(0)
#define local_irq_disable()			do { arch_local_irq_disable(); } while(0)
#define local_irq_save(flags)		do { flags = arch_local_irq_save(); } while(0)
#define local_irq_restore(flags)	do { arch_local_irq_restore(flags); } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_IRQFLAGS_H__ */
