/*
 * kernel/core/bus.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <spinlock.h>
#include <xboot/bus.h>

struct bus_list_t __bus_list = {
	.entry = {
		.next	= &(__bus_list.entry),
		.prev	= &(__bus_list.entry),
	},
};
static spinlock_t __bus_list_lock = SPIN_LOCK_INIT();

struct bus_t * search_bus(const char * name)
{
	struct bus_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__bus_list.entry), entry)
	{
		if(strcmp(pos->bus->name, name) == 0)
			return pos->bus;
	}

	return NULL;
}

struct bus_t * search_bus_with_type(const char * name, enum bus_type_t type)
{
	struct bus_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__bus_list.entry), entry)
	{
		if(pos->bus->type == type)
		{
			if(strcmp(pos->bus->name, name) == 0)
				return pos->bus;
		}
	}

	return NULL;
}

bool_t register_bus(struct bus_t * bus)
{
	struct bus_list_t * bl;

	if(!bus || !bus->name)
		return FALSE;

	if(!bus->driver)
		return FALSE;

	if(search_bus(bus->name))
		return FALSE;

	bl = malloc(sizeof(struct bus_list_t));
	if(!bl)
		return FALSE;

	bl->bus = bus;

	spin_lock_irq(&__bus_list_lock);
	list_add_tail(&bl->entry, &(__bus_list.entry));
	spin_unlock_irq(&__bus_list_lock);

	return TRUE;
}

bool_t unregister_bus(struct bus_t * bus)
{
	struct bus_list_t * pos, * n;

	if(!bus || !bus->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__bus_list.entry), entry)
	{
		if(pos->bus == bus)
		{
			spin_lock_irq(&__bus_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__bus_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

static s32_t bus_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct bus_list_t * pos, * n;
	char * p;
	int len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), "[bus]");

	list_for_each_entry_safe(pos, n, &(__bus_list.entry), entry)
	{
		len += sprintf((char *)(p + len), "\r\n    %s", pos->bus->name);
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (char *)(p + offset), len);
	free(p);

	return len;
}

static struct proc_t bus_proc = {
	.name	= "bus",
	.read	= bus_proc_read,
};

static __init void bus_pure_sync_init(void)
{
	proc_register(&bus_proc);
}

static __exit void bus_pure_sync_exit(void)
{
	proc_unregister(&bus_proc);
}

core_initcall(bus_pure_sync_init);
core_exitcall(bus_pure_sync_exit);
