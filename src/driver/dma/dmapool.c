/*
 * driver/dma/dmapool.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <dma/dmapool.h>

static void * __dma_alloc_coherent(unsigned long size)
{
	return memalign(SZ_4K, size);
}
extern __typeof(__dma_alloc_coherent) dma_alloc_coherent __attribute__((weak, alias("__dma_alloc_coherent")));

static void __dma_free_coherent(void * addr)
{
	free(addr);
}
extern __typeof(__dma_free_coherent) dma_free_coherent __attribute__((weak, alias("__dma_free_coherent")));

static void * __dma_alloc_noncoherent(unsigned long size)
{
	return memalign(SZ_4K, size);
}
extern __typeof(__dma_alloc_noncoherent) dma_alloc_noncoherent __attribute__((weak, alias("__dma_alloc_noncoherent")));

static void __dma_free_noncoherent(void * addr)
{
	free(addr);
}
extern __typeof(__dma_free_noncoherent) dma_free_noncoherent __attribute__((weak, alias("__dma_free_noncoherent")));

static void __dma_cache_sync(void * addr, unsigned long size, int dir)
{
}
extern __typeof(__dma_cache_sync) dma_cache_sync __attribute__((weak, alias("__dma_cache_sync")));

