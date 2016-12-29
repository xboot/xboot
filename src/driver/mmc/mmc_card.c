/*
 * driver/mmc/mmc_card.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <xboot/initcall.h>
#include <block/block.h>
#include <block/disk.h>
#include <block/partition.h>
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

static bool_t mmc_card_decode(struct mmc_card_t * card)
{
	struct mmc_card_info_t * info;
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
			LOG("the card '%s' (%s) has unknown mmca version %ld", card->name, card->host->name, mmca_vsn);
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
			LOG("unrecognized csd structure version (%ld) of the card '%s' (%s)", csd_struct, card->name, card->host->name);
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
			LOG("unrecognized csd structure version (%ld) of the card '%s' (%s)", csd_struct, card->name, card->host->name);
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

static u64_t mmc_read(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count)
{
	struct mmc_card_t * card = (struct mmc_card_t *)(disk->priv);

	if(card->host->read_sectors(card, buf, sector, count) != TRUE)
		return 0;
	return count;
}

static u64_t mmc_write(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count)
{
	struct mmc_card_t * card = (struct mmc_card_t *)(disk->priv);

	if(card->host->write_sectors(card, buf, sector, count) != TRUE)
		return 0;
	return count;
}

static void mmc_sync(struct disk_t * disk)
{
}

static bool_t register_mmc_card(struct mmc_card_t * card)
{
	struct mmc_card_list * list;
	struct disk_t * disk;

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

	disk = malloc(sizeof(struct disk_t));
	if(!disk)
	{
		free(list);
		return FALSE;
	}

	disk->name = card->name;
	disk->size = card->info->sector_size;
	disk->count = card->info->sector_count;
	disk->read = mmc_read;
	disk->write = mmc_write;
	disk->sync = mmc_sync;
	disk->priv = (void *)card;
	card->priv = (void *)disk;

	if(!register_disk(NULL, disk))
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
	struct mmc_host_t * host;
	struct mmc_card_info_t * info;
	struct mmc_card_t * card;
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
		info = malloc(sizeof(struct mmc_card_info_t));
		if(!info)
		{
			LOG("can not malloc buffer for mmc card information");
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
		card = malloc(sizeof(struct mmc_card_t));
		if(!card)
		{
			LOG("can not malloc buffer for mmc card");

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
			snprintf((char *)card->name, 32, (const char *)"mmc%d", i++);
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
			LOG("Failed to decode mmc card '%s' (%s)", card->name, card->host->name);
			free(card);
			free(info);
		}

		/*
		 * register mmc card
		 */
		if(register_mmc_card(card) == TRUE)
			LOG("found mmc card '%s' (%s)", card->name, card->host->name);
		else
		{
			LOG("Failed to register mmc card '%s' (%s)", card->name, card->host->name);
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
	struct disk_t * disk;

	head = &mmc_card_list->entry;
	curr = head->next;

	while(curr != head)
	{
		list = list_entry(curr, struct mmc_card_list, entry);

		next = curr->next;
		list_del(curr);
		curr = next;

		disk = (struct disk_t *)(list->card->priv);
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
struct mmc_card_t * search_mmc_card(const char * name)
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
