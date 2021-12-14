/*
 * driver/block/blk-spinand.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#define SPINAND_ID(...)			{ .val = { __VA_ARGS__ }, .len = sizeof((uint8_t[]){ __VA_ARGS__ }) }

enum {
	OPCODE_RDID					= 0x9f,
	OPCODE_GET_FEATURE			= 0x0f,
	OPCODE_SET_FEATURE			= 0x1f,
	OPCODE_FEATURE_PROTECT		= 0xa0,
	OPCODE_FEATURE_CONFIG		= 0xb0,
	OPCODE_FEATURE_STATUS		= 0xc0,
	OPCODE_READ_PAGE_TO_CACHE	= 0x13,
	OPCODE_READ_PAGE_FROM_CACHE	= 0x03,
	OPCODE_WRITE_ENABLE			= 0x06,
	OPCODE_BLOCK_ERASE			= 0xd8,
	OPCODE_PROGRAM_LOAD			= 0x02,
	OPCODE_PROGRAM_EXEC			= 0x10,
	OPCODE_RESET				= 0xff,
};

struct spinand_info_t {
	char * name;
	struct {
		uint8_t val[4];
		uint8_t len;
	} id;
	uint32_t page_size;
	uint32_t spare_size;
	uint32_t pages_per_block;
	uint32_t blocks_per_die;
	uint32_t planes_per_die;
	uint32_t ndies;
};

struct blk_spinand_pdata_t {
	struct spi_device_t * dev;
	struct spinand_info_t info;
	u8_t * buf;
};

static const struct spinand_info_t spinand_infos[] = {
	/* Winbond */
	{ "W25N512GV",       SPINAND_ID(0xef, 0xaa, 0x20), 2048,  64,  64,  512, 1, 1 },
	{ "W25N01GV",        SPINAND_ID(0xef, 0xaa, 0x21), 2048,  64,  64, 1024, 1, 1 },
	{ "W25M02GV",        SPINAND_ID(0xef, 0xab, 0x21), 2048,  64,  64, 1024, 1, 2 },
	{ "W25N02KV",        SPINAND_ID(0xef, 0xaa, 0x22), 2048, 128,  64, 2048, 1, 1 },

	/* Gigadevice */
	{ "GD5F1GQ4UAWxx",   SPINAND_ID(0xc8, 0x10),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ5UExxG",   SPINAND_ID(0xc8, 0x51),       2048, 128,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExIG",   SPINAND_ID(0xc8, 0xd1),       2048, 128,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExxH",   SPINAND_ID(0xc8, 0xd9),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ4xAYIG",   SPINAND_ID(0xc8, 0xf1),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F2GQ4UExIG",   SPINAND_ID(0xc8, 0xd2),       2048, 128,  64, 2048, 1, 1 },
	{ "GD5F2GQ5UExxH",   SPINAND_ID(0xc8, 0x32),       2048,  64,  64, 2048, 1, 1 },
	{ "GD5F2GQ4xAYIG",   SPINAND_ID(0xc8, 0xf2),       2048,  64,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UBxIG",   SPINAND_ID(0xc8, 0xd4),       4096, 256,  64, 2048, 1, 1 },
	{ "GD5F4GQ4xAYIG",   SPINAND_ID(0xc8, 0xf4),       2048,  64,  64, 4096, 1, 1 },
	{ "GD5F2GQ5UExxG",   SPINAND_ID(0xc8, 0x52),       2048, 128,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UCxIG",   SPINAND_ID(0xc8, 0xb4),       4096, 256,  64, 2048, 1, 1 },

	/* Macronix */
	{ "MX35LF1GE4AB",    SPINAND_ID(0xc2, 0x12),       2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF1G24AD",    SPINAND_ID(0xc2, 0x14),       2048, 128,  64, 1024, 1, 1 },
	{ "MX31LF1GE4BC",    SPINAND_ID(0xc2, 0x1e),       2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF2GE4AB",    SPINAND_ID(0xc2, 0x22),       2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF2G24AD",    SPINAND_ID(0xc2, 0x24),       2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2GE4AD",    SPINAND_ID(0xc2, 0x26),       2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2G14AC",    SPINAND_ID(0xc2, 0x20),       2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF4G24AD",    SPINAND_ID(0xc2, 0x35),       4096, 256,  64, 2048, 1, 1 },
	{ "MX35LF4GE4AD",    SPINAND_ID(0xc2, 0x37),       4096, 256,  64, 2048, 1, 1 },

	/* Micron */
	{ "MT29F1G01AAADD",  SPINAND_ID(0x2c, 0x12),       2048,  64,  64, 1024, 1, 1 },
	{ "MT29F1G01ABAFD",  SPINAND_ID(0x2c, 0x14),       2048, 128,  64, 1024, 1, 1 },
	{ "MT29F2G01AAAED",  SPINAND_ID(0x2c, 0x9f),       2048,  64,  64, 2048, 2, 1 },
	{ "MT29F2G01ABAGD",  SPINAND_ID(0x2c, 0x24),       2048, 128,  64, 2048, 2, 1 },
	{ "MT29F4G01AAADD",  SPINAND_ID(0x2c, 0x32),       2048,  64,  64, 4096, 2, 1 },
	{ "MT29F4G01ABAFD",  SPINAND_ID(0x2c, 0x34),       4096, 256,  64, 2048, 1, 1 },
	{ "MT29F4G01ADAGD",  SPINAND_ID(0x2c, 0x36),       2048, 128,  64, 2048, 2, 2 },
	{ "MT29F8G01ADAFD",  SPINAND_ID(0x2c, 0x46),       4096, 256,  64, 2048, 1, 2 },

	/* Toshiba */
	{ "TC58CVG0S3HRAIG", SPINAND_ID(0x98, 0xc2),       2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIG", SPINAND_ID(0x98, 0xcb),       2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIG", SPINAND_ID(0x98, 0xcd),       4096, 256,  64, 2048, 1, 1 },
	{ "TC58CVG0S3HRAIJ", SPINAND_ID(0x98, 0xe2),       2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIJ", SPINAND_ID(0x98, 0xeb),       2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIJ", SPINAND_ID(0x98, 0xed),       4096, 256,  64, 2048, 1, 1 },
	{ "TH58CVG3S0HRAIJ", SPINAND_ID(0x98, 0xe4),       4096, 256,  64, 4096, 1, 1 },

	/* Esmt */
	{ "F50L512M41A",     SPINAND_ID(0xc8, 0x20),       2048,  64,  64,  512, 1, 1 },
	{ "F50L1G41A",       SPINAND_ID(0xc8, 0x21),       2048,  64,  64, 1024, 1, 1 },
	{ "F50L1G41LB",      SPINAND_ID(0xc8, 0x01),       2048,  64,  64, 1024, 1, 1 },
	{ "F50L2G41LB",      SPINAND_ID(0xc8, 0x0a),       2048,  64,  64, 1024, 1, 2 },

	/* Fison */
	{ "CS11G0T0A0AA",    SPINAND_ID(0x6b, 0x00),       2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0G0A0AA",    SPINAND_ID(0x6b, 0x10),       2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0S0A0AA",    SPINAND_ID(0x6b, 0x20),       2048,  64,  64, 1024, 1, 1 },
	{ "CS11G1T0A0AA",    SPINAND_ID(0x6b, 0x01),       2048, 128,  64, 2048, 1, 1 },
	{ "CS11G1S0A0AA",    SPINAND_ID(0x6b, 0x21),       2048,  64,  64, 2048, 1, 1 },
	{ "CS11G2T0A0AA",    SPINAND_ID(0x6b, 0x02),       2048, 128,  64, 4096, 1, 1 },
	{ "CS11G2S0A0AA",    SPINAND_ID(0x6b, 0x22),       2048,  64,  64, 4096, 1, 1 },

	/* Etron */
	{ "EM73B044VCA",     SPINAND_ID(0xd5, 0x01),       2048,  64,  64,  512, 1, 1 },
	{ "EM73C044SNB",     SPINAND_ID(0xd5, 0x11),       2048, 120,  64, 1024, 1, 1 },
	{ "EM73C044SNF",     SPINAND_ID(0xd5, 0x09),       2048, 128,  64, 1024, 1, 1 },
	{ "EM73C044VCA",     SPINAND_ID(0xd5, 0x18),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNA",     SPINAND_ID(0xd5, 0x19),       2048,  64, 128,  512, 1, 1 },
	{ "EM73C044VCD",     SPINAND_ID(0xd5, 0x1c),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SND",     SPINAND_ID(0xd5, 0x1d),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73D044SND",     SPINAND_ID(0xd5, 0x1e),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73C044VCC",     SPINAND_ID(0xd5, 0x22),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044VCF",     SPINAND_ID(0xd5, 0x25),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNC",     SPINAND_ID(0xd5, 0x31),       2048, 128,  64, 1024, 1, 1 },
	{ "EM73D044SNC",     SPINAND_ID(0xd5, 0x0a),       2048, 120,  64, 2048, 1, 1 },
	{ "EM73D044SNA",     SPINAND_ID(0xd5, 0x12),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNF",     SPINAND_ID(0xd5, 0x10),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCA",     SPINAND_ID(0xd5, 0x13),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCB",     SPINAND_ID(0xd5, 0x14),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCD",     SPINAND_ID(0xd5, 0x17),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCH",     SPINAND_ID(0xd5, 0x1b),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044SND",     SPINAND_ID(0xd5, 0x1d),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCG",     SPINAND_ID(0xd5, 0x1f),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCE",     SPINAND_ID(0xd5, 0x20),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCL",     SPINAND_ID(0xd5, 0x2e),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNB",     SPINAND_ID(0xd5, 0x32),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73E044SNA",     SPINAND_ID(0xd5, 0x03),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044SND",     SPINAND_ID(0xd5, 0x0b),       4096, 240,  64, 2048, 1, 1 },
	{ "EM73E044SNB",     SPINAND_ID(0xd5, 0x23),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCA",     SPINAND_ID(0xd5, 0x2c),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCB",     SPINAND_ID(0xd5, 0x2f),       2048, 128,  64, 4096, 1, 1 },
	{ "EM73F044SNA",     SPINAND_ID(0xd5, 0x24),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73F044VCA",     SPINAND_ID(0xd5, 0x2d),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73E044SNE",     SPINAND_ID(0xd5, 0x0e),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73C044SNG",     SPINAND_ID(0xd5, 0x0c),       2048, 120,  64, 1024, 1, 1 },
	{ "EM73D044VCN",     SPINAND_ID(0xd5, 0x0f),       2048,  64,  64, 2048, 1, 1 },

	/* Elnec */
	{ "FM35Q1GA",        SPINAND_ID(0xe5, 0x71),       2048,  64,  64, 1024, 1, 1 },

	/* Paragon */
	{ "PN26G01A",        SPINAND_ID(0xa1, 0xe1),       2048, 128,  64, 1024, 1, 1 },
	{ "PN26G02A",        SPINAND_ID(0xa1, 0xe2),       2048, 128,  64, 2048, 1, 1 },

	/* Ato */
	{ "ATO25D1GA",       SPINAND_ID(0x9b, 0x12),       2048,  64,  64, 1024, 1, 1 },

	/* Heyang */
	{ "HYF1GQ4U",        SPINAND_ID(0xc9, 0x51),       2048, 128,  64, 1024, 1, 1 },
	{ "HYF2GQ4U",        SPINAND_ID(0xc9, 0x52),       2048, 128,  64, 2048, 1, 1 },
};

static inline int spinand_info(struct spi_device_t * dev, struct spinand_info_t * info)
{
	const struct spinand_info_t * t;
	uint8_t tx[2];
	uint8_t rx[4];
	int i, r;

	tx[0] = OPCODE_RDID;
	tx[1] = 0x0;
	spi_device_select(dev);
	r = spi_device_write_then_read(dev, tx, 2, rx, 4);
	spi_device_deselect(dev);
	if(r < 0)
		return 0;
	for(i = 0; i < ARRAY_SIZE(spinand_infos); i++)
	{
		t = &spinand_infos[i];
		if(memcmp(rx, t->id.val, t->id.len) == 0)
		{
			memcpy(info, t, sizeof(struct spinand_info_t));
			return 1;
		}
	}
	tx[0] = OPCODE_RDID;
	spi_device_select(dev);
	r = spi_device_write_then_read(dev, tx, 1, rx, 4);
	spi_device_deselect(dev);
	if(r < 0)
		return 0;
	for(i = 0; i < ARRAY_SIZE(spinand_infos); i++)
	{
		t = &spinand_infos[i];
		if(memcmp(rx, t->id.val, t->id.len) == 0)
		{
			memcpy(info, t, sizeof(struct spinand_info_t));
			return 1;
		}
	}
	LOG("The spi nand flash '0x%02x%02x%02x%02x' is not yet supported", rx[0], rx[1], rx[2], rx[3]);
	return 0;
}

static inline int spinand_reset(struct spi_device_t * dev)
{
	uint8_t tx[1];
	int r;

	tx[0] = OPCODE_RESET;
	spi_device_select(dev);
	r = spi_device_write_then_read(dev, tx, 1, 0, 0);
	spi_device_deselect(dev);
	if(r < 0)
		return 0;
	udelay(100 * 1000);
	return 1;
}

static inline int spinand_get_feature(struct spi_device_t * dev, uint8_t addr, uint8_t * val)
{
	uint8_t tx[2];
	int r;

	tx[0] = OPCODE_GET_FEATURE;
	tx[1] = addr;
	spi_device_select(dev);
	r = spi_device_write_then_read(dev, tx, 2, val, 1);
	spi_device_deselect(dev);
	if(r < 0)
		return 0;
	return 1;
}

static inline int spinand_set_feature(struct spi_device_t * dev, uint8_t addr, uint8_t val)
{
	uint8_t tx[3];
	int r;

	tx[0] = OPCODE_SET_FEATURE;
	tx[1] = addr;
	tx[2] = val;
	spi_device_select(dev);
	r = spi_device_write_then_read(dev, tx, 3, 0, 0);
	spi_device_deselect(dev);
	if(r < 0)
		return 0;
	return 1;
}

static inline void spinand_wait_for_busy(struct spi_device_t * dev)
{
	uint8_t tx[2];
	uint8_t rx[1];
	int r;

	tx[0] = OPCODE_GET_FEATURE;
	tx[1] = 0xc0;
	rx[0] = 0x00;
	do {
		spi_device_select(dev);
		r = spi_device_write_then_read(dev, tx, 2, rx, 1);
		spi_device_deselect(dev);
		if(r < 0)
			break;
	} while((rx[0] & 0x1) == 0x1);
}

static bool_t blk_spinand_detect(struct spi_device_t * dev, struct spinand_info_t * info)
{
	uint8_t val;

	if(spinand_info(dev, info))
	{
		spinand_reset(dev);
		spinand_wait_for_busy(dev);
		if(spinand_get_feature(dev, OPCODE_FEATURE_PROTECT, &val) && (val != 0x0))
		{
			spinand_set_feature(dev, OPCODE_FEATURE_PROTECT, 0x0);
			spinand_wait_for_busy(dev);
		}
		return TRUE;
	}
	return FALSE;
}

static u64_t blk_spinand_capacity(struct block_t * blk)
{
	struct blk_spinand_pdata_t * pdat = (struct blk_spinand_pdata_t *)blk->priv;
	return pdat->info.page_size * pdat->info.pages_per_block * pdat->info.blocks_per_die * pdat->info.ndies;
}

static u64_t blk_spinand_read(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	struct blk_spinand_pdata_t * pdat = (struct blk_spinand_pdata_t *)blk->priv;
	uint32_t addr = offset;
	uint32_t cnt = count;
	uint32_t len = 0;
	uint32_t pa, ca;
	uint32_t n;
	uint8_t tx[4];
	int r;

	while(cnt > 0)
	{
		pa = addr / pdat->info.page_size;
		ca = addr & (pdat->info.page_size - 1);
		n = cnt > (pdat->info.page_size - ca) ? (pdat->info.page_size - ca) : cnt;
		tx[0] = OPCODE_READ_PAGE_TO_CACHE;
		tx[1] = (uint8_t)(pa >> 16);
		tx[2] = (uint8_t)(pa >> 8);
		tx[3] = (uint8_t)(pa >> 0);
		spi_device_select(pdat->dev);
		r = spi_device_write_then_read(pdat->dev, tx, 4, 0, 0);
		spi_device_deselect(pdat->dev);
		if(r < 0)
			break;
		spinand_wait_for_busy(pdat->dev);
		tx[0] = OPCODE_READ_PAGE_FROM_CACHE;
		tx[1] = (uint8_t)(ca >> 8);
		tx[2] = (uint8_t)(ca >> 0);
		tx[3] = 0x0;
		spi_device_select(pdat->dev);
		r = spi_device_write_then_read(pdat->dev, tx, 4, buf, n);
		spi_device_deselect(pdat->dev);
		if(r < 0)
			break;
		spinand_wait_for_busy(pdat->dev);
		addr += n;
		buf += n;
		len += n;
		cnt -= n;
	}
	return len;
}

//TODO fixme for erase...
static u64_t blk_spinand_write(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count)
{
	struct blk_spinand_pdata_t * pdat = (struct blk_spinand_pdata_t *)blk->priv;
	uint32_t addr = offset;
	uint32_t cnt = count;
	uint32_t len = 0;
	uint32_t pa, ca;
	uint32_t n;
	uint8_t tx[4099];
	int r;

	while(cnt > 0)
	{
		pa = addr / pdat->info.page_size;
		ca = addr & (pdat->info.page_size - 1);
		n = cnt > (pdat->info.page_size - ca) ? (pdat->info.page_size - ca) : cnt;
		tx[0] = OPCODE_WRITE_ENABLE;
		spi_device_select(pdat->dev);
		r = spi_device_write_then_read(pdat->dev, tx, 1, 0, 0);
		spi_device_deselect(pdat->dev);
		if(r < 0)
			break;
		spinand_wait_for_busy(pdat->dev);
		tx[0] = OPCODE_PROGRAM_LOAD;
		tx[1] = (uint8_t)(ca >> 8);
		tx[2] = (uint8_t)(ca >> 0);
		memcpy(&tx[3], buf, n);
		spi_device_select(pdat->dev);
		r = spi_device_write_then_read(pdat->dev, tx, 3 + n, 0, 0);
		spi_device_deselect(pdat->dev);
		if(r < 0)
			break;
		spinand_wait_for_busy(pdat->dev);
		tx[0] = OPCODE_PROGRAM_EXEC;
		tx[1] = (uint8_t)(pa >> 16);
		tx[2] = (uint8_t)(pa >> 8);
		tx[3] = (uint8_t)(pa >> 0);
		spi_device_select(pdat->dev);
		r = spi_device_write_then_read(pdat->dev, tx, 4, 0, 0);
		spi_device_deselect(pdat->dev);
		if(r < 0)
			break;
		spinand_wait_for_busy(pdat->dev);
		addr += n;
		buf += n;
		len += n;
		cnt -= n;
	}
	return len;
}

static void blk_spinand_sync(struct block_t * blk)
{
}

static struct device_t * blk_spinand_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct blk_spinand_pdata_t * pdat;
	struct block_t * blk, * sblk;
	struct device_t * dev, * sdev;
	struct dtnode_t o;
	struct spi_device_t * spidev;
	struct spinand_info_t info;
	char nbuf[64];
	char sbuf[64];
	int npart, i;

	spidev = spi_device_alloc(dt_read_string(n, "spi-bus", NULL), dt_read_int(n, "chip-select", 0), dt_read_int(n, "type", 0), dt_read_int(n, "mode", 0), 8, dt_read_int(n, "speed", 0));
	if(!spidev)
		return NULL;

	if(!blk_spinand_detect(spidev, &info))
	{
		spi_device_free(spidev);
		return NULL;
	}

	pdat = malloc(sizeof(struct blk_spinand_pdata_t));
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
	pdat->buf = malloc(info.page_size * info.pages_per_block);
	memcpy(&pdat->info, &info, sizeof(struct spinand_info_t));

	blk->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	blk->capacity = blk_spinand_capacity;
	blk->read = blk_spinand_read;
	blk->write = blk_spinand_write;
	blk->sync = blk_spinand_sync;
	blk->priv = pdat;

	if(!(dev = register_block(blk, drv)))
	{
		free(pdat->buf);
		spi_device_free(pdat->dev);
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
		return NULL;
	}
	LOG("Found spi nand flash '%s' with %s", info.name, ssize(sbuf, block_capacity(blk)));
	if((npart = dt_read_array_length(n, "partition")) > 0)
	{
		LOG("Found partition:");
		LOG("  0x%016Lx ~ 0x%016Lx %s %*s- %s", 0ULL, block_capacity(blk) - 1, ssize(sbuf, block_capacity(blk)), 9 - strlen(sbuf), "", blk->name);
		for(i = 0; i < npart; i++)
		{
			dt_read_array_object(n, "partition", i, &o);
			char * name = dt_read_string(&o, "name", NULL);
			if(!name)
			{
				snprintf(nbuf, sizeof(nbuf), "p%d", i);
				name = nbuf;
			}
			u64_t offset = dt_read_long(&o, "offset", 0);
			if(offset < block_capacity(blk))
			{
				u64_t length = dt_read_long(&o, "length", 0);
				u64_t maxlen = block_capacity(blk) - offset;
				if((length <= 0) || (length > maxlen))
					length = maxlen;
				sdev = register_sub_block(blk, offset, length, name);
				if(sdev)
				{
					sblk = (struct block_t *)sdev->priv;
					LOG("  0x%016Lx ~ 0x%016Lx %s %*s- %s", offset, offset + length - 1, ssize(sbuf, length), 9 - strlen(sbuf), "", sblk->name);
				}
			}
		}
	}
	return dev;
}

static void blk_spinand_remove(struct device_t * dev)
{
	struct block_t * blk = (struct block_t *)dev->priv;
	struct blk_spinand_pdata_t * pdat = (struct blk_spinand_pdata_t *)blk->priv;

	if(blk)
	{
		unregister_sub_block(blk);
		unregister_block(blk);
		free(pdat->buf);
		spi_device_free(pdat->dev);
		free_device_name(blk->name);
		free(blk->priv);
		free(blk);
	}
}

static void blk_spinand_suspend(struct device_t * dev)
{
}

static void blk_spinand_resume(struct device_t * dev)
{
}

static struct driver_t blk_spinand = {
	.name		= "blk-spinand",
	.probe		= blk_spinand_probe,
	.remove		= blk_spinand_remove,
	.suspend	= blk_spinand_suspend,
	.resume		= blk_spinand_resume,
};

static __init void blk_spinand_driver_init(void)
{
	register_driver(&blk_spinand);
}

static __exit void blk_spinand_driver_exit(void)
{
	unregister_driver(&blk_spinand);
}

driver_initcall(blk_spinand_driver_init);
driver_exitcall(blk_spinand_driver_exit);
