/*
 * driver/block/spi-flash.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <spi/spi.h>
#include <block/block.h>

enum {
	OPCODE_SFDP			= 0x5a,
	OPCODE_RDID			= 0x9f,
	OPCODE_WRSR			= 0x01,
	OPCODE_RDSR			= 0x05,
	OPCODE_WREN			= 0x06,
	OPCODE_READ			= 0x03,
	OPCODE_PROG			= 0x02,
	OPCODE_E4K			= 0x20,
	OPCODE_E32K			= 0x52,
	OPCODE_E64K			= 0xd8,
	OPCODE_ENTER_4B		= 0xb7,
	OPCODE_EXIT_4B		= 0xe9,
};
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
	uint8_t	minor;
	uint8_t	major;
	uint8_t	table[16 * 4];
};

struct sfdp_t {
	struct sfdp_header_t h;
	struct sfdp_parameter_header_t ph[SFDP_MAX_NPH];
	struct sfdp_basic_table_t bt;
};

struct spi_flash_info_t {
	char * name;
	uint32_t id;
	uint32_t capacity;
	uint32_t blksz;
	uint32_t read_granularity;
	uint32_t write_granularity;
	uint8_t address_length;
	uint8_t opcode_read;
	uint8_t opcode_write;
	uint8_t opcode_write_enable;
	uint8_t opcode_erase_4k;
	uint8_t opcode_erase_32k;
	uint8_t opcode_erase_64k;
	uint8_t opcode_erase_256k;
};

struct spi_flash_pdata_t {
	struct spi_device_t * dev;
	struct spi_flash_info_t info;
};

static bool_t spi_flash_read_sfdp(struct spi_device_t * dev, struct sfdp_t * sfdp)
{
	uint32_t addr;
	uint8_t tx[5];
	int i, r;

	memset(sfdp, 0, sizeof(struct sfdp_t));
	tx[0] = OPCODE_SFDP;
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
	for(i = 0; i < sfdp->h.nph; i++)
	{
		addr = i * sizeof(struct sfdp_parameter_header_t) + sizeof(struct sfdp_header_t);
		tx[0] = OPCODE_SFDP;
		tx[1] = (addr >> 16) & 0xff;
		tx[2] = (addr >>  8) & 0xff;
		tx[3] = (addr >>  0) & 0xff;
		tx[4] = 0x0;
		spi_device_select(dev);
		r = spi_device_write_then_read(dev, tx, 5, &sfdp->ph[i], sizeof(struct sfdp_parameter_header_t));
		spi_device_deselect(dev);
		if(r < 0)
			return FALSE;
	}

	for(i = 0; i < sfdp->h.nph; i++)
	{
		if((sfdp->ph[i].idlsb == 0x00) && (sfdp->ph[i].idmsb == 0xff))
		{
			addr = (sfdp->ph[i].ptp[0] << 0) | (sfdp->ph[i].ptp[1] << 8) | (sfdp->ph[i].ptp[2] << 16);
			tx[0] = OPCODE_SFDP;
			tx[1] = (addr >> 16) & 0xff;
			tx[2] = (addr >>  8) & 0xff;
			tx[3] = (addr >>  0) & 0xff;
			tx[4] = 0x0;
			spi_device_select(dev);
			r = spi_device_write_then_read(dev, tx, 5, &sfdp->bt.table[0], sfdp->ph[i].length * 4);
			spi_device_deselect(dev);
			if(r >= 0)
			{
				sfdp->bt.major = sfdp->ph[i].major;
				sfdp->bt.minor = sfdp->ph[i].minor;
				return TRUE;
			}
		}
	}
	return FALSE;
}

static bool_t spi_flash_read_id(struct spi_device_t * dev, uint32_t * id)
{
	uint8_t tx[1];
	uint8_t rx[3];
	int r;

	tx[0] = OPCODE_RDID;
	spi_device_select(dev);
	r = spi_device_write_then_read(dev, tx, 1, rx, 3);
	spi_device_deselect(dev);
	if(r < 0)
		return FALSE;
	*id = (rx[0] << 16) | (rx[1] << 8) | (rx[2] << 0);
	return TRUE;
}

static const struct spi_flash_info_t spi_flash_infos[] = {
	{ "w25x40", 0xef3013, 512 * 1024, 4096, 1, 256, 3, OPCODE_READ, OPCODE_PROG, OPCODE_WREN, OPCODE_E4K, 0, OPCODE_E64K, 0 },
};

static bool_t spi_flash_detect(struct spi_device_t * dev, struct spi_flash_info_t * info)
{
	const struct spi_flash_info_t * t;
	struct sfdp_t sfdp;
	uint32_t v, id;
	int i;

	if(spi_flash_read_sfdp(dev, &sfdp))
	{
		info->name = "";
		info->id = 0;
		/* Basic flash parameter table 2th dword */
		v = (sfdp.bt.table[7] << 24) | (sfdp.bt.table[6] << 16) | (sfdp.bt.table[5] << 8) | (sfdp.bt.table[4] << 0);
		if(v & (1 << 31))
		{
			v &= 0x7fffffff;
			info->capacity = 1 << (v - 3);
		}
		else
		{
			info->capacity = (v + 1) >> 3;
		}
		/* Basic flash parameter table 1th dword */
		v = (sfdp.bt.table[3] << 24) | (sfdp.bt.table[2] << 16) | (sfdp.bt.table[1] << 8) | (sfdp.bt.table[0] << 0);
		if((info->capacity <= (16 * 1024 * 1024)) && (((v >> 17) & 0x3) != 0x2))
			info->address_length = 3;
		else
			info->address_length = 4;
		if(((v >> 0) & 0x3) == 0x1)
			info->opcode_erase_4k = (v >> 8) & 0xff;
		else
			info->opcode_erase_4k = 0x00;
		info->opcode_erase_32k = 0x00;
		info->opcode_erase_64k = 0x00;
		info->opcode_erase_256k = 0x00;
		/* Basic flash parameter table 8th dword */
		v = (sfdp.bt.table[31] << 24) | (sfdp.bt.table[30] << 16) | (sfdp.bt.table[29] << 8) | (sfdp.bt.table[28] << 0);
		switch((v >> 0) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 8) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 8) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 8) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 8) & 0xff;
			break;
		default:
			break;
		}
		switch((v >> 16) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 24) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 24) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 24) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 24) & 0xff;
			break;
		default:
			break;
		}
		/* Basic flash parameter table 9th dword */
		v = (sfdp.bt.table[35] << 24) | (sfdp.bt.table[34] << 16) | (sfdp.bt.table[33] << 8) | (sfdp.bt.table[32] << 0);
		switch((v >> 0) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 8) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 8) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 8) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 8) & 0xff;
			break;
		default:
			break;
		}
		switch((v >> 16) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 24) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 24) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 24) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 24) & 0xff;
			break;
		default:
			break;
		}
		if(info->opcode_erase_4k != 0x00)
			info->blksz = 4096;
		else if(info->opcode_erase_32k != 0x00)
			info->blksz = 32768;
		else if(info->opcode_erase_64k != 0x00)
			info->blksz = 65536;
		else if(info->opcode_erase_256k != 0x00)
			info->blksz = 262144;
		info->opcode_write_enable = OPCODE_WREN;
		info->read_granularity = 1;
		info->opcode_read = OPCODE_READ;
		if((sfdp.bt.major == 1) && (sfdp.bt.minor < 5))
		{
			/* Basic flash parameter table 1th dword */
			v = (sfdp.bt.table[3] << 24) | (sfdp.bt.table[2] << 16) | (sfdp.bt.table[1] << 8) | (sfdp.bt.table[0] << 0);
			if((v >> 2) & 0x1)
				info->write_granularity = 64;
			else
				info->write_granularity = 1;
		}
		else if((sfdp.bt.major == 1) && (sfdp.bt.minor >= 5))
		{
			/* Basic flash parameter table 11th dword */
			v = (sfdp.bt.table[43] << 24) | (sfdp.bt.table[42] << 16) | (sfdp.bt.table[41] << 8) | (sfdp.bt.table[40] << 0);
			info->write_granularity = 1 << ((v >> 4) & 0xf);
		}
		info->opcode_write = OPCODE_PROG;
		return TRUE;
	}
	else if(spi_flash_read_id(dev, &id))
	{
		for(i = 0; i < ARRAY_SIZE(spi_flash_infos); i++)
		{
			t = &spi_flash_infos[i];
			if(id == t->id)
			{
				memcpy(info, t, sizeof(struct spi_flash_info_t));
				return TRUE;
			}
		}
	}
	return FALSE;
}

