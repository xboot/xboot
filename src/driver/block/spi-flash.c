/*
 * driver/block/spi-flash.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#define	OPCODE_RDSR		0x05	/* Read status register */
#define	OPCODE_WRSR		0x01	/* Write status register 1 byte */
#define	OPCODE_WREN		0x06	/* Write enable */
#define	OPCODE_WRDI		0x04	/* Write disable */
#define	OPCODE_NREAD	0x03	/* Normal read data bytes (low frequency) */
#define	OPCODE_FREAD	0x0b	/* Fast read data bytes (high frequency) */
#define OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_BE_4K	0x20	/* Erase 4KiB block */
#define	OPCODE_PP		0x02	/* Page program (up to 256 bytes) */
#define OPCODE_RDID		0x9f	/* Read JEDEC ID */

#define	SECTOR_4K		(1<<0)	/* OPCODE_BE_4K works uniformly */
#define	NO_FASTREAD		(1<<1)	/* Can't do fastread */

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

struct spi_flash_pdata_t {
	struct spi_device_t * dev;
	struct spi_flash_id_t * id;
};

static struct spi_flash_id_t spi_flash_ids[] = {
	/* Atmel */
	FLASH_INFO("at25fs010",   0x1f6601, 0, 32 * 1024,   4, SECTOR_4K),
	FLASH_INFO("at25fs040",   0x1f6604, 0, 64 * 1024,   8, SECTOR_4K),
	FLASH_INFO("at25df041a",  0x1f4401, 0, 64 * 1024,   8, SECTOR_4K),
	FLASH_INFO("at25df321a",  0x1f4701, 0, 64 * 1024,  64, SECTOR_4K),
	FLASH_INFO("at25df641",   0x1f4800, 0, 64 * 1024, 128, SECTOR_4K),
	FLASH_INFO("at26f004",    0x1f0400, 0, 64 * 1024,   8, SECTOR_4K),
	FLASH_INFO("at26df081a",  0x1f4501, 0, 64 * 1024,  16, SECTOR_4K),
	FLASH_INFO("at26df161a",  0x1f4601, 0, 64 * 1024,  32, SECTOR_4K),
	FLASH_INFO("at26df321",   0x1f4700, 0, 64 * 1024,  64, SECTOR_4K),
	FLASH_INFO("at45db081d",  0x1f2500, 0, 64 * 1024,  16, SECTOR_4K),

	/* Winbond */
	FLASH_INFO("w25x05",  0xef3010, 0, 64 * 1024,   1, SECTOR_4K),
	FLASH_INFO("w25x10",  0xef3011, 0, 64 * 1024,   2, SECTOR_4K),
	FLASH_INFO("w25x20",  0xef3012, 0, 64 * 1024,   4, SECTOR_4K),
	FLASH_INFO("w25x40",  0xef3013, 0, 64 * 1024,   8, SECTOR_4K),
	FLASH_INFO("w25x80",  0xef3014, 0, 64 * 1024,  16, SECTOR_4K),
	FLASH_INFO("w25x16",  0xef3015, 0, 64 * 1024,  32, SECTOR_4K),
	FLASH_INFO("w25x32",  0xef3016, 0, 64 * 1024,  64, SECTOR_4K),
	FLASH_INFO("w25q32",  0xef4016, 0, 64 * 1024,  64, SECTOR_4K),
	FLASH_INFO("w25x64",  0xef3017, 0, 64 * 1024, 128, SECTOR_4K),
	FLASH_INFO("w25q64",  0xef4017, 0, 64 * 1024, 128, SECTOR_4K),
	FLASH_INFO("w25q128", 0xef4018, 0, 64 * 1024, 256, SECTOR_4K),
	FLASH_INFO("w25q256", 0xef4019, 0, 64 * 1024, 512, SECTOR_4K),
};

