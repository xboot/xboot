/*
 * driver/mtd/nand/nand.c
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
#include <stdio.h>
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


static s32_t nand_read_page(struct nand_device * nand, u32_t page, u8_t * buf, u32_t size)
{
	struct nfc * nfc;
	u32_t len, tmp;
	s32_t i;

	if(!nand)
		return -1;

	if((nfc = nand->nfc) == NULL)
		return -1;

	if( (buf == NULL) || (size <= 0) )
		return -1;

	if(size > nand->page_size)
		size = nand->page_size;

	/*
	 * nand chip enable
	 */
	nfc->control(NULL, NAND_ENABLE_CE);

	/*
	 * send command read0
	 */
	nfc->command(nand, NAND_CMD_READ0);

	/*
	 * small page device
	 */
	if(nand->page_size <= 512)
	{
		/*
		 * column
		 */
		nfc->address(nand, 0x0);

		/*
		 * row
		 */
		nfc->address(nand, page & 0xff);
		nfc->address(nand, (page >> 8) & 0xff);

		/*
		 * 4th cycle only on devices with more than 32 MiB
		 */
		if(nand->addr_cycles >= 4)
			nfc->address(nand, (page >> 16) & 0xff);

		/*
		 * 5th cycle only on devices with more than 8 GiB
		 */
		if(nand->addr_cycles >= 5)
			nfc->address(nand, (page >> 24) & 0xff);
	}

	/*
	 * large page device
	 */
	else
	{
		/*
		 * column
		 */
		nfc->address(nand, 0x0);
		nfc->address(nand, 0x0);

		/*
		 * row
		 */
		nfc->address(nand, page & 0xff);
		nfc->address(nand, (page >> 8) & 0xff);

		/*
		 * 5th cycle only on devices with more than 128 MiB
		 */
		if(nand->addr_cycles >= 5)
			nfc->address(nand, (page >> 16) & 0xff);

		/*
		 * large page devices need a start command if reading
		 */
		nfc->command(nand, NAND_CMD_READSTART);
	}

	/*
	 * wait nand ready
	 */
	nfc->nand_ready(nand, 100);

	/*
	 * read data from chip
	 */
	if(nand->bus_width == 8)
	{
		for(i = 0; i < size; i++)
		{
			nfc->read_data(nand, &tmp);
			buf[i] = (u8_t)tmp;
		}
	}
	else if(nand->bus_width == 16)
	{
		len = size & (~0x01);

		for(i = 0; i < len; i+=2)
		{
			nfc->read_data(nand, &tmp);
			buf[i] = (u8_t)(tmp & 0xff);
			buf[i+1] = (u8_t)((tmp >> 8) & 0xff);
		}

		if(size & 0x1)
		{
			nfc->read_data(nand, &tmp);
			buf[len] = (u8_t)(tmp & 0xff);
		}
	}
	else
	{
		nfc->control(NULL, NAND_DISABLE_CE);
		return -1;
	}

	/*
	 * nand chip disable
	 */
	nfc->control(NULL, NAND_DISABLE_CE);

	return 0;
}

static s32_t nand_read_oob(struct nand_device * nand, u32_t page, u8_t * buf, u32_t size)
{
	struct nfc * nfc;
	u32_t len, tmp;
	s32_t i;

	if(!nand)
		return -1;

	if((nfc = nand->nfc) == NULL)
		return -1;

	if( (buf == NULL) || (size <= 0) )
		return -1;

	/*
	 * nand chip enable
	 */
	nfc->control(NULL, NAND_ENABLE_CE);

	/*
	 * small page device
	 */
	if(nand->page_size <= 512)
	{
		/*
		 * send command readoob
		 */
		nfc->command(nand, NAND_CMD_READOOB);

		/*
		 * column
		 */
		nfc->address(nand, 0x0);

		/*
		 * row
		 */
		nfc->address(nand, page & 0xff);
		nfc->address(nand, (page >> 8) & 0xff);

		/*
		 * 4th cycle only on devices with more than 32 MiB
		 */
		if(nand->addr_cycles >= 4)
			nfc->address(nand, (page >> 16) & 0xff);

		/*
		 * 5th cycle only on devices with more than 8 GiB
		 */
		if(nand->addr_cycles >= 5)
			nfc->address(nand, (page >> 24) & 0xff);
	}

	/*
	 * large page device
	 */
	else
	{
		/*
		 * send command read0
		 */
		nfc->command(nand, NAND_CMD_READ0);

		/*
		 * column
		 */
		nfc->address(nand, 0x0);
		nfc->address(nand, (nand->page_size >> 8) & 0xff);

		/*
		 * row
		 */
		nfc->address(nand, page & 0xff);
		nfc->address(nand, (page >> 8) & 0xff);

		/*
		 * 5th cycle only on devices with more than 128 MiB
		 */
		if(nand->addr_cycles >= 5)
			nfc->address(nand, (page >> 16) & 0xff);

		/*
		 * large page devices need a start command if reading
		 */
		nfc->command(nand, NAND_CMD_READSTART);
	}

	/*
	 * wait nand ready
	 */
	nfc->nand_ready(nand, 100);

	/*
	 * read data from chip
	 */
	if(nand->bus_width == 8)
	{
		for(i = 0; i < size; i++)
		{
			nfc->read_data(nand, &tmp);
			buf[i] = (u8_t)tmp;
		}
	}
	else if(nand->bus_width == 16)
	{
		len = size & (~0x01);

		for(i = 0; i < len; i+=2)
		{
			nfc->read_data(nand, &tmp);
			buf[i] = (u8_t)(tmp & 0xff);
			buf[i+1] = (u8_t)((tmp >> 8) & 0xff);
		}

		if(size & 0x1)
		{
			nfc->read_data(nand, &tmp);
			buf[len] = (u8_t)(tmp & 0xff);
		}
	}
	else
	{
		nfc->control(NULL, NAND_DISABLE_CE);
		return -1;
	}

	/*
	 * nand chip disable
	 */
	nfc->control(NULL, NAND_DISABLE_CE);

	return 0;
}

