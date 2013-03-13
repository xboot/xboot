/*
 * arch/arm/mach-x6410/s3c6410-irom.c
 *
 * this file used by start.s assembler code, and the linker script
 * must make sure this file is linked within the first 4kB. DO NOT
 * use any .bss segment and .data segment, just use irom's small
 * stack. it's just a small c routline.
 *
 * NOTE:
 *     please do not use 'switch, case' instead of 'if, else if',
 *     because switch statement may be compiled as a jump table,
 *     which belong to .data segment, if it has many cases.
 *
 *     to make sure this file is linked within the first 4KB, please
 *     look at the file of xboot.map.
 *
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
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-nand.h>
#include <s3c6410/reg-mmc.h>
#include <s3c6410/reg-serial.h>


extern u8_t	__text_start[];
extern u8_t __text_end[];
extern u8_t __data_shadow_start[];
extern u8_t __data_shadow_end[];
extern u8_t __data_start[];
extern u8_t __data_end[];
extern u8_t __bss_start[];
extern u8_t __bss_end[];
extern u8_t __heap_start[];
extern u8_t __heap_end[];
extern u8_t __stack_start[];
extern u8_t __stack_end[];

/*
 * sdhc information.
 *
 * [31:16] RCA address
 * [2] if sd card detected, this value will be set.
 * [1] if mmc card detected, this value will be set.
 * [0] if the sd/mmc device is operating in sector mode, this value will be set.
 */
#define irom_movi_high_capcaity									\
		(*((volatile u32_t *)(0x0c004000 - 0x8)))

/*
 * total block count of the MMC device
 */
#define irom_movi_total_blkcnt									\
		(*((volatile u32_t *)(0x0c004000 - 0x4)))

/*
 * this function copies a block of page to destination memory
 * 8-bit ecc check, 512 bytes page size only.
 *
 * @param u32_t block : source block address number to copy.
 * @param u32_t page  : source page address number to copy.
 * @param u8_t * mem  : target memory pointer.
 * @return s32_t - success or failure.
 */
#define	irom_nand_readpage(block, page, mem)					\
		(((s32_t(*)(u32_t, u32_t, u8_t *))(*((u32_t *)(0x0c004000+0x00))))(block, page, mem))

/*
 * this function copies a block of page to destination memory
 * 8-bit ecc check, 2KB and 4KB page size only.
 *
 * @param u32_t blcok : source block address number to copy.
 * @param u32_t page  : source page address number to copy.
 * @param u8_t * mem  : target memory pointer.
 * @return s32_t - success or failure.
*/
#define	irom_nand_readpage_advanced(block, page, mem)			\
		(((s32_t(*)(u32_t, u32_t, u8_t *))(*((u32_t *)(0x0c004000+0x04))))(block, page, mem))

/*
 * this function copies SD/MMC card data to memory.
 * always use EPLL source clock.
 *
 * @param ch 	 : HSMMC controller channel number (not support. depend on GPN15, GPN14 and GPN13).
 * @param sector : source card(SD/MMC) address (it must block address).
 * @param count  : number of blocks to copy.
 * @param mem    : memory to copy to.
 * @param init 	 : reinitialize or not.
 * @return bool(u8_t) - success or failure.
 */
#define irom_movi_to_mem(ch, sector, count, mem, init)			\
		(((u8_t(*)(s32_t, u32_t, u16_t, u32_t *, s32_t))(*((u32_t *)(0x0c004000+0x08))))(ch, sector, count, mem, init))

/*
 * onenand single word transfer.
 *
 * @param u32_t ctl - onenand controller number ('0' fixed).
 * @param u32_t block - block number to read.
 * @param u8_t page - page number to read.
 * @param u32_t * mem - destination address.
 * @return bool(u8_t) - success or failure.
 */
#define irom_onenand_readpage(ctl, block, page, mem)			\
		(((u8_t(*)(u32_t, u32_t, u8_t, u32_t *))(*((u32_t *)(0x0c004000+0x0c))))(ctl, block, page, mem))

