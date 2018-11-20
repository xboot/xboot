#ifndef __RISCV64_ATOMIC_H__
#define __RISCV64_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <barrier.h>
#include <irqflags.h>

#ifdef __riscv_atomic
static inline void atomic_add(atomic_t * a, int v)
{
	__asm__ __volatile__ (
		"amoadd.w zero, %1, %0"
		: "+A" (a->counter)
		: "r" (v)
		: "memory");
}

static inline int atomic_add_return(atomic_t * a, int v)
{
	int ret;
	__asm__ __volatile__ (
		"amoadd.w.aqrl %1, %2, %0"
		: "+A" (a->counter), "=r" (ret)
		: "r" (v)
		: "memory");
	return ret + v;
}

static inline void atomic_sub(atomic_t * a, int v)
{
	__asm__ __volatile__ (
		"amoadd.w zero, %1, %0"
		: "+A" (a->counter)
		: "r" (-v)
		: "memory");
}

static inline int atomic_sub_return(atomic_t * a, int v)
{
	int ret;
	__asm__ __volatile__ (
		"amoadd.w.aqrl %1, %2, %0"
		: "+A" (a->counter), "=r" (ret)
		: "r" (-v)
		: "memory");
	return ret - v;
}
#else
static inline void atomic_add(atomic_t * a, int v)
{
	irq_flags_t flags;

	local_irq_save(flags);
	(volatile)a->counter += v;
	local_irq_restore(flags);
}

static inline int atomic_add_return(atomic_t * a, int v)
{
	irq_flags_t flags;
	int tmp;

	local_irq_save(flags);
	(volatile)a->counter += v;
	tmp = a->counter;
	local_irq_restore(flags);
	return tmp;
}

static inline void atomic_sub(atomic_t * a, int v)
{
	irq_flags_t flags;

	local_irq_save(flags);
	(volatile)a->counter -= v;
	local_irq_restore(flags);
}

static inline int atomic_sub_return(atomic_t * a, int v)
{
	irq_flags_t flags;
	int tmp;

	local_irq_save(flags);
	(volatile)a->counter -= v;
	tmp = a->counter;
	local_irq_restore(flags);
	return tmp;
}
#endif

#define atomic_set(a, v)			do { ((a)->counter) = (v); smp_wmb(); } while(0)
#define atomic_get(a)				({ int __v; __v = (a)->counter; smp_rmb(); __v; })
#define atomic_inc(a)				(atomic_add(a, 1))
#define atomic_dec(a)				(atomic_sub(a, 1))
#define atomic_inc_return(a)		(atomic_add_return(a, 1))
#define atomic_dec_return(a)		(atomic_sub_return(a, 1))
#define atomic_inc_and_test(a)		(atomic_add_return(a, 1) == 0)
#define atomic_dec_and_test(a)		(atomic_sub_return(a, 1) == 0)
#define atomic_add_negative(a, v)	(atomic_add_return(a, v) < 0)
#define atomic_sub_and_test(a, v)	(atomic_sub_return(a, v) == 0)

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_ATOMIC_H__ */
