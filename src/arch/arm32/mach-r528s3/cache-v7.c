/*
 * cache-v7.c
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

static inline uint32_t get_ccsidr(void)
{
	uint32_t ccsidr;

	__asm__ __volatile__("mrc p15, 1, %0, c0, c0, 0" : "=r" (ccsidr));
	return ccsidr;
}

static inline void __v7_cache_flush_range(uint32_t start, uint32_t stop, uint32_t line)
{
	uint32_t mva;

	start &= ~(line - 1);
	if(stop & (line - 1))
		stop = (stop + line) & ~(line - 1);
	for(mva = start; mva < stop; mva = mva + line)
	{
		__asm__ __volatile__("mcr p15, 0, %0, c7, c14, 1" : : "r" (mva));
	}
}

static inline void __v7_cache_inv_range(uint32_t start, uint32_t stop, uint32_t line)
{
	uint32_t mva;

	start &= ~(line - 1);
	if(stop & (line - 1))
		stop = (stop + line) & ~(line - 1);
	for(mva = start; mva < stop; mva = mva + line)
	{
		__asm__ __volatile__("mcr p15, 0, %0, c7, c6, 1" : : "r" (mva));
	}
}

/*
 * Flush range(clean & invalidate), affects the range [start, stop - 1]
 */
void cache_flush_range(unsigned long start, unsigned long stop)
{
	uint32_t ccsidr;
	uint32_t line;

	ccsidr = get_ccsidr();
	line = ((ccsidr & 0x7) >> 0) + 2;
	line += 2;
	line = 1 << line;
	__v7_cache_flush_range(start, stop, line);
	dsb();
}

/*
 * Invalidate range, affects the range [start, stop - 1]
 */
void cache_inv_range(unsigned long start, unsigned long stop)
{
	uint32_t ccsidr;
	uint32_t line;

	ccsidr = get_ccsidr();
	line = ((ccsidr & 0x7) >> 0) + 2;
	line += 2;
	line = 1 << line;
	__v7_cache_inv_range(start, stop, line);
	dsb();
}

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
