/*
 * kernel/core/irq.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <spinlock.h>
#include <xboot/irq.h>

struct irq_list_t
{
	struct irq_t * irq;
	struct list_head entry;
};

struct irq_list_t __irq_list = {
	.entry = {
		.next	= &(__irq_list.entry),
		.prev	= &(__irq_list.entry),
	},
};
static spinlock_t __irq_list_lock = SPIN_LOCK_INIT();

static void null_interrupt_function(void * data)
{
}

static struct irq_t * irq_search(const char * name)
{
	struct irq_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__irq_list.entry), entry)
	{
		if(strcmp(pos->irq->name, name) == 0)
			return pos->irq;
	}

	return NULL;
}

bool_t irq_register(struct irq_t * irq)
{
	struct irq_list_t * il;

	if(!irq || !irq->name)
		return FALSE;

	if(irq_search(irq->name))
		return FALSE;

	il = malloc(sizeof(struct irq_list_t));
	if(!il)
		return FALSE;

	il->irq = irq;

	irq->handler->func = null_interrupt_function;
	irq->handler->data = NULL;

	spin_lock_irq(&__irq_list_lock);
	list_add_tail(&il->entry, &(__irq_list.entry));
	spin_unlock_irq(&__irq_list_lock);

	return TRUE;
}

bool_t irq_unregister(struct irq_t * irq)
{
	struct irq_list_t * pos, * n;

	if(!irq || !irq->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__irq_list.entry), entry)
	{
		if(pos->irq == irq)
		{
			irq->handler->func = null_interrupt_function;
			irq->handler->data = NULL;

			if(pos->irq->enable)
				pos->irq->enable(pos->irq, FALSE);

			spin_lock_irq(&__irq_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__irq_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t request_irq(const char * name, interrupt_function_t func, void * data)
{
	struct irq_t * irq;

	if(!name || !func)
		return FALSE;

	irq = irq_search(name);
	if(!irq)
		return FALSE;

	if(irq->handler->func != null_interrupt_function)
		return FALSE;

	irq->handler->func = func;
	irq->handler->data = data;

	if(irq->enable)
		irq->enable(irq, TRUE);

	return TRUE;
}

bool_t free_irq(const char * name)
{
	struct irq_t * irq;

	if(!name)
		return FALSE;

	irq = irq_search(name);
	if(!irq)
		return FALSE;

	irq->handler->func = null_interrupt_function;
	irq->handler->data = NULL;

	if(irq->enable)
		irq->enable(irq, FALSE);

	return TRUE;
}