static s32_t nand_write_page(struct nand_device * nand, u32_t page, u8_t * buf, u32_t size)
{
	return -1;
}

static s32_t nand_write_oob(struct nand_device * nand, u32_t page, u8_t * buf, u32_t size)
{
	return -1;
}

static bool_t register_nand_device(struct nand_device * nand)
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
 * probe all nand flash device
 */
void nand_flash_probe(void)
{
	struct nfc_list * list;
	struct list_head * pos;
	struct nfc * nfc;
	struct nand_device * nand;
	struct nand_info * nand_info;
	struct nand_manufacturer * nand_manufacturer;
	u8_t m_id, d_id, id_buf[3];
	u32_t data;
	s32_t i;

	/*
	 * remove all nand flash device
	 */
	nand_flash_remove();

	/*
	 * probe nand device by nand flash controller
	 */
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

			if(nfc->exit)
				nfc->exit();

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
			snprintf((char *)nand->name, 32, (const char *)"nand%ld", i++);
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
 * remove all nand flash device
 */
void nand_flash_remove(void)
{
	struct nand_list * list;
	struct list_head * head, * curr, * next;

	head = &nand_list->entry;
	curr = head->next;

	while(curr != head)
	{
		list = list_entry(curr, struct nand_list, entry);

		next = curr->next;
		list_del(curr);
		curr = next;

		free(list->nand->blocks);
		free(list->nand);
		free(list);
	}
}

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
		if(strcmp(list->nand->name, name) == 0)
			return list->nand;
	}

	return NULL;
}

/*
 * read nand device
 */
s32_t nand_read(struct nand_device * nand, u8_t * buf, u32_t addr, u32_t size)
{
	u8_t * page_buf;
	u32_t page;
	u32_t len = 0;
	u8_t * p = buf;
	u32_t o = 0, l = 0;

	if(!nand)
		return -1;

	if( (buf == NULL) || (size <= 0) )
		return -1;

	page_buf = malloc(nand->page_size);
	if(!page_buf)
		return -1;

	while(len < size)
	{
		page = addr / nand->page_size;
		o = addr % nand->page_size;
		l = nand->page_size - o;

		if(len + l > size)
			l = size - len;

		if(nand_read_page(nand, page, page_buf, nand->page_size) != 0)
		{
			free(page_buf);
			return -1;
		}

		memcpy((void *)p, (const void *)(&page_buf[o]), l);

		addr += l;
		p += l;
		len += l;
	}

	free(page_buf);

	return 0;
}

/*
 * nand proc interface
 */
static s32_t nand_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct nand_list * list;
	struct list_head * pos;
	s8_t * p;
	s32_t len = 0;
	char size[16];

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	for(pos = (&nand_list->entry)->next; pos != (&nand_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nand_list, entry);

		len += sprintf((char *)(p + len), (const char *)"%s:\r\n", list->nand->name);
		len += sprintf((char *)(p + len), (const char *)" description     : %s\r\n", list->nand->info->name);
		len += sprintf((char *)(p + len), (const char *)" manufacturer    : %s\r\n", list->nand->manufacturer->name);
		len += sprintf((char *)(p + len), (const char *)" nand controller : %s\r\n", list->nand->nfc->name);
		len += sprintf((char *)(p + len), (const char *)" bus width       : %ld\r\n", list->nand->bus_width);
		len += sprintf((char *)(p + len), (const char *)" address cycles  : %ld\r\n", list->nand->addr_cycles);
		ssize(size, (u64_t)(list->nand->page_size));
		len += sprintf((char *)(p + len), (const char *)" page size       : %s\r\n", size);
		ssize(size, (u64_t)(list->nand->erase_size));
		len += sprintf((char *)(p + len), (const char *)" block size      : %s\r\n", size);
		len += sprintf((char *)(p + len), (const char *)" block number    : %ld\r\n", list->nand->num_blocks);
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

pure_initcall_sync(nand_pure_sync_init);
pure_exitcall_sync(nand_pure_sync_exit);