/*
 * onenand 4 burst word transfer (for enhanced read performance)
 *
 * @param u32_t ctl - onenand controller number ('0' fixed).
 * @param u32_t block - block number to read.
 * @param u8_t page - page number to read.
 * @param u32_t * mem - destination address.
 * @return bool(u8_t) - success or failure.
 */
#define irom_onenand_readpage_4burst(ctl, block, page, mem)		\
		(((u8_t(*)(u32_t, u32_t, u8_t, u32_t *))(*((u32_t *)(0x0c004000+0x10))))(ctl, block, page, mem))

/*
 * onenand 8 burst word transfer (for enhanced read performance)
 *
 * @param u32_t ctl - onenand controller number ('0' fixed).
 * @param u32_t block - block number to read.
 * @param u8_t page - page number to read.
 * @param u32_t * mem - destination address.
 * @return bool(u8_t) - success or failure.
 */
#define irom_onenand_readpage_8burst(ctl, block, page, mem)		\
		(((u8_t(*)(u32_t, u32_t, u8_t, u32_t *))(*((u32_t *)(0x0c004000+0x14))))(ctl, block, page, mem))

/*
 * write a 32-bits value to register.
 */
static void reg_write(u32_t addr, u32_t value)
{
	( *((volatile u32_t *)(addr)) ) = value;
}

/*
 * read a 32-bits value from register.
 */
static u32_t reg_read(u32_t addr)
{
	return( *((volatile u32_t *)(addr)) );
}

/*
 * only support irom bootting.
 */
