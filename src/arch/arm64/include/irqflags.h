#ifndef __ARM64_IRQFLAGS_H__
#define __ARM64_IRQFLAGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#if !defined(__SANDBOX__)
static inline void arch_local_irq_enable(void)
{
	__asm__ __volatile__("msr daifclr, #2" ::: "memory");
}

static inline void arch_local_irq_disable(void)
{
	__asm__ __volatile__("msr daifset, #2" ::: "memory");
}

static inline irq_flags_t arch_local_irq_save(void)
{
	irq_flags_t flags;

	__asm__ __volatile__(
		"mrs %0, daif\n"
		"msr daifset, #2"
		: "=r" (flags)
		:
		:"memory", "cc");
	return flags;
}

static inline void arch_local_irq_restore(irq_flags_t flags)
{
	__asm__ __volatile__(
		"msr daif, %0"
		:
		:"r" (flags)
		:"memory", "cc");
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

#endif /* __ARM64_IRQFLAGS_H__ */
