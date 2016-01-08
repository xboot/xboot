/*
 * kernel/core/bus.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/bus.h>

struct bus_list_t __bus_list = {
	.entry = {
		.next	= &(__bus_list.entry),
		.prev	= &(__bus_list.entry),
	},
};
static spinlock_t __bus_lock = SPIN_LOCK_INIT();
static struct notifier_chain_t __bus_nc = NOTIFIER_CHAIN_INIT();

static struct kobj_t * search_bus_kobj(struct bus_t * bus)
{
	struct kobj_t * kbus;
	char * name;

	if(!bus || !bus->kobj)
		return NULL;

	kbus = kobj_search_directory_with_create(kobj_get_root(), "bus");
	if(!kbus)
		return NULL;

	switch(bus->type)
	{
	case BUS_TYPE_W1:
		name = "w1";
		break;
	case BUS_TYPE_UART:
		name = "uart";
		break;
	case BUS_TYPE_I2C:
		name = "i2c";
		break;
	case BUS_TYPE_SPI:
		name = "spi";
		break;
	case BUS_TYPE_CAN:
		name = "can";
		break;
	case BUS_TYPE_USB:
		name = "usb";
		break;
	default:
		return NULL;
	}

	return kobj_search_directory_with_create(kbus, (const char *)name);
}

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
	irq_flags_t flags;

	if(!bus || !bus->name)
		return FALSE;

	if(!bus->driver)
		return FALSE;

	if(search_bus(bus->name))
		return FALSE;

	bl = malloc(sizeof(struct bus_list_t));
	if(!bl)
		return FALSE;

	kobj_add(search_bus_kobj(bus), bus->kobj);
	bl->bus = bus;

	spin_lock_irqsave(&__bus_lock, flags);
	list_add_tail(&bl->entry, &(__bus_list.entry));
	spin_unlock_irqrestore(&__bus_lock, flags);
	notifier_chain_call(&__bus_nc, NOTIFIER_BUS_ADD, bus);

	return TRUE;
}

bool_t unregister_bus(struct bus_t * bus)
{
	struct bus_list_t * pos, * n;
	irq_flags_t flags;

	if(!bus || !bus->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__bus_list.entry), entry)
	{
		if(pos->bus == bus)
		{
			notifier_chain_call(&__bus_nc, NOTIFIER_BUS_REMOVE, bus);
			spin_lock_irqsave(&__bus_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__bus_lock, flags);

			kobj_remove(search_bus_kobj(bus), pos->bus->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t register_bus_notifier(struct notifier_t * n)
{
	return notifier_chain_register(&__bus_nc, n);
}

bool_t unregister_bus_notifier(struct notifier_t * n)
{
	return notifier_chain_unregister(&__bus_nc, n);
}
