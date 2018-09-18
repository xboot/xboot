#ifndef __ARM64_ATOMIC_H__
#define __ARM64_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <irqflags.h>

static inline void atomic_add(atomic_t * a, int v)
{
	unsigned int tmp;
	int result;

	__asm__ __volatile__ (
"1:	ldxr %w0, [%3]\n"
"	add	%w0, %w0, %w4\n"
"	stxr %w1, %w0, [%3]\n"
"	cbnz %w1,1b"
	: "=&r" (result), "=&r" (tmp), "+o" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");
}

static inline long atomic_add_return(atomic_t * a, int v)
{
	unsigned int tmp;
	int result;

	__asm__ __volatile__ (
"1:	ldaxr %w0, [%3]\n"
"	add	%w0, %w0, %w4\n"
"	stlxr %w1, %w0, [%3]\n"
"	cbnz %w1, 1b"
	: "=&r" (result), "=&r" (tmp), "+o" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");

	return result;
}

static inline void atomic_sub(atomic_t * a, int v)
{
	unsigned int tmp;
	int result;

	__asm__ __volatile__ (
"1:	ldxr %w0, [%3]\n"
"	sub	%w0, %w0, %w4\n"
"	stxr %w1, %w0, [%3]\n"
"	cbnz %w1, 1b"
	: "=&r" (result), "=&r" (tmp), "+o" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");
}

static inline int atomic_sub_return(atomic_t * a, int v)
{
	unsigned int tmp;
	int result;

	__asm__ __volatile__ (
"1:	ldaxr %w0, [%3]\n"
"	add	%w0, %w0, %w4\n"
"	stlxr %w1, %w0, [%3]\n"
"	cbnz %w1, 1b"
	: "=&r" (result), "=&r" (tmp), "+o" (a->counter)
	: "r" (&a->counter), "Ir" (v)
	: "cc");

	return result;
}

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

#endif /* __ARM64_ATOMIC_H__ */
