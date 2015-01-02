/*
 * kernel/core/notifier.c
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

#include <xboot/notifier.h>

static bool_t __notifier_register(struct notifier_t ** nl, struct notifier_t * n)
{
	while((*nl) != NULL)
	{
		if((*nl) == n)
			return TRUE;
		if(n->priority > (*nl)->priority)
			break;
		nl = &((*nl)->next);
	}

	n->next = *nl;
	*nl = n;
	return TRUE;
}

static bool_t __notifier_unregister(struct notifier_t ** nl, struct notifier_t * n)
{
	while((*nl) != NULL)
	{
		if((*nl) == n)
		{
			*nl = n->next;
			return TRUE;
		}
		nl = &((*nl)->next);
	}

	return FALSE;
}

static bool_t __notifier_call(struct notifier_t ** nl, int cmd, void * arg)
{
	struct notifier_t * n, * nn;
	int ret = 0;

	n = *nl;
	while(n)
	{
		nn = n->next;
		ret = n->call(n, cmd, arg);
		if(ret != 0)
			break;
		n = nn;
	}

	return (ret == 0) ? TRUE : FALSE;
}

void notifier_chain_init(struct notifier_chain_t * nc)
{
	spin_lock_init(&nc->lock);
	nc->head = NULL;
}

bool_t notifier_chain_register(struct notifier_chain_t * nc, struct notifier_t * n)
{
	irq_flags_t flags;
	bool_t ret;

	spin_lock_irqsave(&nc->lock, flags);
	ret = __notifier_register(&nc->head, n);
	spin_unlock_irqrestore(&nc->lock, flags);

	return ret;
}

bool_t notifier_chain_unregister(struct notifier_chain_t * nc, struct notifier_t * n)
{
	irq_flags_t flags;
	bool_t ret;

	spin_lock_irqsave(&nc->lock, flags);
	ret = __notifier_unregister(&nc->head, n);
	spin_unlock_irqrestore(&nc->lock, flags);

	return ret;
}

bool_t notifier_chain_call(struct notifier_chain_t * nc, int cmd, void * arg)
{
	irq_flags_t flags;
	bool_t ret;

	spin_lock_irqsave(&nc->lock, flags);
	ret = __notifier_call(&nc->head, cmd, arg);
	spin_unlock_irqrestore(&nc->lock, flags);

	return ret;
}