void irom_copyself(void)
{
	u8_t gpn;
	u32_t * mem;
	u32_t page, block, size;

	/* GPN15, GPN14, GPN13 */
	reg_write(S3C6410_GPNCON, reg_read(S3C6410_GPNCON)&0x03ffffff);
	reg_write(S3C6410_GPNPUD, reg_read(S3C6410_GPNPUD)&0x03ffffff);
	gpn = (u8_t)((reg_read(S3C6410_GPNDAT)>>13)&0x7);

	/* sd-mmc(ch0) */
	if(gpn == 0x0)
	{
		/*
		 * high speed mmc0 initialize.
		 */
		reg_write(S3C6410_HM_CONTROL4_0, reg_read(S3C6410_HM_CONTROL4_0)|(0x3<<16));

		/*
		 * the xboot's memory base address.
		 */
		mem = (u32_t *)__text_start;

		/*
		 * the size which will be copyed, the 'size' is
		 * 1 : 256KB, 2 : 512KB, 3 : 768KB, 4 : 1024KB ...
		 */
		size = (__data_shadow_end - __text_start + 0x00040000) >> 18;

		/*
		 * how many blocks the 'size' is , 512 bytes per block.
		 * size * 256 *1024 / 512 = size * 2^9 = size << 9
		 */
		size = size << 9;

		/*
		 * the xboot's store position in sd card.
		 * it can be written using s3c6410-irom-sd.sh script.
		 */
		block = irom_movi_total_blkcnt - (size + 16 + 1 + 1);

		/*
		 * copy xboot to memory from movi nand.
		 */
		irom_movi_to_mem(0, block, size, mem, 0);
	}

	/* onenand */
	else if(gpn == 0x1)
	{
		/*
		 * the xboot's memory base address.
		 */
		mem = (u32_t *)__text_start;

		/*
		 * the xboot's size, the 'size' is number of block. 128KB per block.
		 */
		size = (__data_shadow_end - __text_start + 0x00020000) >> 17;

		/*
		 * copy xboot to memory from one nand.
		 */
		for(block = 0; block < size; block++)
		{
			for(page = 0; page < 64; page++)
			{
				irom_onenand_readpage_4burst(0, block, page, (u32_t *)mem);
				mem += 512;
			}
		}
	}

	/* nand(512 byte, 3-cycle) */
	/* nand(512 byte, 4-cycle) */
	else if((gpn == 0x2) || (gpn == 0x3))
	{
		/*
		 * nand flash controller initialize
		 */
		reg_write(S3C6410_NFCONF, reg_read(S3C6410_NFCONF)|0x00007770);
		reg_write(S3C6410_NFCONT, reg_read(S3C6410_NFCONT)|0x00000003);

		/*
		 * the xboot's memory base address.
		 */
		mem = (u32_t *)__text_start;

		/*
		 * the xboot's size, the 'size' is number of block. 16KB per block.
		 */
		size = (__data_shadow_end - __text_start + 0x00004000) >> 14;

		/*
		 * copy xboot to memory from nand flash.
		 */
		for(block = 0; block < size; block++)
		{
			for(page = 0; page < 32; page++)
			{
				irom_nand_readpage(block, page, (u8_t *)mem);
				mem += 128;
			}
		}
	}

	/* nand(2048 byte, 4-cycle) */
	/* nand(2048 byte, 5-cycle) */
	else if((gpn == 0x4) || (gpn == 0x5))
	{
		/*
		 * nand flash controller initialize
		 */
		reg_write(S3C6410_NFCONF, reg_read(S3C6410_NFCONF)|0x00007770);
		reg_write(S3C6410_NFCONT, reg_read(S3C6410_NFCONT)|0x00000003);

		/*
		 * the xboot's memory base address.
		 */
		mem = (u32_t *)__text_start;

		/*
		 * the xboot's size, the 'size' is number of block. 256KB per block.
		 */
		size = (__data_shadow_end - __text_start + 0x00040000) >> 18;

		/*
		 * copy xboot to memory from nand flash.
		 */
		for(block = 0; block < size; block++)
		{
			for(page = 0; page < 128; page++)
			{
				irom_nand_readpage_advanced(block, page, (u8_t *)mem);
				mem += 512;
			}
		}
	}

	/* nand(4096 byte, 5-cycle) */
	else if(gpn == 0x6)
	{
		/*
		 * nand flash controller initialize
		 */
		reg_write(S3C6410_NFCONF, reg_read(S3C6410_NFCONF)|0x00007770);
		reg_write(S3C6410_NFCONT, reg_read(S3C6410_NFCONT)|0x00000003);

		/*
		 * the xboot's memory base address.
		 */
		mem = (u32_t *)__text_start;

		/*
		 * the xboot's size, the 'size' is number of block. 512KB per block.
		 */
		size = (__data_shadow_end - __text_start + 0x00080000) >> 19;

		/*
		 * copy xboot to memory from nand flash.
		 */
		for(block = 0; block < size; block++)
		{
			for(page = 0; page < 128; page++)
			{
				irom_nand_readpage_advanced(block, page, (u8_t *)mem);
				mem += 1024;
			}
		}
	}

	/* sd-mmc(ch1) */
	else if(gpn == 0x7)
	{
		/*
		 * high speed mmc1 initialize.
		 */
		reg_write(S3C6410_HM_CONTROL4_1, reg_read(S3C6410_HM_CONTROL4_1)|(0x3<<16));

		/*
		 * the xboot's memory base address.
		 */
		mem = (u32_t *)__text_start;

		/*
		 * the size which will be copyed, the 'size' is
		 * 1 : 256KB, 2 : 512KB, 3 : 768KB, 4 : 1024KB ...
		 */
		size = (__data_shadow_end - __text_start + 0x00040000) >> 18;

		/*
		 * how many blocks the 'size' is , 512 bytes per block.
		 * size * 256 *1024 / 512 = size * 2^9 = size << 9
		 */
		size = size << 9;

		/*
		 * the xboot's store position in sd card.
		 * it can be written using s3c6410-irom-sd.sh script.
		 */
		block = irom_movi_total_blkcnt - (size + 16 + 1 + 1);

		/*
		 * copy xboot to memory from movi nand.
		 */
		irom_movi_to_mem(1, block, size, mem, 0);
	}
}
