/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_ATOMIC_H__
#define __CSKY_ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <barrier.h>
#include <irqflags.h>

static inline void atomic_add(atomic_t * a, int v)
{
	mb();
	a->counter += v;
	mb();
}

static inline int atomic_add_return(atomic_t * a, int v)
{
	mb();
	a->counter += v;
	mb();
	return a->counter;
}

static inline void atomic_sub(atomic_t * a, int v)
{
	mb();
	a->counter -= v;
	mb();
}

static inline int atomic_sub_return(atomic_t * a, int v)
{
	mb();
	a->counter -= v;
	mb();
	return a->counter;
}

static inline int atomic_cmp_exchange(atomic_t * a, int o, int n)
{
	volatile int v;

	mb();
	v = a->counter;
	if(v == o)
		a->counter = n;
	mb();
	return v;
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

#endif /* __CSKY_ATOMIC_H__ */
