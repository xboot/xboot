/*
 * kernel/core/setting.c
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
#include <xboot/setting.h>

struct setting_t {
	struct timer_t timer;
	struct hmap_t * map;
	char * path;
	int dirty;
	spinlock_t lock;
};
static struct setting_t __setting = { 0 };

void setting_set(const char * key, const char * value)
{
	irq_flags_t flags;
	char * v;

	spin_lock_irqsave(&__setting.lock, flags);
	v = hmap_search(__setting.map, key);
	if(v)
	{
		__setting.dirty = 1;
		hmap_remove(__setting.map, key);
		free(v);
	}
	if(value)
	{
		__setting.dirty = 1;
		hmap_add(__setting.map, key, strdup(value));
	}
	if(__setting.dirty)
		timer_start_now(&__setting.timer, ms_to_ktime(4000));
	spin_unlock_irqrestore(&__setting.lock, flags);
}

const char * setting_get(const char * key, const char * def)
{
	irq_flags_t flags;
	const char * v;

	spin_lock_irqsave(&__setting.lock, flags);
	v = hmap_search(__setting.map, key);
	spin_unlock_irqrestore(&__setting.lock, flags);
	if(!v)
		v = def;
	return v;
}

static void hmap_entry_callback(struct hmap_entry_t * e)
{
	if(e)
		free(e->value);
}

void setting_clear(void)
{
	irq_flags_t flags;

	spin_lock_irqsave(&__setting.lock, flags);
	hmap_clear(__setting.map, hmap_entry_callback);
	__setting.dirty = 1;
	timer_start_now(&__setting.timer, ms_to_ktime(4000));
	spin_unlock_irqrestore(&__setting.lock, flags);
}

void setting_summary(void)
{
	struct hmap_entry_t * e;

	hmap_sort(__setting.map);
	hmap_for_each_entry(e, __setting.map)
	{
		printf("%s = %s\r\n", e->key, e->value);
	}
}

static int setting_timer_function(struct timer_t * timer, void * data)
{
	struct hmap_entry_t * e;
	char buf[256];
	int fd, len;
	irq_flags_t flags;

	if(__setting.dirty)
	{
		spin_lock_irqsave(&__setting.lock, flags);
		hmap_sort(__setting.map);
		fd = vfs_open(__setting.path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if(fd)
		{
			hmap_for_each_entry(e, __setting.map)
			{
				len = sprintf(buf, "%s=%s;\r\n", e->key, e->value);
				vfs_write(fd, buf, len);
			}
			vfs_close(fd);
		}
		__setting.dirty = 0;
		spin_unlock_irqrestore(&__setting.lock, flags);
	}
	return 0;
}

void do_init_setting(void)
{
	struct vfs_stat_t st;
	char * buf, * p, * r, * k, * v;
	int fd, n, len = 0;
	irq_flags_t flags;

	__setting.map = hmap_alloc(0);
	__setting.path = "/private/setting.cfg";
	__setting.dirty = 0;
	timer_init(&__setting.timer, setting_timer_function, NULL);
	spin_lock_init(&__setting.lock);

	spin_lock_irqsave(&__setting.lock, flags);
	if((vfs_stat(__setting.path, &st) >= 0) && S_ISREG(st.st_mode) && (st.st_size > 0))
	{
		buf = malloc(st.st_size + 1);
		if(buf)
		{
			if((fd = vfs_open(__setting.path, O_RDONLY, 0)) >= 0)
			{
				for(;;)
				{
					n = vfs_read(fd, (void *)(buf + len), SZ_64K);
					if(n <= 0)
						break;
					len += n;
				}
				vfs_close(fd);
				buf[len] = 0;
				p = buf;
				while((r = strsep(&p, ";\r\n")) != NULL)
				{
					if(strchr(r, '='))
					{
						k = strim(strsep(&r, "="));
						v = strim(r);
						k = (k && strcmp(k, "") != 0) ? k : NULL;
						v = (v && strcmp(v, "") != 0) ? v : NULL;
						if(k && v)
							hmap_add(__setting.map, k, strdup(v));
					}
				}
			}
			free(buf);
		}
	}
	spin_unlock_irqrestore(&__setting.lock, flags);
}
