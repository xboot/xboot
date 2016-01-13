#ifndef __X64_ATOMIC_H__
#define __X64_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <barrier.h>
#include <irqflags.h>

static inline void atomic_add(atomic_t * a, long v)
{
	__asm__ __volatile__ (
		"lock;\n"
		" addl %k1,%k0\n\t"
		:"+m"(a->counter)
		:"ir"(v));
}

static inline long atomic_add_return(atomic_t * a, long v)
{
	long tmp;

	__asm__ __volatile__ (
		"lock;\n"
		" xaddl %k0,%k1\n\t"
		:"=r"(tmp),"+m"(a->counter)
		:"0"(v):"cc");

	return v + tmp;
}

static inline void atomic_sub(atomic_t * a, long v)
{
	__asm__ __volatile__ (
		"lock;\n"
		" subl %k1,%k0\n\t"
		:"+m"(a->counter)
		:"ir"(v));
}

static inline long atomic_sub_return(atomic_t * a, long v)
{
	return atomic_add_return(a, -v);
}

#define atomic_set(a, v)			do { ((a)->counter) = (v); wmb(); } while(0)
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

#endif /* __X64_ATOMIC_H__ */
