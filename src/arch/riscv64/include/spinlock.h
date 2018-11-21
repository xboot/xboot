#ifndef __RISCV64_SPINLOCK_H__
#define __RISCV64_SPINLOCK_H__

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
	int tmp = 1, busy;

	__asm__ __volatile__ (
		"amoswap.w %0, %2, %1\n"
		"fence r, rw\n"
		: "=r" (busy), "+A" (lock->lock)
		: "r" (tmp)
		: "memory");

	return !busy;
}

static inline void arch_spin_lock(spinlock_t * lock)
{
	while(1)
	{
		smp_mb();
		if((lock->lock == 0) ? 0 : 1)
			continue;
		if(arch_spin_trylock(lock))
			break;
	}
}

static inline void arch_spin_unlock(spinlock_t * lock)
{
	__asm__ __volatile__ ("fence rw, w" : : : "memory");
	lock->lock = 0;
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

#define SPIN_LOCK_INIT()					{ .lock = 0 }
#define spin_lock_init(plock)				do { (plock)->lock = 0; } while(0)
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

#endif /* __RISCV64_SPINLOCK_H__ */
