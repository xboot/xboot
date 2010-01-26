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
#include <vsprintf.h>
#include <xboot/initcall.h>
#include <xboot/proc.h>
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
static x_bool register_nand_device(struct nand_device * nand)
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
static x_bool unregister_nand_device(struct nand_device * nand)
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
 * probe nand device
 */
void nand_probe(void)
{
	struct nfc_list * list;
	struct list_head * pos;
	struct nfc * nfc;
	struct nand_device * nand;
	struct nand_info * nand_info;
	struct nand_manufacturer * nand_manufacturer;
	x_u8 m_id, d_id, id_buf[3];
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

			nfc->control(NULL, NAND_DISABLE_CE);
			nfc->control(NULL, NAND_DISABLE_CONTROLLER);
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

		/*
		 * malloc nand device buffer.
		 */
		nand = malloc(sizeof(struct nand_device));
		if(!nand)
		{
			LOG_E("can not malloc buffer for nand device");
			continue;
		}

		/*
		 * alloc nand device's name
		 */
		i = 0;
		while(1)
		{
			snprintf((x_s8 *)nand->name, 32, (const x_s8 *)"nand%ld", i++);
			if(search_nand_device(nand->name) == NULL)
				break;
		}

		/*
		 * initialize device parameters
		 */
		nand->info = nand_info;
		nand->manufacturer = nand_manufacturer;
		nand->nfc = nfc;

		/*
		 * bus width
		 */
		if(nand->info->options & NAND_BUSWIDTH_16)
			nand->bus_width = 16;
		else
			nand->bus_width = 8;

		/*
		 * extended information
		 */
		if((nand->info->page_size == 0) || (nand->info->erase_size == 0))
		{
			if(nand->bus_width == 8)
			{
				nfc->read_data(NULL, &data);
				id_buf[0] = data & 0xff;

				nfc->read_data(NULL, &data);
				id_buf[1] = data & 0xff;

				nfc->read_data(NULL, &data);
				id_buf[2] = data & 0xff;
			}
			else
			{
				nfc->read_data(NULL, &data);
				id_buf[0] = data & 0xff;

				nfc->read_data(NULL, &data);
				id_buf[1] = data & 0xff;

				nfc->read_data(NULL, &data);
				id_buf[2] = (data >> 8) && 0xff;
			}

			nand->page_size = 0x1 << (10 + (id_buf[1] & 0x3));

			switch ((id_buf[1] >> 4) & 0x3)
			{
			case 0:
				nand->erase_size = 64 << 10;
				break;
			case 1:
				nand->erase_size = 128 << 10;
				break;
			case 2:
				nand->erase_size = 256 << 10;
				break;
			case 3:
				nand->erase_size = 512 << 10;
				break;
			default:
				break;
			}
		}
		else
		{
			nand->page_size = nand->info->page_size;
			nand->erase_size = nand->info->erase_size;
		}

		/*
		 * nand chip disable
		 */
		nfc->control(NULL, NAND_DISABLE_CE);

		/*
		 * number of address cycles
		 */
		if(nand->page_size <= 512)
		{
			/*
			 * small page devices
			 */
			if(nand->info->chip_size <= 32)
				nand->addr_cycles = 3;
			else if (nand->info->chip_size <= 8 * 1024)
				nand->addr_cycles = 4;
			else
			{
				LOG_E("small page nand with more than 8 GiB encountered");
				nand->addr_cycles = 5;
			}
		}
		else
		{
			/*
			 * large page devices
			 */
			if(nand->info->chip_size <= 128)
				nand->addr_cycles = 4;
			else if(nand->info->chip_size <= 32 * 1024)
				nand->addr_cycles = 5;
			else
			{
				LOG_E("large page nand with more than 32 GiB encountered");
				nand->addr_cycles = 6;
			}
		}

		nand->num_blocks = (nand->info->chip_size * 1024) / (nand->erase_size / 1024);
		nand->blocks = malloc(sizeof(struct nand_block) * nand->num_blocks);

		for(i = 0; i < nand->num_blocks; i++)
		{
			nand->blocks[i].size = nand->erase_size;
			nand->blocks[i].offset = i * nand->erase_size;
			nand->blocks[i].is_erased = FALSE;
			nand->blocks[i].is_bad = FALSE;
		}

		if(register_nand_device(nand) == TRUE)
			LOG_I("found nand chip %s (%s)", nand->info->name, nand->manufacturer->name);
		else
		{
			LOG_E("fail to register nand device");
			free(nand->blocks);
			free(nand);
		}
	}
}

/*
 * nand proc interface
 */
static x_s32 nand_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct nand_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;
	x_s8 size[16];

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	for(pos = (&nand_list->entry)->next; pos != (&nand_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nand_list, entry);

		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"%s:\r\n", list->nand->name);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" description     : %s\r\n", list->nand->info->name);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" manufacturer    : %s\r\n", list->nand->manufacturer->name);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" nand controller : %s\r\n", list->nand->nfc->name);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" bus width       : %ld\r\n", list->nand->bus_width);
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" address cycles  : %ld\r\n", list->nand->addr_cycles);
		ssize(size, (x_u64)(list->nand->page_size));
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" page size       : %s\r\n", size);
		ssize(size, (x_u64)(list->nand->erase_size));
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" erase size      : %s\r\n", size);
		ssize(size, (x_u64)(list->nand->num_blocks));
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)" block number    : %s\r\n", size);
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

static struct proc nand_proc = {
	.name	= "nand",
	.read	= nand_proc_read,
};

/*
 * nand pure sync init
 */
static __init void nand_pure_sync_init(void)
{
	/* register nand proc interface */
	proc_register(&nand_proc);
}

static __exit void nand_pure_sync_exit(void)
{
	/* unregister nand proc interface */
	proc_unregister(&nand_proc);
}

module_init(nand_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(nand_pure_sync_exit, LEVEL_PURE_SYNC);
