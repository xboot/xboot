/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <cache.h>

static inline void cache_line_op(size_t addr, uint32_t val)
{
    mtcr("cr22", addr);
    mtcr("cr17", val);
}

void icache_inval_all(void)
{
    mtcr("cr17", CFR_SELI | CFR_INV);
}

void dcache_inval_all(void)
{
    mtcr("cr17", CFR_SELD | CFR_INV);
}

void dcache_writeback_all(void)
{
    mtcr("cr17", CFR_SELD | CFR_CLR);
}

