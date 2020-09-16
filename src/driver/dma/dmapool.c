/*
 * driver/dma/dmapool.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

static void * __dma_pool = NULL;
static spinlock_t __dma_lock = SPIN_LOCK_INIT();

void * dma_alloc_coherent(unsigned long size)
{
	irq_flags_t flags;
	void * m;

	spin_lock_irqsave(&__dma_lock, flags);
	m = mm_memalign(__dma_pool, SZ_4K, size);
	spin_unlock_irqrestore(&__dma_lock, flags);

	return m;
}

void dma_free_coherent(void * addr)
{
	irq_flags_t flags;

	spin_lock_irqsave(&__dma_lock, flags);
	mm_free(__dma_pool, addr);
	spin_unlock_irqrestore(&__dma_lock, flags);
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

static __init void dma_pure_init(void)
{
	void * dma;
	size_t size;

#ifdef __SANDBOX__
	extern void * sandbox_get_dma_buffer(void);
	extern size_t sandbox_get_dma_size(void);
	dma = sandbox_get_dma_buffer();
	size = sandbox_get_dma_size();
#else
	extern unsigned char __dma_start;
	extern unsigned char __dma_end;
	dma = (void *)&__dma_start;
	size = (size_t)(&__dma_end - &__dma_start);
#endif

	spin_lock_init(&__dma_lock);
	__dma_pool = mm_create(dma, size);
	kobj_add_regular(search_class_memory_kobj(), "dmainfo", memory_read_dmainfo, NULL, mm_get(__dma_pool));
}
pure_initcall(dma_pure_init);