static inline uint8_t spi_flash_read_status_register(struct spi_flash_pdata_t * pdat)
{
	uint8_t tx = OPCODE_RDSR;
	uint8_t rx = 0;

	spi_device_select(pdat->dev);
	spi_device_write_then_read(pdat->dev, &tx, 1, &rx, 1);
	spi_device_deselect(pdat->dev);
	return rx;
}

static inline void spi_flash_write_status_register(struct spi_flash_pdata_t * pdat, uint8_t sr)
{
	uint8_t tx[2];

	tx[0] = OPCODE_WRSR;
	tx[1] = sr;
	spi_device_select(pdat->dev);
	spi_device_write_then_read(pdat->dev, tx, 2, 0, 0);
	spi_device_deselect(pdat->dev);
}

static inline void spi_flash_write_enable(struct spi_flash_pdata_t * pdat)
{
	spi_device_select(pdat->dev);
	spi_device_write_then_read(pdat->dev, &pdat->info.opcode_write_enable, 1, 0, 0);
	spi_device_deselect(pdat->dev);
}

static inline void spi_flash_address_mode_4byte(struct spi_flash_pdata_t * pdat, int enable)
{
	uint8_t tx;

	if(enable)
		tx = OPCODE_ENTER_4B;
	else
		tx = OPCODE_EXIT_4B;
	spi_device_select(pdat->dev);
	spi_device_write_then_read(pdat->dev, &tx, 1, 0, 0);
	spi_device_deselect(pdat->dev);
}