struct spi_flash_id_t * spi_flash_read_id(struct spi_device_t * dev)
{
	struct spi_flash_id_t * id;
	u8_t tx[1];
	u8_t rx[6];
	int res, i;

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_RDID;
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
	u8_t tx[2];

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_WRSR;
	tx[1] = sr;
	spi_device_write_then_read(dev, tx, 2, 0, 0);
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

static void spi_flash_wait_for_busy(struct spi_device_t * dev)
{
	while((spi_flash_read_status_register(dev) & 0x01) == 0x01);
}

static void spi_flash_normal_read_bytes(struct spi_device_t * dev, u64_t addr, u8_t * buf, u32_t count)
{
	u8_t tx[4];

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_NREAD;
	tx[1] = (u8_t)(addr >> 16);
	tx[2] = (u8_t)(addr >> 8);
	tx[3] = (u8_t)(addr >> 0);
	spi_device_write_then_read(dev, tx, 4, buf, count);
	spi_device_chipselect(dev, 0);
}

static void spi_flash_fast_read_bytes(struct spi_device_t * dev, u64_t addr, u8_t * buf, u32_t count)
{
	u8_t tx[5];

	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_FREAD;
	tx[1] = (u8_t)(addr >> 16);
	tx[2] = (u8_t)(addr >> 8);
	tx[3] = (u8_t)(addr >> 0);
	tx[4] = 0;
	spi_device_write_then_read(dev, tx, 5, buf, count);
	spi_device_chipselect(dev, 0);
}

static void spi_flash_sector_erase(struct spi_device_t * dev, u64_t addr)
{
	u8_t tx[4];

	spi_flash_write_enable(dev);
	spi_flash_wait_for_busy(dev);
	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_SE;
	tx[1] = (u8_t)(addr >> 16);
	tx[2] = (u8_t)(addr >> 8);
	tx[3] = (u8_t)(addr >> 0);
	spi_device_write_then_read(dev, tx, 4, 0, 0);
	spi_device_chipselect(dev, 0);
	spi_flash_wait_for_busy(dev);
}

static void spi_flash_sector_erase_4k(struct spi_device_t * dev, u64_t addr)
{
	u8_t tx[4];

	spi_flash_write_enable(dev);
	spi_flash_wait_for_busy(dev);
	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_BE_4K;
	tx[1] = (u8_t)(addr >> 16);
	tx[2] = (u8_t)(addr >> 8);
	tx[3] = (u8_t)(addr >> 0);
	spi_device_write_then_read(dev, tx, 4, 0, 0);
	spi_device_chipselect(dev, 0);
	spi_flash_wait_for_busy(dev);
}

static void spi_flash_write_one_page(struct spi_device_t * dev, u64_t addr, u8_t * buf)
{
	u8_t tx[4];

	spi_flash_write_enable(dev);
	spi_device_chipselect(dev, 1);
	tx[0] = OPCODE_PP;
	tx[1] = (u8_t)(addr >> 16);
	tx[2] = (u8_t)(addr >> 8);
	tx[3] = (u8_t)(addr >> 0);
	spi_device_write_then_read(dev, tx, 4, 0, 0);
	spi_device_write_then_read(dev, buf, 256, 0, 0);
	spi_device_chipselect(dev, 0);
	spi_flash_wait_for_busy(dev);
}

static u64_t spi_flash_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct spi_flash_pdata_t * pdat = (struct spi_flash_pdata_t *)blk->priv;
	u64_t addr = blkno * blk->blksz;
	u64_t count = blkcnt * blk->blksz;

	if(!(pdat->id->flags & NO_FASTREAD))
		spi_flash_fast_read_bytes(pdat->dev, addr, buf, count);
	else
		spi_flash_normal_read_bytes(pdat->dev, addr, buf, count);
	return blkcnt;
}

static u64_t spi_flash_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct spi_flash_pdata_t * pdat = (struct spi_flash_pdata_t *)blk->priv;
	u64_t addr = blkno * blk->blksz;
	s64_t count = blkcnt * blk->blksz;
	u8_t * pbuf = buf;
	u64_t i;

	if(pdat->id->flags & SECTOR_4K)
	{
		for(i = 0; i < blkcnt; i++)
			spi_flash_sector_erase_4k(pdat->dev, addr + i * blk->blksz);
	}
	else
	{
		for(i = 0; i < blkcnt; i++)
			spi_flash_sector_erase(pdat->dev, addr + i * blk->blksz);
	}

	while(count > 0)
	{
		spi_flash_write_one_page(pdat->dev, addr, pbuf);
		addr += 256;
		pbuf += 256;
		count -= 256;
	}
	return blkcnt;
}

static void spi_flash_sync(struct block_t * blk)
{
}

static struct device_t * spi_flash_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct spi_flash_pdata_t * pdat;
	struct block_t * blk;
	struct device_t * dev;
	struct spi_device_t * spidev;
	struct spi_flash_id_t * id;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	id = spi_flash_read_id(spidev);
	if(!id)
	{
		spi_device_free(spidev);
		return NULL;
	}

	pdat = malloc(sizeof(struct spi_flash_pdata_t));
	if(!pdat)
	{
		spi_device_free(spidev);
		return NULL;
	}

	blk = malloc(sizeof(struct block_t));
	if(!blk)
	{
		spi_device_free(spidev);
		free(pdat);
		return NULL;
	}

	pdat->dev = spidev;
	pdat->id = id;

	blk->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	if(id->flags & SECTOR_4K)
	{
		blk->blksz = 4096;
		blk->blkcnt = id->sector_size * id->sector_count / 4096;
	}
	else
	{
		blk->blksz = id->sector_size;
		blk->blkcnt = id->sector_count;
	}
	blk->read = spi_flash_read;
	blk->write = spi_flash_write;
	blk->sync = spi_flash_sync;
	blk->priv = pdat;

	spi_flash_write_disable(pdat->dev);
	spi_flash_write_enable(pdat->dev);
	spi_flash_write_status_register(pdat->dev, 0);

	if(!register_block(&dev, blk))
	{
		spi_device_free(pdat->dev);

		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void spi_flash_remove(struct device_t * dev)
{
	struct block_t * blk = (struct block_t *)dev->priv;
	struct spi_flash_pdata_t * pdat = (struct spi_flash_pdata_t *)blk->priv;

	if(blk && unregister_block(blk))
	{
		spi_device_free(pdat->dev);

		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
	}
}

static void spi_flash_suspend(struct device_t * dev)
{
}

static void spi_flash_resume(struct device_t * dev)
{
}

static struct driver_t spi_flash = {
	.name		= "spi-flash",
	.probe		= spi_flash_probe,
	.remove		= spi_flash_remove,
	.suspend	= spi_flash_suspend,
	.resume		= spi_flash_resume,
};

static __init void spi_flash_driver_init(void)
{
	register_driver(&spi_flash);
}

static __exit void spi_flash_driver_exit(void)
{
	unregister_driver(&spi_flash);
}

driver_initcall(spi_flash_driver_init);
driver_exitcall(spi_flash_driver_exit);
