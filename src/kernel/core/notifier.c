/*
 * kernel/core/notifier.c
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
		if(ret < 0)
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
