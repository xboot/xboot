/*
 * driver/mmc/mmc_card.c
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
#include <xboot/blkdev.h>
#include <disk/disk.h>
#include <disk/partition.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>

extern struct mmc_host_list * mmc_host_list;

#define UNSTUFF_BITS(resp, start, size)								\
	({																\
		const s32_t __size = size;									\
		const u32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const s32_t __off = 3 - ((start) / 32);						\
		const s32_t __shft = (start) & 31;							\
		u32_t __res;												\
																	\
		__res = resp[__off] >> __shft;								\
		if (__size + __shft > 32)									\
			__res |= resp[__off-1] << ((32 - __shft) % 32);			\
		__res & __mask;												\
	})

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

static const u32_t tran_exp[] = {
	10000, 100000, 1000000, 10000000, 0, 0, 0, 0
};

static const u8_t tran_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static const u32_t tacc_exp[] = {
	1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
};

static const u32_t tacc_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static bool_t mmc_card_decode(struct mmc_card * card)
{
	struct mmc_card_info * info;
	u32_t e, m, csd_struct;
	u32_t mmca_vsn;

	if(!card || !card->info)
		return FALSE;

	info = card->info;

	/*
	 * decode cid
	 */
	switch(info->type)
	{
	case MMC_CARD_TYPE_MMC:
		mmca_vsn				= UNSTUFF_BITS(info->raw_csd, 122, 4);
		switch(mmca_vsn)
		{
		case 0:	/* mmc v1.0 - v1.2 */
		case 1: /* mmc v1.4 */
			info->cid.mid		= UNSTUFF_BITS(info->raw_cid, 120, 8);
			info->cid.oid		= UNSTUFF_BITS(info->raw_cid, 104, 16);
			info->cid.pnm[0]	= UNSTUFF_BITS(info->raw_cid, 96, 8);
			info->cid.pnm[1] 	= UNSTUFF_BITS(info->raw_cid, 88, 8);
			info->cid.pnm[2] 	= UNSTUFF_BITS(info->raw_cid, 80, 8);
			info->cid.pnm[3] 	= UNSTUFF_BITS(info->raw_cid, 72, 8);
			info->cid.pnm[4] 	= UNSTUFF_BITS(info->raw_cid, 64, 8);
			info->cid.pnm[5] 	= UNSTUFF_BITS(info->raw_cid, 56, 8);
			info->cid.pnm[6] 	= UNSTUFF_BITS(info->raw_cid, 48, 8);
			info->cid.pnm[7] 	= 0;
			info->cid.hwrev 	= UNSTUFF_BITS(info->raw_cid, 44, 4);
			info->cid.fwrev 	= UNSTUFF_BITS(info->raw_cid, 40, 4);
			info->cid.serial 	= UNSTUFF_BITS(info->raw_cid, 16, 24);
			info->cid.year		= UNSTUFF_BITS(info->raw_cid, 8, 4);
			info->cid.year		= info->cid.year + 1997;
			info->cid.month		= UNSTUFF_BITS(info->raw_cid, 12, 4);
			break;

		case 2: /* mmc v2.0 - v2.2 */
		case 3: /* mmc v3.1 - v3.3 */
		case 4: /* mmc v4 */
			info->cid.mid		= UNSTUFF_BITS(info->raw_cid, 120, 8);
			info->cid.oid		= UNSTUFF_BITS(info->raw_cid, 104, 16);
			info->cid.pnm[0]	= UNSTUFF_BITS(info->raw_cid, 96, 8);
			info->cid.pnm[1] 	= UNSTUFF_BITS(info->raw_cid, 88, 8);
			info->cid.pnm[2] 	= UNSTUFF_BITS(info->raw_cid, 80, 8);
			info->cid.pnm[3] 	= UNSTUFF_BITS(info->raw_cid, 72, 8);
			info->cid.pnm[4] 	= UNSTUFF_BITS(info->raw_cid, 64, 8);
			info->cid.pnm[5] 	= UNSTUFF_BITS(info->raw_cid, 56, 8);
			info->cid.pnm[6] 	= 0;
			info->cid.pnm[7] 	= 0;
			info->cid.hwrev 	= 0;
			info->cid.fwrev 	= 0;
			info->cid.serial 	= UNSTUFF_BITS(info->raw_cid, 16, 32);
			info->cid.year		= UNSTUFF_BITS(info->raw_cid, 8, 4);
			info->cid.year		= info->cid.year + 1997;
			info->cid.month		= UNSTUFF_BITS(info->raw_cid, 12, 4);
			break;

		default:
			LOG_E("the card '%s' (%s) has unknown mmca version %ld", card->name, card->host->name, mmca_vsn);
			return FALSE;
		}

		break;

	case MMC_CARD_TYPE_SD:
	case MMC_CARD_TYPE_SD20:
	case MMC_CARD_TYPE_SDHC:
		info->cid.mid		= UNSTUFF_BITS(info->raw_cid, 120, 8);
		info->cid.oid		= UNSTUFF_BITS(info->raw_cid, 104, 16);
		info->cid.pnm[0]	= UNSTUFF_BITS(info->raw_cid, 96, 8);
		info->cid.pnm[1] 	= UNSTUFF_BITS(info->raw_cid, 88, 8);
		info->cid.pnm[2] 	= UNSTUFF_BITS(info->raw_cid, 80, 8);
		info->cid.pnm[3] 	= UNSTUFF_BITS(info->raw_cid, 72, 8);
		info->cid.pnm[4] 	= UNSTUFF_BITS(info->raw_cid, 64, 8);
		info->cid.pnm[5] 	= 0;
		info->cid.pnm[6] 	= 0;
		info->cid.pnm[7] 	= 0;
		info->cid.hwrev 	= UNSTUFF_BITS(info->raw_cid, 60, 4);
		info->cid.fwrev 	= UNSTUFF_BITS(info->raw_cid, 56, 4);
		info->cid.serial 	= UNSTUFF_BITS(info->raw_cid, 24, 32);
		info->cid.year		= UNSTUFF_BITS(info->raw_cid, 12, 8);
		info->cid.year		= info->cid.year + 2000;
		info->cid.month		= UNSTUFF_BITS(info->raw_cid, 8, 4);
		break;

	default:
		return FALSE;
	}

	/*
	 * decode csd
	 */
	csd_struct = UNSTUFF_BITS(info->raw_csd, 126, 2);
	switch(info->type)
	{
	case MMC_CARD_TYPE_MMC:
		if(csd_struct != 1 && csd_struct != 2)
		{
			LOG_E("unrecognized csd structure version (%ld) of the card '%s' (%s)", csd_struct, card->name, card->host->name);
			return FALSE;
		}

		info->csd.mmca_vsn				= UNSTUFF_BITS(info->raw_csd, 122, 4);
		info->csd.cmdclass				= UNSTUFF_BITS(info->raw_csd, 84, 12);

		info->csd.tacc_clks				= UNSTUFF_BITS(info->raw_csd, 104, 8) * 100;
		m								= UNSTUFF_BITS(info->raw_csd, 115, 4);
		e								= UNSTUFF_BITS(info->raw_csd, 112, 3);
		info->csd.tacc_ns				= (tacc_exp[e] * tacc_mant[m] + 9) / 10;

		info->csd.r2w_factor			= UNSTUFF_BITS(info->raw_csd, 26, 3);
		m								= UNSTUFF_BITS(info->raw_csd, 99, 4);
		e								= UNSTUFF_BITS(info->raw_csd, 96, 3);
		info->csd.max_dtr				= tran_exp[e] * tran_mant[m];

		info->csd.read_blkbits			= UNSTUFF_BITS(info->raw_csd, 80, 4);
		info->csd.write_blkbits			= UNSTUFF_BITS(info->raw_csd, 22, 4);
		e								= UNSTUFF_BITS(info->raw_csd, 47, 3);
		m								= UNSTUFF_BITS(info->raw_csd, 62, 12);
		info->csd.capacity				= (1 + m) << (e + 2);

		info->csd.read_partial			= UNSTUFF_BITS(info->raw_csd, 79, 1);
		info->csd.read_misalign			= UNSTUFF_BITS(info->raw_csd, 77, 1);
		info->csd.write_misalign		= UNSTUFF_BITS(info->raw_csd, 78, 1);
		info->csd.write_partial			= UNSTUFF_BITS(info->raw_csd, 21, 1);
		break;

	case MMC_CARD_TYPE_SD:
	case MMC_CARD_TYPE_SD20:
	case MMC_CARD_TYPE_SDHC:
		switch(csd_struct)
		{
		case 0:
			info->csd.cmdclass			= UNSTUFF_BITS(info->raw_csd, 84, 12);

			info->csd.tacc_clks			= UNSTUFF_BITS(info->raw_csd, 104, 8) * 100;
			m 							= UNSTUFF_BITS(info->raw_csd, 115, 4);
			e 							= UNSTUFF_BITS(info->raw_csd, 112, 3);
			info->csd.tacc_ns			= (tacc_exp[e] * tacc_mant[m] + 9) / 10;

			info->csd.r2w_factor		= UNSTUFF_BITS(info->raw_csd, 26, 3);
			m 							= UNSTUFF_BITS(info->raw_csd, 99, 4);
			e 							= UNSTUFF_BITS(info->raw_csd, 96, 3);
			info->csd.max_dtr			= tran_exp[e] * tran_mant[m];

			info->csd.read_blkbits 		= UNSTUFF_BITS(info->raw_csd, 80, 4);
			info->csd.write_blkbits		= UNSTUFF_BITS(info->raw_csd, 22, 4);
			e 							= UNSTUFF_BITS(info->raw_csd, 47, 3);
			m 							= UNSTUFF_BITS(info->raw_csd, 62, 12);
			info->csd.capacity			= (1 + m) << (e + 2);

			info->csd.read_partial 		= UNSTUFF_BITS(info->raw_csd, 79, 1);
			info->csd.read_misalign		= UNSTUFF_BITS(info->raw_csd, 77, 1);
			info->csd.write_misalign	= UNSTUFF_BITS(info->raw_csd, 78, 1);
			info->csd.write_partial		= UNSTUFF_BITS(info->raw_csd, 21, 1);
			break;

		case 1:
			info->csd.cmdclass			= UNSTUFF_BITS(info->raw_csd, 84, 12);

			info->csd.tacc_clks			= 0;
			info->csd.tacc_ns			= 0;

			info->csd.r2w_factor		= 4;
			m							= UNSTUFF_BITS(info->raw_csd, 99, 4);
			e							= UNSTUFF_BITS(info->raw_csd, 96, 3);
			info->csd.max_dtr			= tran_exp[e] * tran_mant[m];

			info->csd.read_blkbits		= 9;
			info->csd.write_blkbits		= 9;
			m							= UNSTUFF_BITS(info->raw_csd, 48, 22);
			info->csd.capacity			= (1 + m) << 10;

			info->csd.read_partial		= 0;
			info->csd.read_misalign		= 0;
			info->csd.write_misalign	= 0;
			info->csd.write_partial		= 0;
			break;

		default:
			LOG_E("unrecognized csd structure version (%ld) of the card '%s' (%s)", csd_struct, card->name, card->host->name);
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}

	info->sector_size = 1 << info->csd.read_blkbits;
	info->sector_count = info->csd.capacity;
	info->capacity = info->sector_count * info->sector_size;

	return TRUE;
}

