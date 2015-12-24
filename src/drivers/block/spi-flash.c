/*
 * drivers/block/spi-flash.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <block/spi-flash.h>

#define FLASH_INFO(n, jid, eid, sz, cnt, f)				\
	{													\
		.name = n,										\
		.id = {											\
			((jid) >> 16) & 0xff,						\
			((jid) >> 8) & 0xff,						\
			(jid) & 0xff,								\
			((eid) >> 8) & 0xff,						\
			(eid) & 0xff,								\
			},											\
		.id_len = (!(jid) ? 0 : (3 + ((eid) ? 2 : 0))),	\
		.sector_size = (sz),							\
		.sector_count = (cnt),							\
		.flags = (f),									\
	}

struct spi_flash_id_t {
	char * name;
	char id[5];
	char id_len;
	int sector_size;
	int sector_count;
	int flags;
};

struct spi_flash_private_data_t {
	int rev;
};

static const struct spi_flash_id_t spi_flash_ids[] = {
	/* Winbond */
	FLASH_INFO("w25x05",  0xef3010, 0, 64 * 1024,   1, 0),
	FLASH_INFO("w25x10",  0xef3011, 0, 64 * 1024,   2, 0),
	FLASH_INFO("w25x20",  0xef3012, 0, 64 * 1024,   4, 0),
	FLASH_INFO("w25x40",  0xef3013, 0, 64 * 1024,   8, 0),
	FLASH_INFO("w25x80",  0xef3014, 0, 64 * 1024,  16, 0),
	FLASH_INFO("w25x16",  0xef3015, 0, 64 * 1024,  32, 0),
	FLASH_INFO("w25x32",  0xef3016, 0, 64 * 1024,  64, 0),
	FLASH_INFO("w25q32",  0xef4016, 0, 64 * 1024,  64, 0),
	FLASH_INFO("w25x64",  0xef3017, 0, 64 * 1024, 128, 0),
	FLASH_INFO("w25q64",  0xef4017, 0, 64 * 1024, 128, 0),
	FLASH_INFO("w25q128", 0xef4018, 0, 64 * 1024, 256, 0),
	FLASH_INFO("w25q256", 0xef4019, 0, 64 * 1024, 512, 0),
};

static u64_t spi_flash_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	return 0;
}

static void spi_flash_sync(struct block_t * blk)
{
}

static void spi_flash_suspend(struct block_t * blk)
{
}

static void spi_flash_resume(struct block_t * blk)
{
}

static bool_t spi_flash_register(struct resource_t * res)
{
	struct spi_flash_data_t * rdat = (struct spi_flash_data_t *)res->data;
	struct spi_flash_private_data_t * dat;
	struct block_t * blk;
	char name[64];

	return FALSE;
}

static bool_t spi_flash_unregister(struct resource_t * res)
{
	return TRUE;
}

static __init void spi_flash_device_init(void)
{
	resource_for_each_with_name("spi-flash", spi_flash_register);
}

static __exit void spi_flash_device_exit(void)
{
	resource_for_each_with_name("spi-flash", spi_flash_unregister);
}

device_initcall(spi_flash_device_init);
device_exitcall(spi_flash_device_exit);
