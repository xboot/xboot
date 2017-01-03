/*
 * driver/console/console.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <console/console.h>

static ssize_t __console_dummy_read(struct console_t * console, unsigned char * buf, size_t count)
{
	return 0;
}

static ssize_t __console_dummy_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	return 0;
}

static struct console_t __console_dummy = {
	.name	= "console-dummy",
	.read	= __console_dummy_read,
	.write	= __console_dummy_write,
};
static struct console_t * __console = &__console_dummy;
static spinlock_t __console_lock = SPIN_LOCK_INIT();

static ssize_t console_read_active(struct kobj_t * kobj, void * buf, size_t size)
{
	struct console_t * console = (struct console_t *)kobj->priv;

	return sprintf(buf, "%d", (__console == console) ? 1 : 0);
}

static ssize_t console_write_active(struct kobj_t * kobj, void * buf, size_t size)
{
	struct console_t * console = (struct console_t *)kobj->priv;
	irq_flags_t flags;

	spin_lock_irqsave(&__console_lock, flags);
	__console = console;
	spin_unlock_irqrestore(&__console_lock, flags);

	return size;
}

struct console_t * search_console(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_CONSOLE);
	if(!dev)
		return NULL;

	return (struct console_t *)dev->priv;
}

struct console_t * search_first_console(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_CONSOLE);
	if(!dev)
		return NULL;

	return (struct console_t *)dev->priv;
}

bool_t register_console(struct device_t ** device, struct console_t * console)
{
	struct device_t * dev;
	irq_flags_t flags;

	if(!console || !console->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(console->name);
	dev->type = DEVICE_TYPE_CONSOLE;
	dev->priv = console;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "active", console_read_active, console_write_active, console);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(__console == &__console_dummy)
	{
		spin_lock_irqsave(&__console_lock, flags);
		__console = console;
		spin_unlock_irqrestore(&__console_lock, flags);
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_console(struct console_t * console)
{
	struct device_t * dev;
	struct console_t * c;
	irq_flags_t flags;

	if(!console || !console->name)
		return FALSE;

	dev = search_device(console->name, DEVICE_TYPE_CONSOLE);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	if(__console == console)
	{
		if(!(c = search_first_console()))
			c = &__console_dummy;

		spin_lock_irqsave(&__console_lock, flags);
		__console = c;
		spin_unlock_irqrestore(&__console_lock, flags);
	}

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

struct console_t * console_get(void)
{
	return __console;
}

bool_t console_set(const char * name)
{
	struct console_t * c = search_console(name);
	irq_flags_t flags;

	if(c)
	{
		spin_lock_irqsave(&__console_lock, flags);
		__console = c;
		spin_unlock_irqrestore(&__console_lock, flags);
		return TRUE;
	}
	return FALSE;
}

ssize_t console_stdin_read(unsigned char * buf, size_t count)
{
	if(__console && __console->read)
		return __console->read(__console, buf, count);
	return 0;
}

ssize_t console_stdout_write(const unsigned char * buf, size_t count)
{
	if(__console && __console->write)
		return __console->write(__console, buf, count);
	return 0;
}

ssize_t console_stderr_write(const unsigned char * buf, size_t count)
{
	if(__console && __console->write)
		return __console->write(__console, buf, count);
	return 0;
}