static ssize_t mmc_read_sectors(struct disk * disk, u8_t * buf, size_t sector, size_t count)
{
	struct mmc_card * card = (struct mmc_card *)(disk->priv);

	if(card->host->read_sectors(card, buf, sector, count) != TRUE)
		return 0;

	return count;
}

static ssize_t mmc_write_sectors(struct disk * disk, const u8_t * buf, size_t sector, size_t count)
{
	struct mmc_card * card = (struct mmc_card *)(disk->priv);

	if(card->host->write_sectors(card, buf, sector, count) != TRUE)
		return 0;

	return count;
}

static bool_t register_mmc_card(struct mmc_card * card)
{
	struct mmc_card_list * list;
	struct disk * disk;

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

	disk = malloc(sizeof(struct disk));
	if(!disk)
	{
		free(list);
		return FALSE;
	}

	disk->name = card->name;
	disk->sector_size = card->info->sector_size;
	disk->sector_count = card->info->sector_count;
	disk->read_sectors = mmc_read_sectors;
	disk->write_sectors = mmc_write_sectors;
	disk->priv = (void *)card;
	card->priv = (void *)disk;

	if(!register_disk(disk, BLK_DEV_MMC))
	{
		free(list);
		free(disk);
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
	s32_t i;

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
			snprintf((char *)card->name, 32, (const char *)"mmc%ld", i++);
			if(search_mmc_card(card->name) == NULL)
				break;
		}

		/*
		 * initialize mmc card's parameters
		 */
		card->info = info;
		card->host = host;

		/*
		 * decode mmc card information
		 */
		if(!mmc_card_decode(card))
		{
			LOG_E("fail to decode mmc card '%s' (%s)", card->name, card->host->name);
			free(card);
			free(info);
		}

		/*
		 * register mmc card
		 */
		if(register_mmc_card(card) == TRUE)
			LOG_I("found mmc card '%s' (%s)", card->name, card->host->name);
		else
		{
			LOG_E("fail to register mmc card '%s' (%s)", card->name, card->host->name);
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
	struct disk * disk;

	head = &mmc_card_list->entry;
	curr = head->next;

	while(curr != head)
	{
		list = list_entry(curr, struct mmc_card_list, entry);

		next = curr->next;
		list_del(curr);
		curr = next;

		disk = (struct disk *)(list->card->priv);
		unregister_disk(disk);
		free(disk);

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
		if(strcmp((char*)list->card->name, (const char *)name) == 0)
			return list->card;
	}

	return NULL;
}

/*
 * mmc card proc interface
 */
static s32_t mmc_card_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct mmc_card_list * list;
	struct list_head * pos;
	s8_t * p;
	s32_t len = 0;
	char buff[32];

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	for(pos = (&mmc_card_list->entry)->next; pos != (&mmc_card_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct mmc_card_list, entry);

		len += sprintf((char *)(p + len), (const char *)"%s:\r\n", list->card->name);
		len += sprintf((char *)(p + len), (const char *)" host controller   : %s\r\n", list->card->host->name);
		switch(list->card->info->type)
		{
		case MMC_CARD_TYPE_MMC:
			strcpy(buff, (const char *)"mmc card");
			break;

		case MMC_CARD_TYPE_SD:
			strcpy(buff, (const char *)"sd card");
			break;

		case MMC_CARD_TYPE_SD20:
			strcpy(buff, (const char *)"sd card version 2.0");
			break;

		case MMC_CARD_TYPE_SDHC:
			strcpy(buff, (const char *)"sdhc card");
			break;

		default:
			strcpy(buff, (const char *)"unknown");
			break;
		}
		len += sprintf((char *)(p + len), (const char *)" card type         : %s\r\n", buff);
		len += sprintf((char *)(p + len), (const char *)" manufacturer id   : 0x%lx\r\n", (u32_t)list->card->info->cid.mid);
		len += sprintf((char *)(p + len), (const char *)" oem id            : 0x%lx\r\n", (u32_t)list->card->info->cid.oid);
		len += sprintf((char *)(p + len), (const char *)" product name      : %s\r\n", list->card->info->cid.pnm);
		len += sprintf((char *)(p + len), (const char *)" hardware revision : 0x%lx\r\n", (u32_t)list->card->info->cid.hwrev);
		len += sprintf((char *)(p + len), (const char *)" firmware revision : 0x%lx\r\n", (u32_t)list->card->info->cid.fwrev);
		len += sprintf((char *)(p + len), (const char *)" serial number     : 0x%lx\r\n", (u32_t)list->card->info->cid.serial);
		len += sprintf((char *)(p + len), (const char *)" manufacture date  : %ld/%02ld\r\n", (u32_t)list->card->info->cid.year, (u32_t)list->card->info->cid.month);

		ssize(buff, (u64_t)(list->card->info->sector_size));
		len += sprintf((char *)(p + len), (const char *)" sector size       : %s\r\n", buff);
		len += sprintf((char *)(p + len), (const char *)" sector count      : %zd\r\n", list->card->info->sector_count);
		ssize(buff, (u64_t)(list->card->info->capacity));
		len += sprintf((char *)(p + len), (const char *)" total capacity    : %s\r\n", buff);
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

pure_initcall_sync(mmc_card_pure_sync_init);
pure_exitcall_sync(mmc_card_pure_sync_exit);
