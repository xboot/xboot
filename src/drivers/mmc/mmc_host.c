/*
 * driver/mmc/mmc_host.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <mmc/mmc_host.h>


/*
 * the list of mmc host controller
 */
static struct mmc_host_list __mmc_host_list = {
	.entry = {
		.next	= &(__mmc_host_list.entry),
		.prev	= &(__mmc_host_list.entry),
	},
};
struct mmc_host_list * mmc_host_list = &__mmc_host_list;

/*
 * search mmc host controller by name
 */
struct mmc_host * search_mmc_host(const char * name)
{
	struct mmc_host_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&mmc_host_list->entry)->next; pos != (&mmc_host_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct mmc_host_list, entry);
		if(strcmp(list->host->name, name) == 0)
			return list->host;
	}

	return NULL;
}

/*
 * register a mmc host controller into mmc_host_list
 */
bool_t register_mmc_host(struct mmc_host * host)
{
	struct mmc_host_list * list;

	list = malloc(sizeof(struct mmc_host_list));
	if(!list || !host)
	{
		free(list);
		return FALSE;
	}

	if(!host->name || search_mmc_host(host->name))
	{
		free(list);
		return FALSE;
	}

	list->host = host;
	list_add(&list->entry, &mmc_host_list->entry);

	return TRUE;
}

/*
 * unregister mmc host controller from mmc_host_list
 */
bool_t unregister_mmc_host(struct mmc_host * host)
{
	struct mmc_host_list * list;
	struct list_head * pos;

	if(!host || !host->name)
		return FALSE;

	for(pos = (&mmc_host_list->entry)->next; pos != (&mmc_host_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct mmc_host_list, entry);
		if(list->host == host)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}
