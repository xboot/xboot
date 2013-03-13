/*
 * kernel/core/irq.c
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
#include <sizes.h>
#include <types.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <stdio.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <xboot/irq.h>


/*
 * the hash list of irq
 */
static struct hlist_head irq_hash[CONFIG_IRQ_HASH_SIZE] = {{0}};

/*
 * null function for irq handler
 */
static void null_irq_handler(void)	{ }

/*
 * search irq by name. a static function.
 */
static struct irq_list * irq_search(const char *name)
{
	struct irq_list * list;
	struct hlist_node * pos;
	u32_t hash;

	if(!name)
		return NULL;

	hash = string_hash(name) % CONFIG_IRQ_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(irq_hash[hash]), node)
	{
		if(strcmp(list->irq->name, name) == 0)
			return list;
	}

	return NULL;
}

/*
 * register a irq into irq_list
 * return true is successed, otherwise is not.
 */
bool_t irq_register(struct irq * irq)
{
	struct irq_list * list;
	u32_t hash;

	list = malloc(sizeof(struct irq_list));
	if(!list || !irq)
	{
		free(list);
		return FALSE;
	}

	if(!irq->name || irq_search(irq->name))
	{
		free(list);
		return FALSE;
	}

	list->irq = irq;
	list->busy = FALSE;
	*(list->irq->handler) = (irq_handler)null_irq_handler;

	hash = string_hash(irq->name) % CONFIG_IRQ_HASH_SIZE;
	hlist_add_head(&(list->node), &(irq_hash[hash]));

	return TRUE;
}

/*
 * unregister irq from irq_list
 */
bool_t irq_unregister(struct irq * irq)
{
	struct irq_list * list;
	struct hlist_node * pos;
	u32_t hash;

	if(!irq || !irq->name)
		return FALSE;

	hash = string_hash(irq->name) % CONFIG_IRQ_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(irq_hash[hash]), node)
	{
		if(list->irq == irq)
		{
			if(list->irq->enable)
				list->irq->enable(list->irq, FALSE);
			hlist_del(&(list->node));
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * request irq
 */
bool_t request_irq(const char *name, irq_handler handler)
{
	struct irq_list *list;

	if(!name || !handler)
		return FALSE;

	list = irq_search(name);
	if(!list)
		return FALSE;

	if(list->busy)
		return FALSE;

	list->busy = TRUE;
	*(list->irq->handler) = handler;
	if(list->irq->enable)
		list->irq->enable(list->irq, TRUE);

	return TRUE;
}

/*
 * free irq
 */
bool_t free_irq(const char *name)
{
	struct irq_list *list;

	if(!name)
		return FALSE;

	list = irq_search(name);
	if(!list)
		return FALSE;

	list->busy = FALSE;
	*(list->irq->handler) = (irq_handler)null_irq_handler;
	if(list->irq->enable)
		list->irq->enable(list->irq, FALSE);

	return TRUE;
}

/*
 * interrupt proc interface
 */
static s32_t interrupt_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct irq_list * list;
	struct hlist_node * pos;
	s8_t * p;
	s32_t i, len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)"[interrupt]");
	for(i = 0; i < CONFIG_IRQ_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(irq_hash[i]), node)
		{
			if(list->busy)
				len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%3ld used", list->irq->name, (int)(16 - strlen((char *)list->irq->name)), "", list->irq->irq_no);
			else
				len += sprintf((char *)(p + len), (const char *)"\r\n %s%*s%3ld", list->irq->name, (int)(16 - strlen((char *)list->irq->name)), "", list->irq->irq_no);
		}
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (u8_t *)(p + offset), len);
	free(p);

	return len;
}

static struct proc interrupt_proc = {
	.name	= "interrupt",
	.read	= interrupt_proc_read,
};

/*
 * interrupt pure sync init
 */
static __init void interrupt_pure_sync_init(void)
{
	s32_t i;

	/* initialize interrupt hash list */
	for(i = 0; i < CONFIG_IRQ_HASH_SIZE; i++)
		init_hlist_head(&irq_hash[i]);

	/* register interrupt proc interface */
	proc_register(&interrupt_proc);
}

static __exit void interrupt_pure_sync_exit(void)
{
	/* unregister interrupt proc interface */
	proc_unregister(&interrupt_proc);
}

pure_initcall_sync(interrupt_pure_sync_init);
pure_exitcall_sync(interrupt_pure_sync_exit);
