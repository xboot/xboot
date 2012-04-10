/*
 * s5pv210-irom.c
 *
 * this file used by start.s assembler code, and the linker script
 * must make sure this file is linked within the first 16kB. DO NOT
 * use any .bss segment and .data segment, just use irom's small
 * stack. it's just a small c routline.
 *
 * NOTE:
 *     please do not use 'switch, case' instead of 'if, else if',
 *     because switch statement may be compiled as a jump table,
 *     which belong to .data segment, if it has many cases.
 *
 *     to make sure this file is linked within the first 8KB, please
 *     look at the file of xboot.map.
 *
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-nand.h>
#include <s5pv210/reg-others.h>

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
 * global block size.
 */
#define irom_global_block_size								\
		(*((volatile u32_t *)(0xd0037480)))

/*
 * global sdhc information bit.
 *
 * [31:16] RCA address
 * [2] sd card.
 * [1] mmc card.
 * [0] high capacity enable.
 */
#define irom_global_sdhc_info_bit							\
		(*((volatile u32_t *)(0xd0037484)))

/*
 * sdmmc base, current boot channel.
 */
#define irom_v210_sdmmc_base								\
		(*((volatile u32_t *)(0xd0037488)))


/*
 * this function copies a block of page to destination memory (8-bit ecc only)
 * 2048, 4096 page 8bits-bus nand Only.
 *
 * @param u32_t block : source block address number to copy.
 * @param u32_t page  : source page address number to copy.
 * @param u8_t * mem  : target memory pointer.
 * @return s32_t - success or failure.
*/
#define	irom_nf8_readpage_adv(block, page, mem)				\
		(((s32_t(*)(u32_t, u32_t, u8_t *))(*((u32_t *)(0xd0037f90))))(block, page, mem))

/*
 * this function copies a block of page to destination memory (4-bit ecc only)
 * 2048 page size, 5 cycle address, 16bits-bus nand Only
 *
 * @param u32_t block : source block address number to copy.
 * @param u32_t page  : source page address number to copy.
 * @param u8_t * mem  : target memory pointer.
 * @return s32_t - success or failure.
*/
#define	irom_nf16_readpage_adv(block, page, mem)			\
		(((s32_t(*)(u32_t, u32_t, u8_t *))(*((u32_t *)(0xd0037f94))))(block, page, mem))

/*
 * this function copies SD/MMC card data to memory.
 * always use EPLL source clock. this function works at 20Mhz.
 *
 * @param ch 	 : HSMMC controller channel number
 * @param sector : source card(SD/MMC) address (it must block address).
 * @param count  : number of blocks to copy.
 * @param mem    : memory to copy to.
 * @param init 	 : reinitialize or not.
 * @return bool(u8_t) - success or failure.
 */
#define irom_sdmmc_to_mem(ch, sector, count, mem, init)		\
		(((u8_t(*)(s32_t, u32_t, u16_t, u32_t *, s32_t))(*((u32_t *)(0xd0037f98))))(ch, sector, count, mem, init))

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
 * define for simple nand read function.
 */
#define NAND_HW_INIT()																		\
	do {																					\
		reg_write(S5PV210_MP0_3CON, 0x22222222);											\
		reg_write(S5PV210_MP0_6CON, 0x22222222);											\
		reg_write(S5PV210_NFCONF, (reg_read(S5PV210_NFCONF) & ~(0x0000fff0)) | 0x00007770);	\
		reg_write(S5PV210_NFCONT, (reg_read(S5PV210_NFCONT) & ~(0x00000003)) | 0x00000003);	\
	} while(0)

#define NAND_CONTROL_ENABLE()												\
	reg_write(S5PV210_NFCONT, reg_read(S5PV210_NFCONT) | (1 << 0))

#define NAND_CONTROL_DISABLE()												\
	reg_write(S5PV210_NFCONT, reg_read(S5PV210_NFCONT) & ~(1 << 0))

#define NAND_ENABLE_CE()													\
	reg_write(S5PV210_NFCONT, reg_read(S5PV210_NFCONT) & ~(1 << 1))

#define NAND_DISABLE_CE()													\
	reg_write(S5PV210_NFCONT, reg_read(S5PV210_NFCONT) | (1 << 1))

#define NAND_WRITE_CMD(cmd)													\
	reg_write(S5PV210_NFCMD, cmd)

