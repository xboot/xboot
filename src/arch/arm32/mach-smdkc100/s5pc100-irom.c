/*
 * arch/arm/mach-smdkc100/s5pc100-irom.c
 *
 * this file used by start.s assembler code, and the linker script
 * must make sure this file is linked within the first 8kB. DO NOT
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
#include <s5pc100/reg-gpio.h>
#include <s5pc100/reg-nand.h>


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
 * define for simple nand read function.
 */
#define NAND_HW_INIT()														\
	do {																	\
		reg_write(S5PC100_GPK0CON, 0x22443322);								\
		reg_write(S5PC100_GPK2CON, 0x33333333);								\
		reg_write(S5PC100_NFCONF, reg_read(S5PC100_NFCONF) | 0x00007770);	\
		reg_write(S5PC100_NFCONT, reg_read(S5PC100_NFCONT) | 0x00000003);	\
	} while(0)

#define NAND_CONTROL_ENABLE()												\
	reg_write(S5PC100_NFCONT, reg_read(S5PC100_NFCONT) | (1 << 0))

#define NAND_CONTROL_DISABLE()												\
	reg_write(S5PC100_NFCONT, reg_read(S5PC100_NFCONT) & ~(1 << 0))

#define NAND_ENABLE_CE()													\
	reg_write(S5PC100_NFCONT, reg_read(S5PC100_NFCONT) & ~(1 << 1))

#define NAND_DISABLE_CE()													\
	reg_write(S5PC100_NFCONT, reg_read(S5PC100_NFCONT) | (1 << 1))

#define NAND_WRITE_CMD(cmd)													\
	reg_write(S5PC100_NFCMD, cmd)

#define NAND_WRITE_ADDR(addr)												\
	reg_write(S5PC100_NFADDR, addr)

#define NAND_READ_BYTE()													\
	(*((volatile u8_t *)(S5PC100_NFDATA)))

#define NAND_WAIT_READY()													\
	do { while(!(reg_read(S5PC100_NFSTAT) & (1 << 0))); } while(0)

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
 * simple nand read function
 */
static s32_t simple_nand_read(u8_t * mem, u32_t addr, u32_t size)
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
	u8_t om, nfmod;
	u32_t mem, size;

	/*
	 * read config register
	 */
	om = (u8_t)((reg_read(0xe0000004) >> 0) & 0x1f);
	nfmod = (u8_t)((reg_read(0xe0000004) >> 8) & 0x3f);

	/*
	 * the xboot's memory base address and size.
	 */
	mem = (u32_t)__text_start;
	size = (u32_t)__data_shadow_end - (u32_t)__text_start;

	/* booting from usb */
	if(nfmod & 0x20)
	{
		/* not support */
	}

	/* booting from the device selected by om[2:1] */
	else
	{
		om = (om >> 1) & 0x3;

		/* nand flash */
		if(om == 0x0)
		{
			nfmod = (nfmod >> 0) & 0x7;

			/* nfmod = 0x0, nand(512 byte, 3-cycle) */
			/* nfmod = 0x4, nand(512 byte, 4-cycle) */
			/* nfmod = 0x2, nand(2048 byte, 4-cycle) */
			/* nfmod = 0x6, nand(2048 byte, 5-cycle) */
			if( (nfmod == 0x0) || (nfmod == 0x4) || (nfmod == 0x2) || (nfmod == 0x6) )
			{
				simple_nand_read((u8_t *)mem, 0x00000000, size);
			}

			/* nfmod = 0x3, nand(4096 byte, 4-cycle) */
			/* nfmod = 0x7, nand(4096 byte, 5-cycle) */
			else if( (nfmod == 0x3) || (nfmod == 0x7) )
			{
				/* not support */
			}

			/* not support */
			else
			{
				return;
			}
		}

		/* onenand */
		else if(om == 0x1)
		{
			/* not support */
		}

		/* sd/mmc */
		else if(om == 0x2)
		{
			/* not support */
		}

		/* not support */
		else
		{
			return;
		}
	}
}