static inline void spi_flash_chip_reset(struct spi_flash_pdata_t * pdat)
{
	uint8_t tx[2];

	tx[0] = 0x66;
	tx[1] = 0x99;
	spi_device_select(pdat->dev);
	spi_device_write_then_read(pdat->dev, tx, 2, 0, 0);
	spi_device_deselect(pdat->dev);
}

static inline void spi_flash_wait_for_busy(struct spi_flash_pdata_t * pdat)
{
	while((spi_flash_read_status_register(pdat) & 0x1) == 0x1);
}

static void spi_flash_read_bytes(struct spi_flash_pdata_t * pdat, u32_t addr, u8_t * buf, u32_t count)
{
	u8_t tx[5];

	switch(pdat->info.address_length)
	{
	case 3:
		tx[0] = pdat->info.opcode_read;
		tx[1] = (u8_t)(addr >> 16);
		tx[2] = (u8_t)(addr >> 8);
		tx[3] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 4, buf, count);
		spi_device_deselect(pdat->dev);
		break;

	case 4:
		tx[0] = pdat->info.opcode_read;
		tx[1] = (u8_t)(addr >> 24);
		tx[2] = (u8_t)(addr >> 16);
		tx[3] = (u8_t)(addr >> 8);
		tx[4] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 5, buf, count);
		spi_device_deselect(pdat->dev);
		break;

	default:
		break;
	}
}

