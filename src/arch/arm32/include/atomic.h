#ifndef __ARM32_ATOMIC_H__
#define __ARM32_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <irqflags.h>

#if __ARM32_ARCH__ == 5
static inline void atomic_add(atomic_t * a, long v)
{
	irq_flags_t flags;

	local_irq_save(flags);
	a->counter += v;
	local_irq_restore(flags);
}

static inline long atomic_add_return(atomic_t * a, long v)
{
	irq_flags_t flags;
	long tmp;

	local_irq_save(flags);
	a->counter += v;
	tmp = a->counter;
	local_irq_restore(flags);

	return tmp;
}

static inline void atomic_sub(atomic_t * a, long v)
{
	irq_flags_t flags;

	local_irq_save(flags);
	a->counter -= v;
	local_irq_restore(flags);
}

static inline long atomic_sub_return(atomic_t * a, long v)
{
	irq_flags_t flags;
	long tmp;

	local_irq_save(flags);
	a->counter -= v;
	tmp = a->counter;
	local_irq_restore(flags);

	return tmp;
}
#else
static inline void atomic_add(atomic_t * a, long v)
{
	unsigned int tmp;
	long result;

	__asm__ __volatile__(
"1:	ldrex %0, [%3]\n"
"	add	%0, %0, %4\n"
"	strex %1, %0, [%3]\n"
"	teq %1, #0\n"
"	bne 1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");
}

static inline long atomic_add_return(atomic_t * a, long v)
{
	unsigned int tmp;
	long result;

	__asm__ __volatile__(
"1:	ldrex %0, [%3]\n"
"	add	%0, %0, %4\n"
"	strex %1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");

	return result;
}

static inline void atomic_sub(atomic_t * a, long v)
{
	unsigned int tmp;
	long result;

	__asm__ __volatile__(
"1:	ldrex %0, [%3]\n"
"	sub	%0, %0, %4\n"
"	strex %1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");
}

static inline long atomic_sub_return(atomic_t * a, long v)
{
	unsigned int tmp;
	long result;

	__asm__ __volatile__(
"1:	ldrex %0, [%3]\n"
"	sub	%0, %0, %4\n"
"	strex %1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");

	return result;
}
#endif

#define atomic_set(a, v)			(((a)->counter) = (v))
#define atomic_inc(a)				(atomic_add(a, 1))
#define atomic_dec(a)				(atomic_sub(a, 1))
#define atomic_inc_return(a)		(atomic_add_return(a, 1))
#define atomic_dec_return(a)		(atomic_sub_return(a, 1))
#define atomic_inc_and_test(a)		(atomic_add_return(a, 1) == 0)
#define atomic_dec_and_test(a)		(atomic_sub_return(a, 1) == 0)
#define atomic_add_negative(a, v)	(atomic_add_return(a, 1) < 0)
#define atomic_sub_and_test(a, v)	(atomic_sub_return(a, 1) == 0)

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_ATOMIC_H__ */
