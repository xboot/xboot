/*
 * cache-c906.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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

#define L1_CACHE_BYTES	(64)

void dcache_wb_range(unsigned long start, unsigned long end)
{
	unsigned long i = start & ~(L1_CACHE_BYTES - 1);

	for(; i < end; i += L1_CACHE_BYTES)
		__asm__ __volatile__("dcache.cpa %0\n"::"r"(i):"memory");
	__asm__ __volatile__("sync.is");
}

void dcache_inv_range(unsigned long start, unsigned long end)
{
	unsigned long i = start & ~(L1_CACHE_BYTES - 1);

	for(; i < end; i += L1_CACHE_BYTES)
		__asm__ __volatile__("dcache.ipa %0\n"::"r"(i):"memory");
	__asm__ __volatile__("sync.is");
}

void dcache_wbinv_range(unsigned long start, unsigned long end)
{
	unsigned long i = start & ~(L1_CACHE_BYTES - 1);

	for(; i < end; i += L1_CACHE_BYTES)
		__asm__ __volatile__("dcache.cipa %0\n"::"r"(i):"memory");
	__asm__ __volatile__("sync.is");
}
