/*
 * driver/dma/dma.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <dma/dma.h>

static void * __dma_pool = NULL;

void * dma_alloc_coherent(unsigned long size)
{
	return mm_memalign(__dma_pool, SZ_4K, size);
}

void dma_free_coherent(void * addr)
{
	mm_free(__dma_pool, addr);
}

void * dma_alloc_noncoherent(unsigned long size)
{
	return memalign(SZ_4K, size);
}

void dma_free_noncoherent(void * addr)
{
	free(addr);
}

static void __dma_cache_sync(void * addr, unsigned long size, int dir)
{
}
extern __typeof(__dma_cache_sync) dma_cache_sync __attribute__((weak, alias("__dma_cache_sync")));

void do_init_dma_pool(void)
{
#ifndef __SANDBOX__
	extern unsigned char __dma_start;
	extern unsigned char __dma_end;
	__dma_pool = mm_create((void *)&__dma_start, (size_t)(&__dma_end - &__dma_start));
#else
	static char __dma_buf[SZ_64M];
	__dma_pool = mm_create((void *)__dma_buf, (size_t)(sizeof(__dma_buf)));
#endif
}
