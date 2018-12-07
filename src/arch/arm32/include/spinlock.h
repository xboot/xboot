#ifndef __ARM32_SPINLOCK_H__
#define __ARM32_SPINLOCK_H__

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
	unsigned long tmp;

	__asm__ __volatile__ (
	"ldrex %0, [%1]\n"
	"teq %0, %3\n"
	"movne %0, #1\n"
	"strexeq %0, %2, [%1]"
	: "=&r" (tmp)
	: "r" (&lock->lock), "r" (cpu), "r" (0xffffffff)
	: "cc");

	if(tmp == 0)
	{
		smp_mb();
		return 1;
	}
	else
	{
		return 0;
	}
}

static inline void arch_spin_lock(spinlock_t * lock)
{
	unsigned int cpu = smp_processor_id();
	unsigned long tmp;

	__asm__ __volatile__ (
	"1: ldrex %0, [%1]\n"
	"teq %0, %3\n"
	"wfene\n"
	"strexeq %0, %2, [%1]\n"
	"teqeq %0, #0\n"
	"bne 1b"
	: "=&r" (tmp)
	: "r" (&lock->lock), "r" (cpu), "r" (0xffffffff)
	: "cc");

	smp_mb();
}

static inline void arch_spin_unlock(spinlock_t * lock)
{
	smp_mb();
	lock->lock = 0xffffffff;
	dsb();
	__asm__ __volatile__ ("sev" : : : "memory");
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

#endif /* __ARM32_SPINLOCK_H__ */