static void spi_flash_write_bytes(struct spi_flash_pdata_t * pdat, u32_t addr, u8_t * buf, u32_t count)
{
	u8_t tx[5];

	switch(pdat->info.address_length)
	{
	case 3:
		tx[0] = pdat->info.opcode_write;
		tx[1] = (u8_t)(addr >> 16);
		tx[2] = (u8_t)(addr >> 8);
		tx[3] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 4, 0, 0);
		spi_device_write_then_read(pdat->dev, buf, count, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	case 4:
		tx[0] = pdat->info.opcode_write;
		tx[1] = (u8_t)(addr >> 24);
		tx[2] = (u8_t)(addr >> 16);
		tx[3] = (u8_t)(addr >> 8);
		tx[4] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 5, 0, 0);
		spi_device_write_then_read(pdat->dev, buf, count, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	default:
		break;
	}
}

static void spi_flash_sector_erase_4k(struct spi_flash_pdata_t * pdat, u32_t addr)
{
	u8_t tx[5];

	switch(pdat->info.address_length)
	{
	case 3:
		tx[0] = pdat->info.opcode_erase_4k;
		tx[1] = (u8_t)(addr >> 16);
		tx[2] = (u8_t)(addr >> 8);
		tx[3] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 4, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	case 4:
		tx[0] = pdat->info.opcode_erase_4k;
		tx[1] = (u8_t)(addr >> 24);
		tx[2] = (u8_t)(addr >> 16);
		tx[3] = (u8_t)(addr >> 8);
		tx[4] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 5, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	default:
		break;
	}
}

static void spi_flash_sector_erase_32k(struct spi_flash_pdata_t * pdat, u32_t addr)
{
	u8_t tx[5];

	switch(pdat->info.address_length)
	{
	case 3:
		tx[0] = pdat->info.opcode_erase_32k;
		tx[1] = (u8_t)(addr >> 16);
		tx[2] = (u8_t)(addr >> 8);
		tx[3] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 4, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	case 4:
		tx[0] = pdat->info.opcode_erase_32k;
		tx[1] = (u8_t)(addr >> 24);
		tx[2] = (u8_t)(addr >> 16);
		tx[3] = (u8_t)(addr >> 8);
		tx[4] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 5, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	default:
		break;
	}
}

static void spi_flash_sector_erase_64k(struct spi_flash_pdata_t * pdat, u32_t addr)
{
	u8_t tx[5];

	switch(pdat->info.address_length)
	{
	case 3:
		tx[0] = pdat->info.opcode_erase_64k;
		tx[1] = (u8_t)(addr >> 16);
		tx[2] = (u8_t)(addr >> 8);
		tx[3] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 4, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	case 4:
		tx[0] = pdat->info.opcode_erase_64k;
		tx[1] = (u8_t)(addr >> 24);
		tx[2] = (u8_t)(addr >> 16);
		tx[3] = (u8_t)(addr >> 8);
		tx[4] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 5, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	default:
		break;
	}
}

static void spi_flash_sector_erase_256k(struct spi_flash_pdata_t * pdat, u32_t addr)
{
	u8_t tx[5];

	switch(pdat->info.address_length)
	{
	case 3:
		tx[0] = pdat->info.opcode_erase_256k;
		tx[1] = (u8_t)(addr >> 16);
		tx[2] = (u8_t)(addr >> 8);
		tx[3] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 4, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	case 4:
		tx[0] = pdat->info.opcode_erase_256k;
		tx[1] = (u8_t)(addr >> 24);
		tx[2] = (u8_t)(addr >> 16);
		tx[3] = (u8_t)(addr >> 8);
		tx[4] = (u8_t)(addr >> 0);
		spi_device_select(pdat->dev);
		spi_device_write_then_read(pdat->dev, tx, 5, 0, 0);
		spi_device_deselect(pdat->dev);
		break;

	default:
		break;
	}
}

