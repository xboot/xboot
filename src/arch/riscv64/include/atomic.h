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

#define __cmpxchg(ptr, old, new, size)						\
({															\
	__typeof__(ptr) __ptr = (ptr);							\
	__typeof__(*(ptr)) __old = (old);						\
	__typeof__(*(ptr)) __new = (new);						\
	__typeof__(*(ptr)) __ret;								\
	register unsigned int __rc;								\
	switch (size) {											\
	case 4:													\
		__asm__ __volatile__ (								\
			"0:	lr.w %0, %2\n"								\
			"bne  %0, %z3, 1f\n"							\
			"sc.w.rl %1, %z4, %2\n"							\
			"bnez %1, 0b\n"									\
			"fence rw, rw\n"								\
			"1:\n"											\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)					\
			: "memory");									\
		break;												\
	case 8:													\
		__asm__ __volatile__ (								\
			"0:	lr.d %0, %2\n"								\
			"bne %0, %z3, 1f\n"								\
			"sc.d.rl %1, %z4, %2\n"							\
			"bnez %1, 0b\n"									\
			"fence rw, rw\n"								\
			"1:\n"											\
			: "=&r" (__ret), "=&r" (__rc), "+A" (*__ptr)	\
			: "rJ" (__old), "rJ" (__new)					\
			: "memory");									\
		break;												\
	default:												\
		break;												\
	}														\
	__ret;													\
})

#define cmpxchg(ptr, o, n)									\
({															\
	__typeof__(*(ptr)) _o_ = (o);							\
	__typeof__(*(ptr)) _n_ = (n);							\
	(__typeof__(*(ptr))) __cmpxchg((ptr),					\
			_o_, _n_, sizeof(*(ptr)));						\
})

static inline int atomic_cmp_exchange(atomic_t * a, int o, int n)
{
	return cmpxchg(&a->counter, o, n);
}
#else
static inline void atomic_add(atomic_t * a, int v)
{
	a->counter += v;
}

static inline int atomic_add_return(atomic_t * a, int v)
{
	a->counter += v;
	return a->counter;
}

static inline void atomic_sub(atomic_t * a, int v)
{
	a->counter -= v;
}

static inline int atomic_sub_return(atomic_t * a, int v)
{
	a->counter -= v;
	return a->counter;
}

static inline int atomic_cmp_exchange(atomic_t * a, int o, int n)
{
	volatile int v;

	v = a->counter;
	if(v == o)
		a->counter = n;
	return v;
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
#define atomic_cmpxchg(a, o, n)		(atomic_cmp_exchange(a, o, n))

#ifdef __cplusplus
}
#endif

#endif /* __RISCV64_ATOMIC_H__ */
