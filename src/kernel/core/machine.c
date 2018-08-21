/*
 * kernel/core/machine.c
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
#include <sha256.h>
#include <watchdog/watchdog.h>
#include <xboot/machine.h>

static struct list_head __machine_list = {
	.next = &__machine_list,
	.prev = &__machine_list,
};
static spinlock_t __machine_lock = SPIN_LOCK_INIT();
static struct machine_t * __machine = NULL;

static const char * __machine_uniqueid(struct machine_t * mach)
{
	const char * id = NULL;

	if(mach && mach->uniqueid)
		id = mach->uniqueid(mach);
	return id ? id : "0123456789";
}

static struct kobj_t * search_class_machine_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "machine");
}

static ssize_t machine_read_description(struct kobj_t * kobj, void * buf, size_t size)
{
	struct machine_t * mach = (struct machine_t *)kobj->priv;
	return sprintf(buf, "%s", mach->desc);
}

static ssize_t machine_read_map(struct kobj_t * kobj, void * buf, size_t size)
{
	struct machine_t * mach = (struct machine_t *)kobj->priv;
	struct mmap_t * m = (struct mmap_t *)mach->map;
	char * p = buf;
	int len = 0;

	while(m->size != 0)
	{
		len += sprintf((char *)(p + len), " %s: %p - %p\r\n", m->name, m->virt, m->phys);
		m++;
	}
	return len;
}

static ssize_t machine_read_uniqueid(struct kobj_t * kobj, void * buf, size_t size)
{
	struct machine_t * mach = (struct machine_t *)kobj->priv;
	return sprintf(buf, "%s", __machine_uniqueid(mach));
}

static struct machine_t * search_machine(const char * name)
{
	struct machine_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &__machine_list, list)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

bool_t register_machine(struct machine_t * mach)
{
	irq_flags_t flags;
	int i;

	if(!mach || !mach->name || !mach->detect)
		return FALSE;

	if(search_machine(mach->name))
		return FALSE;

	mach->kobj = kobj_alloc_directory(mach->name);
	kobj_add_regular(mach->kobj, "description", machine_read_description, NULL, mach);
	kobj_add_regular(mach->kobj, "map", machine_read_map, NULL, mach);
	kobj_add_regular(mach->kobj, "uniqueid", machine_read_uniqueid, NULL, mach);
	kobj_add(search_class_machine_kobj(), mach->kobj);

	spin_lock_irqsave(&__machine_lock, flags);
	init_list_head(&mach->list);
	list_add_tail(&mach->list, &__machine_list);
	spin_unlock_irqrestore(&__machine_lock, flags);

	if(!__machine && (mach->detect(mach) > 0))
	{
		if(mach->memmap)
		{
			mach->memmap(mach);
		}
		if(mach->logger)
		{
			for(i = 0; i < 5; i++)
			{
				mach->logger(mach, xboot_character_logo_string(i), strlen(xboot_character_logo_string(i)));
				mach->logger(mach, "\r\n", 2);
			}
			mach->logger(mach, xboot_banner_string(), strlen(xboot_banner_string()));
			mach->logger(mach, " - [", 4);
			mach->logger(mach, mach->name, strlen(mach->name));
			mach->logger(mach, "][", 2);
			mach->logger(mach, mach->desc, strlen(mach->desc));
			mach->logger(mach, "]\r\n", 3);
		}
		__machine = mach;
	}
	return TRUE;
}

bool_t unregister_machine(struct machine_t * mach)
{
	irq_flags_t flags;

	if(!mach || !mach->name)
		return FALSE;

	spin_lock_irqsave(&__machine_lock, flags);
	list_del(&mach->list);
	spin_unlock_irqrestore(&__machine_lock, flags);
	kobj_remove(search_class_machine_kobj(), mach->kobj);
	kobj_remove_self(mach->kobj);

	return TRUE;
}

inline __attribute__((always_inline)) struct machine_t * get_machine(void)
{
	return __machine;
}

void machine_shutdown(void)
{
	struct machine_t * mach = get_machine();

	sync();
	if(mach && mach->shutdown)
		mach->shutdown(mach);
}

void machine_reboot(void)
{
	struct machine_t * mach = get_machine();

	sync();
	if(mach && mach->reboot)
		mach->reboot(mach);
	watchdog_set_timeout(search_first_watchdog(), 1);
}

void machine_sleep(void)
{
	struct machine_t * mach = get_machine();
	struct device_t * pos, * n;

	sync();
	list_for_each_entry_safe_reverse(pos, n, &__device_list, list)
	{
		suspend_device(pos);
	}
	if(mach && mach->sleep)
	{
		mach->sleep(mach);
	}
	list_for_each_entry_safe(pos, n, &__device_list, list)
	{
		resume_device(pos);
	}
}

void machine_cleanup(void)
{
	struct machine_t * mach = get_machine();

	sync();
	if(mach && mach->cleanup)
		mach->cleanup(mach);
}

int machine_logger(const char * fmt, ...)
{
	struct machine_t * mach = get_machine();
	struct timeval tv;
	char buf[SZ_4K];
	int len = 0;
	va_list ap;

	if(mach && mach->logger)
	{
		va_start(ap, fmt);
		gettimeofday(&tv, 0);
		len += sprintf((char *)(buf + len), "[%5u.%06u]", tv.tv_sec, tv.tv_usec);
		len += vsnprintf((char *)(buf + len), (SZ_4K - len), fmt, ap);
		va_end(ap);
		mach->logger(mach, (const char *)buf, len);
	}
	return len;
}

const char * machine_uniqueid(void)
{
	struct machine_t * mach = get_machine();
	return __machine_uniqueid(mach);
}

int machine_keygen(const char * msg, void * key)
{
	struct machine_t * mach = get_machine();
	int len;

	if(mach && mach->keygen && ((len = mach->keygen(mach, msg, key)) > 0))
		return len;
	sha256_hash(msg, strlen(msg), key);
	return 32;
}

static virtual_addr_t __phys_to_virt(physical_addr_t phys)
{
	struct machine_t * mach = get_machine();
	struct mmap_t * m;

	if(mach && (m = (struct mmap_t *)mach->map))
	{
		while(m->size > 0)
		{
			if((phys >= m->phys) && (phys <= m->phys + m->size - 1))
				return (virtual_addr_t)(m->virt + (phys - m->phys));
			m++;
		}
	}
	return (virtual_addr_t)phys;
}
extern __typeof(__phys_to_virt) phys_to_virt __attribute__((weak, alias("__phys_to_virt")));

static physical_addr_t __virt_to_phys(virtual_addr_t virt)
{
	struct machine_t * mach = get_machine();
	struct mmap_t * m;

	if(mach && (m = (struct mmap_t *)mach->map))
	{
		while(m->size > 0)
		{
			if((virt >= m->virt) && (virt <= m->virt + m->size - 1))
				return (physical_addr_t)(m->phys + (virt - m->virt));
			m++;
		}
	}
	return (physical_addr_t)virt;
}
extern __typeof(__virt_to_phys) virt_to_phys __attribute__((weak, alias("__virt_to_phys")));
