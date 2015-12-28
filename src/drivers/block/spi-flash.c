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

#define	OPCODE_RDSR			0x05	/* Read status register */
#define	OPCODE_WRSR			0x01	/* Write status register 1 byte */
#define	OPCODE_WREN			0x06	/* Write enable */
#define	OPCODE_WRDI			0x04	/* Write disable */

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
	unsigned char id[6];
	int id_len;
	int sector_size;
	int sector_count;
	int flags;
};

struct spi_flash_private_data_t {
	struct spi_device_t * dev;
	struct spi_flash_id_t * id;
};

static struct spi_flash_id_t spi_flash_ids[] = {
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

struct spi_flash_id_t * spi_flash_read_id(struct spi_device_t * dev)
{
	struct spi_flash_id_t * id;
	u8_t tx[1];
	u8_t rx[6];
	int res, i;

	spi_device_chipselect(dev, 1);
	tx[0] = 0x9f;
	res = spi_device_write_then_read(dev, tx, 1, rx, 6);
	spi_device_chipselect(dev, 0);

	if(res < 0)
		return NULL;

	for(i = 0; i < ARRAY_SIZE(spi_flash_ids); i++)
	{
		id = &spi_flash_ids[i];
		if((id->id_len > 0) && (memcmp(id->id, rx, id->id_len) == 0))
			return id;
	}

	return NULL;
}

static u8_t spi_flash_read_status_register(struct spi_device_t * dev)
{
	u8_t tx[1];
	u8_t rx[1];

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_RDSR;
	spi_device_write_then_read(dev, tx, 1, rx, 1);
	spi_device_chipselect(dev, 0);

	return rx[0];
}

static void spi_flash_write_status_register(struct spi_device_t * dev, u8_t sr)
{
	u8_t tx[1];

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_WRSR;
	spi_device_write_then_read(dev, tx, 1, 0, 0);
	tx[0] = sr;
	spi_device_write_then_read(dev, tx, 1, 0, 0);
	spi_device_chipselect(dev, 0);
}

static void spi_flash_write_enable(struct spi_device_t * dev)
{
	u8_t tx[1];

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_WREN;
	spi_device_write_then_read(dev, tx, 1, 0, 0);
	spi_device_chipselect(dev, 0);
}

static void spi_flash_write_disable(struct spi_device_t * dev)
{
	u8_t tx[1];

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_WRDI;
	spi_device_write_then_read(dev, tx, 1, 0, 0);
	spi_device_chipselect(dev, 0);
}

static u64_t spi_flash_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	return 0;
}

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
	struct spi_device_t * dev;
	struct spi_flash_id_t * id;
	struct block_t * blk;
	char name[64];

	dev = spi_device_alloc(rdat->spibus, 0, 8, 0);
	if(!dev)
		return FALSE;

	id = spi_flash_read_id(dev);
	if(!id)
	{
		spi_device_free(dev);
		return FALSE;
	}

	dat = malloc(sizeof(struct spi_flash_private_data_t));
	if(!dat)
	{
		spi_device_free(dev);
		return FALSE;
	}

	blk = malloc(sizeof(struct block_t));
	if(!blk)
	{
		spi_device_free(dev);
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->dev = dev;
	dat->id = id;

	blk->name = strdup(name);
	blk->blksz = 512;
	blk->blkcnt = 1000;
	blk->read = spi_flash_read;
	blk->write = spi_flash_write;
	blk->sync = spi_flash_sync;
	blk->suspend = spi_flash_suspend;
	blk->resume = spi_flash_resume;
	blk->priv = dat;

	if(register_block(blk))
		return TRUE;

	spi_device_free(dev);
	free(blk->priv);
	free(blk->name);
	free(blk);
	return FALSE;
}

static bool_t spi_flash_unregister(struct resource_t * res)
{
	struct block_t * blk;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	blk = search_block(name);
	if(!blk)
		return FALSE;

	if(!unregister_block(blk))
		return FALSE;

	spi_device_free(((struct spi_flash_private_data_t *)blk->priv)->dev);
	free(blk->priv);
	free(blk->name);
	free(blk);
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
