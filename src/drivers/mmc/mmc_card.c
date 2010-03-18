/*
 * driver/mmc/mmc_card.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <vsprintf.h>
#include <xboot/initcall.h>
#include <xboot/proc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>

extern struct mmc_host_list * mmc_host_list;

/*
 * the list of mmc card
 */
static struct mmc_card_list __mmc_card_list = {
	.entry = {
		.next	= &(__mmc_card_list.entry),
		.prev	= &(__mmc_card_list.entry),
	},
};
struct mmc_card_list * mmc_card_list = &__mmc_card_list;

static x_bool register_mmc_card(struct mmc_card * card)
{
	struct mmc_card_list * list;

	list = malloc(sizeof(struct mmc_card_list));
	if(!list || !card)
	{
		free(list);
		return FALSE;
	}

	if(!card->name || search_mmc_card(card->name))
	{
		free(list);
		return FALSE;
	}

	list->card = card;
	list_add(&list->entry, &mmc_card_list->entry);

	return TRUE;
}

/*
 * probe all mmc card
 */
void mmc_card_probe(void)
{
	struct mmc_host_list * list;
	struct list_head * pos;
	struct mmc_host * host;
	struct mmc_card_info * info;
	struct mmc_card * card;
	x_s32 i;

	/*
	 * remove all mmc card
	 */
	mmc_card_remove();

	/*
	 * probe all mmc card by mmc host controller
	 */
	for(pos = (&mmc_host_list->entry)->next; pos != (&mmc_host_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct mmc_host_list, entry);
		host = list->host;

		if(!host)
			continue;

		/*
		 * malloc mmc card information buffer.
		 */
		info = malloc(sizeof(struct mmc_card_info));
		if(!info)
		{
			LOG_E("can not malloc buffer for mmc card information");
			continue;
		}

		/*
		 * initialize mmc host controller
		 */
		if(host->init)
			host->init();

		if(!host->probe(info))
		{
			if(host->exit)
				host->exit();

			free(info);

			continue;
		}

		/*
		 * malloc mmc card buffer.
		 */
		card = malloc(sizeof(struct mmc_card));
		if(!card)
		{
			LOG_E("can not malloc buffer for mmc card");

			if(host->exit)
				host->exit();

			free(info);

			continue;
		}

		/*
		 * alloc mmc card's name
		 */
		i = 0;
		while(1)
		{
			snprintf((x_s8 *)card->name, 32, (const x_s8 *)"mmc%ld", i++);
			if(search_mmc_card(card->name) == NULL)
				break;
		}

		/*
		 * initialize mmc card's parameters
		 */
		card->info = info;
		card->host = host;

		/*
		 * register mmc card
		 */
		if(register_mmc_card(card) == TRUE)
			LOG_I("found mmc card '%s' (%s)", card->name, card->host->name);
		else
		{
			LOG_E("fail to register mmc card");
			free(card);
			free(info);
		}
	}
}

/*
 * remove all mmc card
 */
void mmc_card_remove(void)
{
	struct mmc_card_list * list;
	struct list_head * head, * curr, * next;

	head = &mmc_card_list->entry;
	curr = head->next;

	while(curr != head)
	{
		list = list_entry(curr, struct mmc_card_list, entry);

		next = curr->next;
		list_del(curr);
		curr = next;

		free(list->card->info);
		free(list->card);
		free(list);
	}
}

/*
 * search mmc card by name
 */
struct mmc_card * search_mmc_card(const char * name)
{
	struct mmc_card_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&mmc_card_list->entry)->next; pos != (&mmc_card_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct mmc_card_list, entry);
		if(strcmp((x_s8*)list->card->name, (const x_s8 *)name) == 0)
			return list->card;
	}

	return NULL;
}

/*
 * mmc card proc interface
 */
static x_s32 mmc_card_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct mmc_card_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	for(pos = (&mmc_card_list->entry)->next; pos != (&mmc_card_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct mmc_card_list, entry);

		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"%s:\r\n", list->card->name);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" host controller : %s\r\n", list->card->host->name);
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (x_u8 *)(p + offset), len);
	free(p);

	return len;
}

static struct proc mmc_card_proc = {
	.name	= "mmc",
	.read	= mmc_card_proc_read,
};

/*
 * mmc card pure sync init
 */
static __init void mmc_card_pure_sync_init(void)
{
	/* register mmc card proc interface */
	proc_register(&mmc_card_proc);
}

static __exit void mmc_card_pure_sync_exit(void)
{
	/* unregister mmc card proc interface */
	proc_unregister(&mmc_card_proc);
}

module_init(mmc_card_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(mmc_card_pure_sync_exit, LEVEL_PURE_SYNC);
