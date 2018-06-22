/*
 * driver/block/spi-flash.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#define SFDP_MAX_NPH	(6)

struct sfdp_header_t {
	uint8_t	sign[4];
	uint8_t	minor;
	uint8_t	major;
	uint8_t	nph;
	uint8_t	unused;
};

struct sfdp_parameter_header_t {
    uint8_t	idlsb;
    uint8_t	minor;
    uint8_t	major;
    uint8_t	length;
    uint8_t	ptp[3];
    uint8_t	idmsb;
};

struct sfdp_basic_table_t {
	uint8_t	buf[16 * 4];
};

struct sfdp_t {
	struct sfdp_header_t h;
	struct sfdp_parameter_header_t ph[SFDP_MAX_NPH];
	struct sfdp_basic_table_t bt;
};

struct spi_flash_pdata_t {
	struct spi_device_t * dev;
};

static bool_t spi_flash_read_sfdp(struct spi_device_t * dev, struct sfdp_t * sfdp)
{
	uint32_t addr;
	u8_t tx[5];
	int i, r;

	tx[0] = 0x5a;
	tx[1] = 0x0;
	tx[2] = 0x0;
	tx[3] = 0x0;
	tx[4] = 0x0;
	spi_device_select(dev);
	r = spi_device_write_then_read(dev, tx, 5, &sfdp->h, sizeof(struct sfdp_header_t));
	spi_device_deselect(dev);
	if(r < 0)
		return FALSE;
	if((sfdp->h.sign[0] != 'S') || (sfdp->h.sign[1] != 'F') || (sfdp->h.sign[2] != 'D') || (sfdp->h.sign[3] != 'P'))
		return FALSE;

	sfdp->h.nph = sfdp->h.nph > SFDP_MAX_NPH ? sfdp->h.nph + 1 : SFDP_MAX_NPH;
	spi_device_select(dev);
	for(i = 0; i < sfdp->h.nph; i++)
	{
		addr = i * sizeof(struct sfdp_parameter_header_t) + sizeof(struct sfdp_header_t);
		tx[0] = 0x5a;
		tx[1] = (addr >> 16) & 0xff;
		tx[2] = (addr >>  8) & 0xff;
		tx[3] = (addr >>  0) & 0xff;
		tx[4] = 0x0;
		if(spi_device_write_then_read(dev, tx, 5, &sfdp->ph[i], sizeof(struct sfdp_parameter_header_t)) < 0)
			break;
	}
	spi_device_deselect(dev);
	if(i < sfdp->h.nph)
		return FALSE;

	for(i = 0; i < sfdp->h.nph; i++)
	{
		if((sfdp->ph[i].idlsb == 0x00) && (sfdp->ph[i].idmsb == 0xff))
		{
			spi_device_select(dev);
			addr = (sfdp->ph[i].ptp[0] << 0) | (sfdp->ph[i].ptp[1] << 8) | (sfdp->ph[i].ptp[2] << 16);
			tx[0] = 0x5a;
			tx[1] = (addr >> 16) & 0xff;
			tx[2] = (addr >>  8) & 0xff;
			tx[3] = (addr >>  0) & 0xff;
			tx[4] = 0x0;
			r = spi_device_write_then_read(dev, tx, 5, &sfdp->bt, sfdp->ph[i].length * 4);
			spi_device_deselect(dev);
			if(r >= 0)
				return TRUE;
		}
	}
	return FALSE;
}

/*
struct spi_flash_id_t * spi_flash_read_id(struct spi_device_t * dev)
{
	struct spi_flash_id_t * id;
	u8_t tx[1];
	u8_t rx[6];
	int res, i;

	spi_device_select(dev);
	tx[0] = OPCODE_RDID;
	res = spi_device_write_then_read(dev, tx, 1, rx, 6);
	spi_device_deselect(dev);

	if(res < 0)
		return NULL;

	LOG("%02x:%02x:%02x:%02x:%02x:%02x", rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);
	for(i = 0; i < ARRAY_SIZE(spi_flash_ids); i++)
	{
		id = &spi_flash_ids[i];
		if((id->id_len > 0) && (memcmp(id->id, rx, id->id_len) == 0))
			return id;
	}

	return NULL;
}
*/

static u64_t spi_flash_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct spi_flash_pdata_t * pdat = (struct spi_flash_pdata_t *)blk->priv;
	u64_t addr = blkno * blk->blksz;
	u64_t count = blkcnt * blk->blksz;

	spi_device_select(pdat->dev);
/*	if(!(pdat->id->flags & NO_FASTREAD))
		spi_flash_fast_read_bytes(pdat->dev, addr, buf, count);
	else
		spi_flash_normal_read_bytes(pdat->dev, addr, buf, count);*/
	spi_device_deselect(pdat->dev);
	return blkcnt;
}

static u64_t spi_flash_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct spi_flash_pdata_t * pdat = (struct spi_flash_pdata_t *)blk->priv;
	u64_t addr = blkno * blk->blksz;
	s64_t count = blkcnt * blk->blksz;
	u8_t * pbuf = buf;
	u64_t i;

	spi_device_select(pdat->dev);
/*	if(pdat->id->flags & SECTOR_4K)
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
	}*/
	spi_device_deselect(pdat->dev);

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
	struct sfdp_t sfdp;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	memset(&sfdp, 0, sizeof(struct sfdp_t));
	if(!spi_flash_read_sfdp(spidev, &sfdp))
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

	blk->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
/*	if(id->flags & SECTOR_4K)
	{
		blk->blksz = 4096;
		blk->blkcnt = id->sector_size * id->sector_count / 4096;
	}
	else
	{
		blk->blksz = id->sector_size;
		blk->blkcnt = id->sector_count;
	}*/
	blk->read = spi_flash_read;
	blk->write = spi_flash_write;
	blk->sync = spi_flash_sync;
	blk->priv = pdat;

/*	spi_device_select(pdat->dev);
	spi_flash_write_disable(pdat->dev);
	spi_flash_write_enable(pdat->dev);
	spi_flash_write_status_register(pdat->dev, 0);
	spi_device_deselect(pdat->dev);*/

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
