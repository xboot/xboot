/*
 * driver/dma/dma.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <dma/dma.h>

static void * __dma_pool;
static struct mutex_t __dma_mutex;

void * dma_alloc_coherent(unsigned long size)
{
	void * addr;

	mutex_lock(&__dma_mutex);
	addr = mm_memalign(__dma_pool, SZ_4K, size);
	mutex_unlock(&__dma_mutex);
	return addr;
}

void dma_free_coherent(void * addr)
{
	mutex_lock(&__dma_mutex);
	mm_free(__dma_pool, addr);
	mutex_unlock(&__dma_mutex);
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

static __init void dma_pure_init(void)
{
	void * dma;
	size_t size;

#ifdef __SANDBOX__
	static char __dma_buf[SZ_8M];
	dma = (void *)&__dma_buf;
	size = (size_t)(sizeof(__dma_buf));
#else
	extern unsigned char __dma_start;
	extern unsigned char __dma_end;
	dma = (void *)&__dma_start;
	size = (size_t)(&__dma_end - &__dma_start);
#endif

	__dma_pool = mm_create(dma, size);
	mutex_init(&__dma_mutex);
}
pure_initcall(dma_pure_init);
