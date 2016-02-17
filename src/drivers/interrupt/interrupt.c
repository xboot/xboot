/*
 * driver/interrupt/interrupt.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <interrupt/interrupt.h>

struct irqchip_list_t
{
	struct irqchip_t * chip;
	struct list_head entry;
};

struct irqchip_list_t __irqchip_list = {
	.entry = {
		.next	= &(__irqchip_list.entry),
		.prev	= &(__irqchip_list.entry),
	},
};
static spinlock_t __irqchip_list_lock = SPIN_LOCK_INIT();

static void null_interrupt_function(void * data)
{
}

static struct kobj_t * search_class_irqchip_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "irqchip");
}

static ssize_t irqchip_read_base(struct kobj_t * kobj, void * buf, size_t size)
{
	struct irqchip_t * chip = (struct irqchip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->base);
}

static ssize_t irqchip_read_nirq(struct kobj_t * kobj, void * buf, size_t size)
{
	struct irqchip_t * chip = (struct irqchip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->nirq);
}

static struct irqchip_t * search_irqchip_with_no(int irq)
{
	struct irqchip_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__irqchip_list.entry), entry)
	{
		if( (irq >= pos->chip->base) && (irq < (pos->chip->base + pos->chip->nirq)) )
			return pos->chip;
	}

	return NULL;
}

static struct irqchip_t * search_irqchip(const char * name)
{
	struct irqchip_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__irqchip_list.entry), entry)
	{
		if(strcmp(pos->chip->name, name) == 0)
			return pos->chip;
	}

	return NULL;
}

bool_t register_irqchip(struct irqchip_t * chip)
{
	struct irqchip_list_t * il;
	irq_flags_t flags;
	int i;

	if(!chip || !chip->name)
		return FALSE;

	if(search_irqchip(chip->name))
		return FALSE;

	il = malloc(sizeof(struct irqchip_list_t));
	if(!il)
		return FALSE;

	for(i = 0; i < chip->nirq; i++)
	{
		chip->handler[i].func = null_interrupt_function;
		chip->handler[i].data = NULL;
		if(chip->settype)
			chip->settype(chip, i, IRQ_TYPE_NONE);
		if(chip->disable)
			chip->disable(chip, i);
	}
	chip->kobj = kobj_alloc_directory(chip->name);
	kobj_add_regular(chip->kobj, "base", irqchip_read_base, NULL, chip);
	kobj_add_regular(chip->kobj, "nirq", irqchip_read_nirq, NULL, chip);
	kobj_add(search_class_irqchip_kobj(), chip->kobj);
	il->chip = chip;

	spin_lock_irqsave(&__irqchip_list_lock, flags);
	list_add_tail(&il->entry, &(__irqchip_list.entry));
	spin_unlock_irqrestore(&__irqchip_list_lock, flags);

	return TRUE;
}

bool_t unregister_irqchip(struct irqchip_t * chip)
{
	struct irqchip_list_t * pos, * n;
	irq_flags_t flags;
	int i;

	if(!chip || !chip->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__irqchip_list.entry), entry)
	{
		if(pos->chip == chip)
		{
			for(i = 0; i < chip->nirq; i++)
			{
				chip->handler[i].func = null_interrupt_function;
				chip->handler[i].data = NULL;
				if(chip->settype)
					chip->settype(chip, i, IRQ_TYPE_NONE);
				if(chip->disable)
					chip->disable(chip, i);
			}

			spin_lock_irqsave(&__irqchip_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__irqchip_list_lock, flags);

			kobj_remove(search_class_irqchip_kobj(), pos->chip->kobj);
			kobj_remove_self(chip->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t register_sub_irqchip(int parent, struct irqchip_t * chip)
{
	if(!chip || !chip->name)
		return FALSE;

	if(search_irqchip(chip->name))
		return FALSE;

	if(!request_irq(parent, (void (*)(void *))(chip->process), IRQ_TYPE_NONE, chip))
		return FALSE;

	chip->process = NULL;
	return register_irqchip(chip);
}

bool_t unregister_sub_irqchip(int parent, struct irqchip_t * chip)
{
	if(!chip || !chip->name)
		return FALSE;

	if(!free_irq(parent))
		return FALSE;

	return unregister_irqchip(chip);
}

bool_t request_irq(int irq, void (*func)(void *), enum irq_type_t type, void * data)
{
	struct irqchip_t * chip;
	int offset;

	if(!func)
		return FALSE;

	chip = search_irqchip_with_no(irq);
	if(!chip)
		return FALSE;

	offset = irq - chip->base;
	if(chip->handler[offset].func != null_interrupt_function)
		return FALSE;

	chip->handler[offset].func = func;
	chip->handler[offset].data = data;
	if(chip->settype)
		chip->settype(chip, offset, type);
	if(chip->enable)
		chip->enable(chip, offset);

	return TRUE;
}

bool_t free_irq(int irq)
{
	struct irqchip_t * chip;
	int offset;

	chip = search_irqchip_with_no(irq);
	if(!chip)
		return FALSE;

	offset = irq - chip->base;
	if(chip->handler[offset].func == null_interrupt_function)
		return FALSE;

	chip->handler[offset].func = null_interrupt_function;
	chip->handler[offset].data = NULL;
	if(chip->settype)
		chip->settype(chip, offset, IRQ_TYPE_NONE);
	if(chip->enable)
		chip->enable(chip, offset);

	return TRUE;
}

void enable_irq(int irq)
{
	struct irqchip_t * chip = search_irqchip_with_no(irq);

	if(chip && chip->enable)
		chip->enable(chip, irq - chip->base);
}

void disable_irq(int irq)
{
	struct irqchip_t * chip = search_irqchip_with_no(irq);

	if(chip && chip->disable)
		chip->disable(chip, irq - chip->base);
}

void interrupt_handle_exception(void * regs)
{
	struct irqchip_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__irqchip_list.entry), entry)
	{
		if(pos->chip->process)
			pos->chip->process(pos->chip);
	}
}
