/*
 * cache.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

static void __outer_cache_enable(void)
{
}
extern __typeof(__outer_cache_enable) outer_cache_enable __attribute__((weak, alias("__outer_cache_enable")));

static void __outer_cache_disable(void)
{
}
extern __typeof(__outer_cache_disable) outer_cache_disable __attribute__((weak, alias("__outer_cache_disable")));

static void __outer_cache_flush_range(unsigned long start, unsigned long stop)
{
}
extern __typeof(__outer_cache_flush_range) outer_cache_flush_range __attribute__((weak, alias("__outer_cache_flush_range")));

static void __outer_cache_inv_range(unsigned long start, unsigned long stop)
{
}
extern __typeof(__outer_cache_inv_range) outer_cache_inv_range __attribute__((weak, alias("__outer_cache_inv_range")));

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
