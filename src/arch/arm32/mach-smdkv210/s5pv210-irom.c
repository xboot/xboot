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
#include <s5pv210/reg-gpio.h>
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
