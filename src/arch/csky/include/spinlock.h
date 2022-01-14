/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_SPINLOCK_H__
#define __CSKY_SPINLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <smp.h>
#include <barrier.h>
#include <irqflags.h>

static inline int arch_spin_trylock(spinlock_t * lock)
{
	int flag = 0;

	mb();
	if(lock->lock == 0xffffffff)
	{
		lock->lock = 0;
		flag = 1;
	}
	mb();
	return flag;
}

static inline void arch_spin_lock(spinlock_t * lock)
{
	mb();
	while(lock->lock != 0xffffffff);
	mb();
}

static inline void arch_spin_unlock(spinlock_t * lock)
{
	mb();
	lock->lock = 0xffffffff;
	mb();
}

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

#endif /* __CSKY_SPINLOCK_H__ */
