/*
 * driver/mmc/mmc_host.c
 *
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <xboot/major.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <mmc/mmc.h>
#include <mmc/mmc_host.h>

/*
 * the hash list of mmc host controller.
 */
static struct hlist_head mmc_host_hash[CONFIG_MMC_HOST_HASH_SIZE] = {{0}};

/*
 * search mmc host controller by name
 */
struct mmc_host * search_mmc_host(const char *name)
{
	struct mmc_host_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!name)
		return NULL;

	hash = string_hash(name) % CONFIG_MMC_HOST_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(mmc_host_hash[hash]), node)
	{
		if(strcmp((x_s8*)list->host->name, (const x_s8 *)name) == 0)
			return list->host;
	}

	return NULL;
}

/*
 * register a mmc host controller.
 */
x_bool register_mmc_host(struct mmc_host * host)
{
	struct mmc_host_list * list;
	x_u32 hash;

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

	hash = string_hash(host->name) % CONFIG_MMC_HOST_HASH_SIZE;
	hlist_add_head(&(list->node), &(mmc_host_hash[hash]));

	return TRUE;
}

/*
 * unregister mmc host controller.
 */
x_bool unregister_mmc_host(struct mmc_host * host)
{
	struct mmc_host_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!host || !host->name)
		return FALSE;

	hash = string_hash(host->name) % CONFIG_MMC_HOST_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(mmc_host_hash[hash]), node)
	{
		if(list->host == host)
		{
			hlist_del(&(list->node));
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * mmc host pure init
 */
static __init void mmc_host_pure_sync_init(void)
{
	x_s32 i;

	/* initialize mmc host controller hash list */
	for(i = 0; i < CONFIG_MMC_HOST_HASH_SIZE; i++)
		init_hlist_head(&mmc_host_hash[i]);
}

module_init(mmc_host_pure_sync_init, LEVEL_PURE_SYNC);
