/*
 * dmapool.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <cache.h>

extern unsigned char __dma_start[];
extern unsigned char __dma_end[];

static void * __dma_pool = NULL;
static spinlock_t __dma_lock = SPIN_LOCK_INIT();

void * dma_alloc_coherent(unsigned long size)
{
	irq_flags_t flags;
	void * m;

	if(!__dma_pool)
		__dma_pool = mm_create((void *)__dma_start, (size_t)(__dma_end - __dma_start));
	if(__dma_pool)
	{
		spin_lock_irqsave(&__dma_lock, flags);
		m = mm_memalign(__dma_pool, SZ_4K, size);
		spin_unlock_irqrestore(&__dma_lock, flags);
		return m;
	}
	return NULL;
}

void dma_free_coherent(void * addr)
{
	irq_flags_t flags;

	if(__dma_pool)
	{
		spin_lock_irqsave(&__dma_lock, flags);
		mm_free(__dma_pool, addr);
		spin_unlock_irqrestore(&__dma_lock, flags);
	}
}

void dma_cache_sync(void * addr, unsigned long size, int dir)
{
	unsigned long start = (unsigned long)addr;
	unsigned long stop = start + size;

	if(dir == DMA_FROM_DEVICE)
	{
		cache_inv_range(start, stop);
		outer_cache_inv_range(start, stop);
	}
	else
	{
		cache_flush_range(start, stop);
		outer_cache_flush_range(start, stop);
	}
}
