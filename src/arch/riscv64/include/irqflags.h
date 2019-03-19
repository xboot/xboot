#ifndef __RISCV64_IRQFLAGS_H__
#define __RISCV64_IRQFLAGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <riscv64.h>

#if !defined(__SANDBOX__)
static inline void arch_local_irq_enable(void)
{
	csr_set(mstatus, MSTATUS_MIE);
}

static inline void arch_local_irq_disable(void)
{
	csr_clear(mstatus, MSTATUS_MIE);
}

static inline irq_flags_t arch_local_irq_save(void)
{
	return csr_read_clear(mstatus, MSTATUS_MIE);
}

static inline void arch_local_irq_restore(irq_flags_t flags)
{
	csr_set(mstatus, flags & MSTATUS_MIE);
}
#else
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
#endif

#define local_irq_enable()			do { arch_local_irq_enable(); } while(0)
#define local_irq_disable()			do { arch_local_irq_disable(); } while(0)
#define local_irq_save(flags)		do { flags = arch_local_irq_save(); } while(0)
#define local_irq_restore(flags)	do { arch_local_irq_restore(flags); } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_IRQFLAGS_H__ */
