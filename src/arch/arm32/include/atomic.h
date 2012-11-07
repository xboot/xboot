#ifndef __ARM32_ATOMIC_H__
#define __ARM32_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <barrier.h>
#include <irqflags.h>

#if __ARM_ARCH__ >= 6
static inline void atomic_add(long i, atomic_t * v)
{
	unsigned long tmp;
	long result;

	__asm__ __volatile__(
"1:	ldrex	%0, [%3]\n"
"	add	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");
}

static inline long atomic_add_return(long i, atomic_t * v)
{
	unsigned long tmp;
	long result;

	smp_mb();

	__asm__ __volatile__(
"1:	ldrex	%0, [%3]\n"
"	add	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");

	smp_mb();

	return result;
}

static inline void atomic_sub(long i, atomic_t * v)
{
	unsigned long tmp;
	long result;

	__asm__ __volatile__(
"1:	ldrex	%0, [%3]\n"
"	sub	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");
}

static inline long atomic_sub_return(long i, atomic_t * v)
{
	unsigned long tmp;
	long result;

	smp_mb();

	__asm__ __volatile__(
"1:	ldrex	%0, [%3]\n"
"	sub	%0, %0, %4\n"
"	strex	%1, %0, [%3]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp), "+Qo" (v->counter)
	: "r" (&v->counter), "Ir" (i)
	: "cc");

	smp_mb();

	return result;
}
#else
static inline void atomic_add(long i, atomic_t * v)
{
	irq_flags_t flags;
	long val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val += i;
	local_irq_restore(flags);
}

static inline long atomic_add_return(long i, atomic_t * v)
{
	irq_flags_t flags;
	long val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val += i;
	local_irq_restore(flags);

	return val;
}

static inline void atomic_sub(long i, atomic_t * v)
{
	irq_flags_t flags;
	long val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val -= i;
	local_irq_restore(flags);
}

static inline long atomic_sub_return(long i, atomic_t * v)
{
	irq_flags_t flags;
	long val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val -= i;
	local_irq_restore(flags);

	return val;
}
#endif

#define ATOMIC_INIT(i)				{ (i) }

#define atomic_inc(v)				atomic_add(1, v)
#define atomic_dec(v)				atomic_sub(1, v)
#define atomic_inc_return(v)		(atomic_add_return(1, v))
#define atomic_dec_return(v)		(atomic_sub_return(1, v))
#define atomic_inc_and_test(v)		(atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)		(atomic_sub_return(1, v) == 0)
#define atomic_add_negative(i,v)	(atomic_add_return(i, v) < 0)
#define atomic_sub_and_test(i, v)	(atomic_sub_return(i, v) == 0)

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_ATOMIC_H__ */
