#ifndef __X64_SPINLOCK_H__
#define __X64_SPINLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <barrier.h>
#include <irqflags.h>

#if defined(CONFIG_MAX_SMP_CPUS) && (CONFIG_MAX_SMP_CPUS > 1) && !defined(__SANDBOX__)
static inline int arch_spin_trylock(spinlock_t * lock)
{
	lock->lock = 1;
	return 1;
}

static inline void arch_spin_lock(spinlock_t * lock)
{
	lock->lock = 1;
}

static inline void arch_spin_unlock(spinlock_t * lock)
{
	lock->lock = 0;
}
#else
static inline int arch_spin_trylock(spinlock_t * lock)
{
	lock->lock = 1;
	return 1;
}

static inline void arch_spin_lock(spinlock_t * lock)
{
	lock->lock = 1;
}

static inline void arch_spin_unlock(spinlock_t * lock)
{
	lock->lock = 0;
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

#endif /* __X64_SPINLOCK_H__ */
