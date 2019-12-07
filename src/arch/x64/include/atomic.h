#ifndef __X64_ATOMIC_H__
#define __X64_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <barrier.h>
#include <irqflags.h>

static inline void atomic_add(atomic_t * a, int v)
{
	__asm__ __volatile__ (
		"lock;\n"
		" addl %k1, %k0\n"
		:"+m"(a->counter)
		:"ir"(v));
}

static inline int atomic_add_return(atomic_t * a, int v)
{
	int tmp;

	__asm__ __volatile__ (
		"lock;\n"
		" xaddl %k0, %k1\n"
		:"=r"(tmp),"+m"(a->counter)
		:"0"(v):"cc");

	return v + tmp;
}

static inline void atomic_sub(atomic_t * a, int v)
{
	__asm__ __volatile__ (
		"lock;\n"
		" subl %k1, %k0\n"
		:"+m"(a->counter)
		:"ir"(v));
}

static inline int atomic_sub_return(atomic_t * a, int v)
{
	return atomic_add_return(a, -v);
}

static inline int atomic_cmp_exchange(atomic_t * a, int o, int n)
{
	int ret;

	__asm__ __volatile__ (
		"lock;\n"
		"cmpxchgl %k2, %k1\n"
		:"=a"(ret),"+m"(a->counter)
		:"r"(n), "0"(o)
		:"memory");
	return ret;
}

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
#define atomic_cmpxchg(a, o, n)		(atomic_cmp_exchange(a, o, n))

#ifdef __cplusplus
}
#endif

#endif /* __X64_ATOMIC_H__ */
