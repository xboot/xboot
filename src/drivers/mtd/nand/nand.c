/*
 * driver/mtd/nand/nand.c
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
#include <xboot/initcall.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <mtd/nand/nfc.h>
#include <mtd/nand/nand.h>

extern struct nfc_list * nfc_list;

/*
 * the list of nand device
 */
static struct nand_list __nand_list = {
	.entry = {
		.next	= &(__nand_list.entry),
		.prev	= &(__nand_list.entry),
	},
};
struct nand_list * nand_list = &__nand_list;

/*
 * search nand device by name
 */
struct nand_device * search_nand_device(const char * name)
{
	struct nand_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&nand_list->entry)->next; pos != (&nand_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nand_list, entry);
		if(strcmp((x_s8*)list->nand->name, (const x_s8 *)name) == 0)
			return list->nand;
	}

	return NULL;
}

/*
 * register a nand device into nand_list
 * return true is successed, otherwise is not.
 */
x_bool register_nand_device(struct nand_device * nand)
{
	struct nand_list * list;

	list = malloc(sizeof(struct nand_list));
	if(!list || !nand)
	{
		free(list);
		return FALSE;
	}

	if(!nand->name || search_nand_device(nand->name))
	{
		free(list);
		return FALSE;
	}

	list->nand = nand;
	list_add(&list->entry, &nand_list->entry);

	return TRUE;
}

/*
 * unregister nand device from nand_list
 */
x_bool unregister_nand_device(struct nand_device * nand)
{
	struct nand_list * list;
	struct list_head * pos;

	if(!nand || !nand->name)
		return FALSE;

	for(pos = (&nand_list->entry)->next; pos != (&nand_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nand_list, entry);
		if(list->nand == nand)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * initial nand device
 */
static __init void nand_device_init(void)
{
	struct nfc_list * list;
	struct list_head * pos;
	struct nfc * nfc;
	struct nand_info * nand_info;
	struct nand_manufacturer * nand_manufacturer;
	x_u8 m_id, d_id;
	x_u32 data;
	x_s32 i;

	for(pos = (&nfc_list->entry)->next; pos != (&nfc_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nfc_list, entry);
		nfc = list->nfc;

		if(!nfc)
			continue;

		/*
		 * initialize nand controller
		 */
		if(nfc->init)
			nfc->init();

		/*
		 * enable nand controller
		 */
		nfc->control(NULL, NAND_ENABLE_CONTROLLER);

		/*
		 * nand chip enable
		 */
		nfc->control(NULL, NAND_ENABLE_CE);

		/*
		 * write reset command
		 */
		nfc->command(NULL, NAND_CMD_RESET);

		/*
		 * write read id command
		 */
		nfc->command(NULL, NAND_CMD_READID);

		/*
		 * write address 0x0
		 */
		nfc->address(NULL, 0x0);

		/*
		 * read manufacturer id
		 */
		nfc->read_data(NULL, &data);
		m_id = data & 0xff;

		/*
		 * read device id
		 */
		nfc->read_data(NULL, &data);
		d_id = data & 0xff;

		/*
		 * nand chip disable
		 */
		nfc->control(NULL, NAND_DISABLE_CE);

		/*
		 * search nand flash information
		 */
		for(nand_info = NULL, i = 0; nand_flash_ids[i].name != NULL; i++)
		{
			if(nand_flash_ids[i].id == d_id)
			{
				nand_info = &nand_flash_ids[i];
				break;
			}
		}

		if(nand_info == NULL)
		{
			LOG_E("found unknown nand flash, manufacturer id: 0x%02x device id: 0x%02x", m_id, d_id);
			continue;
		}

		/*
		 * search nand flash's manufacturer
		 */
		for(nand_manufacturer = &nand_manuf_ids[0], i = 0; nand_manuf_ids[i].name != NULL; i++)
		{
			if(nand_manuf_ids[i].id == m_id)
			{
				nand_manufacturer = &nand_manuf_ids[i];
				break;
			}
		}

		LOG_I("found nand chip %s (%s)", nand_info->name, nand_manufacturer->name);
	}
}

/*
 * remove all nand device
 */
static __exit void nand_device_exit(void)
{

}

module_init(nand_device_init, LEVEL_DRIVER);
module_exit(nand_device_exit, LEVEL_DRIVER);
