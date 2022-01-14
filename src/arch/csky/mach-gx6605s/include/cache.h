/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_CACHE_H__
#define __CSKY_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sizes.h>
#include <barrier.h>
#include <csky.h>

void icache_inval_all(void);
void dcache_inval_all(void);
void dcache_writeback_all(void);

static inline void cache_flush_range(unsigned long start, unsigned long stop)
{
    icache_inval_all();
    dcache_writeback_all();
}

static inline void cache_inv_range(unsigned long start, unsigned long stop)
{
    icache_inval_all();
    dcache_inval_all();
}

#ifdef __cplusplus
}
#endif

#endif /* __CSKY_CACHE_H__ */
