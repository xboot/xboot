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

static struct kobj_t * search_class_memory_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "memory");
}

static ssize_t memory_read_dmainfo(struct kobj_t * kobj, void * buf, size_t size)
{
	void * mm = (void *)kobj->priv;
	size_t mused, mfree;
	char * p = buf;
	int len = 0;

	mm_info(mm, &mused, &mfree);
	len += sprintf((char *)(p + len), " dma used: %ld\r\n", mused);
	len += sprintf((char *)(p + len), " dma free: %ld\r\n", mfree);
	return len;
}

void do_init_dma_pool(void)
{
#ifndef __SANDBOX__
	extern unsigned char __dma_start;
	extern unsigned char __dma_end;
	__dma_pool = mm_create((void *)&__dma_start, (size_t)(&__dma_end - &__dma_start));
#else
	static char __dma_buf[SZ_8M];
	__dma_pool = mm_create((void *)__dma_buf, (size_t)(sizeof(__dma_buf)));
#endif
	kobj_add_regular(search_class_memory_kobj(), "dmainfo", memory_read_dmainfo, NULL, mm_get(__dma_pool));
}
