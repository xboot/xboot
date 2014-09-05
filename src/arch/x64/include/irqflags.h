#ifndef __X64_IRQFLAGS_H__
#define __X64_IRQFLAGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

/*
 * Enable IRQ
 */
static inline void arch_local_irq_enable(void)
{
}

/*
 * Disable IRQ
 */
static inline void arch_local_irq_disable(void)
{
}

/*
 * Save IRQ flags and disable IRQ
 */
static inline irq_flags_t arch_local_irq_save(void)
{
	irq_flags_t flags;
	return flags;
}

/*
 * Restore IRQ flags
 */
static inline void arch_local_irq_restore(irq_flags_t flags)
{
}

/*
 * Save IRQ flags
 */
static inline irq_flags_t arch_local_save_flags(void)
{
	irq_flags_t flags;
	return flags;
}

#define local_irq_enable()			do { arch_local_irq_enable(); } while(0)
#define local_irq_disable()			do { arch_local_irq_disable(); } while(0)
#define local_irq_save(flags)		do { flags = arch_local_irq_save(); } while(0)
#define local_irq_restore(flags)	do { arch_local_irq_restore(flags); } while(0)
#define local_save_flags(flags)		do { flags = arch_local_save_flags(); } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __X64_IRQFLAGS_H__ */