static void spi_flash_init(struct spi_flash_pdata_t * pdat)
{
	spi_flash_chip_reset(pdat);
	spi_flash_wait_for_busy(pdat);
	spi_flash_write_enable(pdat);
	spi_flash_write_status_register(pdat, 0);
	spi_flash_wait_for_busy(pdat);
	if(pdat->info.address_length == 4)
	{
		spi_flash_write_enable(pdat);
		spi_flash_address_mode_4byte(pdat, 1);
		spi_flash_wait_for_busy(pdat);
	}
}

static u64_t spi_flash_read(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct spi_flash_pdata_t * pdat = (struct spi_flash_pdata_t *)blk->priv;
	u64_t addr = blkno * blk->blksz;
	s64_t cnt = blkcnt * blk->blksz;
	u8_t * pbuf = buf;
	u32_t len;

	if(pdat->info.read_granularity == 1)
		len = (cnt < 0x7fffffff) ? cnt : 0x7fffffff;
	else
		len = pdat->info.read_granularity;
	while(cnt > 0)
	{
		spi_flash_wait_for_busy(pdat);
		spi_flash_read_bytes(pdat, addr, pbuf, len);
		addr += len;
		pbuf += len;
		cnt -= len;
	}

	return blkcnt;
}

static u64_t spi_flash_write(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt)
{
	struct spi_flash_pdata_t * pdat = (struct spi_flash_pdata_t *)blk->priv;
	u64_t addr, baddr = blkno * blk->blksz;
	s64_t cnt, count = blkcnt * blk->blksz;
	u32_t len;
	u8_t * pbuf;

	addr = baddr;
	cnt = count;
	spi_flash_wait_for_busy(pdat);
	while(cnt > 0)
	{
		if((pdat->info.opcode_erase_256k != 0) && ((addr & 0x3ffff) == 0) && (cnt >= 262144))
		{
			len = 262144;
			spi_flash_write_enable(pdat);
			spi_flash_sector_erase_256k(pdat, addr);
			spi_flash_wait_for_busy(pdat);
		}
		else if((pdat->info.opcode_erase_64k != 0) && ((addr & 0xffff) == 0) && (cnt >= 65536))
		{
			len = 65536;
			spi_flash_write_enable(pdat);
			spi_flash_sector_erase_64k(pdat, addr);
			spi_flash_wait_for_busy(pdat);
		}
		else if((pdat->info.opcode_erase_32k != 0) && ((addr & 0x7fff) == 0) && (cnt >= 32768))
		{
			len = 32768;
			spi_flash_write_enable(pdat);
			spi_flash_sector_erase_32k(pdat, addr);
			spi_flash_wait_for_busy(pdat);
		}
		else if((pdat->info.opcode_erase_4k != 0) && ((addr & 0xfff) == 0) && (cnt >= 4096))
		{
			len = 4096;
			spi_flash_write_enable(pdat);
			spi_flash_sector_erase_4k(pdat, addr);
			spi_flash_wait_for_busy(pdat);
		}
		else
		{
			return 0;
		}
		addr += len;
		cnt -= len;
	}

	addr = baddr;
	cnt = count;
	pbuf = buf;
	if(pdat->info.write_granularity == 1)
		len = (cnt < 0x7fffffff) ? cnt : 0x7fffffff;
	else
		len = pdat->info.write_granularity;
	spi_flash_wait_for_busy(pdat);
	while(cnt > 0)
	{
		spi_flash_write_enable(pdat);
		spi_flash_write_bytes(pdat, addr, pbuf, len);
		spi_flash_wait_for_busy(pdat);
		addr += len;
		pbuf += len;
		cnt -= len;
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
	struct spi_flash_info_t info;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	if(!spi_flash_detect(spidev, &info))
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
	memcpy(&pdat->info, &info, sizeof(struct spi_flash_info_t));

	blk->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	blk->blksz = pdat->info.blksz;
	blk->blkcnt = pdat->info.capacity / pdat->info.blksz;
	blk->read = spi_flash_read;
	blk->write = spi_flash_write;
	blk->sync = spi_flash_sync;
	blk->priv = pdat;
	spi_flash_init(pdat);

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
