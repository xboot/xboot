#ifndef __ARM64_SPINLOCK_H__
#define __ARM64_SPINLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <barrier.h>
#include <irqflags.h>
#include <smp.h>

#if defined(CONFIG_MAX_SMP_CPUS) && (CONFIG_MAX_SMP_CPUS > 1)
static inline int arch_spin_trylock(spinlock_t * lock)
{
	unsigned int cpu = smp_processor_id();
	unsigned int tmp;

	__asm__ __volatile__ (
	"ldaxr %w0, %1\n"
	"cmp %w0, %w3\n"
	"b.ne 1f\n"
	"stxr %w0, %w2, %1\n"
	"b 2f\n"
	"1: mov	%w0, #1\n"
	"2:\n"
	: "=&r" (tmp), "+Q" (lock->lock)
	: "r" (cpu), "r" (0xffffffff)
	: "cc", "memory");

	if(tmp == 0)
		return 1;
	else
		return 0;
}

static inline void arch_spin_lock(spinlock_t * lock)
{
	unsigned int cpu = smp_processor_id();
	unsigned long tmp;

	__asm__ __volatile__ (
	"sevl\n"
	"1: wfe\n"
	"2: ldaxr %w0, %1\n"
	"cmp %w0, %w3\n"
	"b.ne 1b\n"
	"stxr %w0, %w2, %1\n"
	"cbnz %w0, 2b\n"
	: "=&r" (tmp), "+Q" (lock->lock)
	: "r" (cpu), "r" (0xffffffff)
	: "cc", "memory");
}

static inline void arch_spin_unlock(spinlock_t * lock)
{
	__asm__ __volatile__ (
	"stlr %w1, %0\n"
	: "=Q" (lock->lock) : "r" (0xffffffff)
	: "memory");
}
#else
static inline int arch_spin_trylock(spinlock_t * lock)
{
	return 0;
}

static inline void arch_spin_lock(spinlock_t * lock)
{
}

static inline void arch_spin_unlock(spinlock_t * lock)
{
}
#endif

#define SPIN_LOCK_INIT()					{ .lock = 0xffffffff }
#define spin_lock_init(plock)				do { (plock)->lock = 0xffffffff; } while(0)
#define spin_trylock(lock)					({ int __ret; __ret = arch_spin_trylock(lock); __ret; })
#define spin_lock(lock)						do { arch_spin_lock(lock); } while(0)
#define spin_unlock(lock)					do { arch_spin_unlock(lock); } while(0)
#define spin_lock_irq(lock)					do { local_irq_disable(); arch_spin_lock(lock); } while(0)
#define spin_unlock_irq(lock)				do { arch_spin_unlock(lock); local_irq_enable(); } while(0)
#define spin_lock_irqsave(lock, flags)		do { local_irq_save(flags); arch_spin_lock(lock); } while(0)
#define spin_unlock_irqrestore(lock, flags)	do { arch_spin_unlock(lock); local_irq_restore(flags); } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __ARM64_SPINLOCK_H__ */