#define NAND_WRITE_ADDR(addr)												\
	reg_write(S5PV210_NFADDR, addr)

#define NAND_READ_BYTE()													\
	(*((volatile u8_t *)(S5PV210_NFDATA)))

#define NAND_WAIT_READY()													\
	do { while(!(reg_read(S5PV210_NFSTAT) & (1 << 0))); } while(0)

s32_t simple_nand_read(u8_t * mem, u32_t addr, u32_t size)
{
	u8_t id;
	u32_t i, page, page_size;

	/* nand hardware initial */
	NAND_HW_INIT();

	/* enable nand controller */
	NAND_CONTROL_ENABLE();

	/* nand chip enable */
	NAND_ENABLE_CE();

	/* write read id command */
	NAND_WRITE_CMD(0x90);

	/* write address 0x0 */
	NAND_WRITE_ADDR(0x0);

	/* read id */
	id = NAND_READ_BYTE();
	id = NAND_READ_BYTE();

	/* nand chip disable */
	NAND_DISABLE_CE();

	/* page size and current page */
	if(id > 0x80)
	{
		page_size = 2048;
		page = addr >> 11;
	}
	else
	{
		page_size = 512;
		page = addr >> 9;
	}

	while(size > 0)
	{
		/* nand chip enable */
		NAND_ENABLE_CE();

		/* write read0 command */
		NAND_WRITE_CMD(0x00);

		/* write address 0x0 */
		NAND_WRITE_ADDR(0x0);

		/* the large page */
		if(id > 0x80)
			NAND_WRITE_ADDR(0x0);

		/* write address for page */
		NAND_WRITE_ADDR((page >> 0) & 0xff);
		NAND_WRITE_ADDR((page >> 8) & 0xff);
		NAND_WRITE_ADDR((page >> 16) & 0xff);

		/* the large page */
		if(id > 0x80)
			NAND_WRITE_CMD(0x30);

		/* wait for ready */
		NAND_WAIT_READY();

		for(i = 0; (i < size) && (i < page_size); i++)
		{
			*mem++ = NAND_READ_BYTE();
		}

		page++;
		size -= i;

		/* nand chip disable */
		NAND_DISABLE_CE();
	}

	return 0;
}

/*
 * only support irom booting.
 */
void irom_copyself(void)
{
	u8_t om;
	u32_t * mem;
	u32_t size;

	/*
	 * read om register, om[4..1]
	 */
	om = (u8_t)((reg_read(S5PV210_OMR) >> 1) & 0x0f);

	/* essd */
	if(om == 0x0)
	{

	}

	/* nand 2KB, 5-cycle, 8-bit ecc */
	else if(om == 0x1)
	{
		/*
		 * the xboot's memory base address.
		 */
		mem = (u32_t *)__text_start;

		/*
		 * the xboot's size, the 'size' is number of block. 256KB per block.
		 */
		size = (__data_shadow_end - __text_start + 0x00040000) >> 18;

		simple_nand_read((u8_t *)mem, 0x00000000, size);
	}

	/* nand 4KB, 5-cycle, 8-bit ecc */
	else if(om == 0x2)
	{

	}

	/* nand 4KB, 5-cycle, 16-bit ecc */
	else if(om == 0x3)
	{

	}

	/* onenand mux */
	else if(om == 0x4)
	{

	}

	/* onenand demux */
	else if(om == 0x5)
	{

	}

	/* sd / mmc */
	else if(om == 0x6)
	{
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
		 * copy xboot to memory from sd/mmc card.
		 */
		if(irom_v210_sdmmc_base == 0xeb000000)
		{
			irom_sdmmc_to_mem(0, 1, size, mem, 0);
		}
		else if(irom_v210_sdmmc_base == 0xeb200000)
		{
			irom_sdmmc_to_mem(2, 1, size, mem, 0);
		}
		else
		{
			return;
		}
	}

	/* emmc, 4-bit */
	else if(om == 0x7)
	{

	}

	/* reserved */
	else if(om == 0x8)
	{

	}

	/* nand 2KB, 4-cycle, 8-bit ecc */
	else if(om == 0x9)
	{

	}

	/* nor flash */
	else if(om == 0xa)
	{

	}

	/* emmc, 8-bit */
	else if(om == 0xa)
	{

	}

	/* not support */
	else
	{
		return;
	}
}
