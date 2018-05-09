/*
 * sys-copyself.c
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

extern unsigned char __image_start;
extern unsigned char __image_end;
extern unsigned char __heap_start;
extern void return_to_fel(void);
extern void sys_uart_putc(char c);
void sys_decompress(char * src, int slen, char * dst, int dlen);
extern void sys_spi_flash_init(void);
extern void sys_spi_flash_exit(void);
extern void sys_spi_flash_read(int addr, void * buf, int count);

struct zdesc_t {
	uint8_t magic[4];
	uint8_t crc[4];
	uint8_t ssize[4];
	uint8_t dsize[4];
};

enum {
	BOOT_DEVICE_FEL		= 0,
	BOOT_DEVICE_MMC0	= 1,
	BOOT_DEVICE_MMC2	= 2,
	BOOT_DEVICE_SPI		= 3,
};

static int get_boot_device(void)
{
	u32_t * sig = (void *)0x4;
	u32_t d = sig[9] & 0xf;

	if((sig[0] == 0x4e4f4765) && (sig[1] == 0x3054422e))
	{
		if(d == 0)
			return BOOT_DEVICE_MMC0;
		else if(d == 2)
			return BOOT_DEVICE_MMC2;
		else if(d == 3)
			return BOOT_DEVICE_SPI;
	}
	return BOOT_DEVICE_FEL;
}

void sys_copyself(void)
{
	struct zdesc_t z;
	uint32_t ssize, dsize;
	int d = get_boot_device();
	void * mem, * tmp;
	u32_t size;

	if(d == BOOT_DEVICE_FEL)
	{
		sys_uart_putc('B');
		sys_uart_putc('o');
		sys_uart_putc('o');
		sys_uart_putc('t');
		sys_uart_putc(' ');
		sys_uart_putc('t');
		sys_uart_putc('o');
		sys_uart_putc(' ');
		sys_uart_putc('F');
		sys_uart_putc('E');
		sys_uart_putc('L');
		sys_uart_putc(' ');
		sys_uart_putc('m');
		sys_uart_putc('o');
		sys_uart_putc('d');
		sys_uart_putc('e');
		sys_uart_putc('\r');
		sys_uart_putc('\n');
		return_to_fel();
	}
	else if(d == BOOT_DEVICE_MMC0)
	{
		mem = (void *)&__image_start;
		size = (&__image_end - &__image_start + 512) >> 9;
	}
	else if(d == BOOT_DEVICE_MMC2)
	{
		mem = (void *)&__image_start;
		size = (&__image_end - &__image_start + 512) >> 9;
	}
	else if(d == BOOT_DEVICE_SPI)
	{
		mem = (void *)&__image_start;
		tmp = (void *)&__heap_start;
		size = &__image_end - &__image_start;

		sys_spi_flash_init();
		sys_spi_flash_read(32768, &z, sizeof(struct zdesc_t));
		sys_spi_flash_exit();
		if((z.magic[0] == 'L') && (z.magic[1] == 'Z') && (z.magic[2] == '4') && (z.magic[3] == ' '))
		{
			ssize = (z.ssize[0] << 24) | (z.ssize[1] << 16) | (z.ssize[2] << 8) | (z.ssize[3] << 0);
			dsize = (z.dsize[0] << 24) | (z.dsize[1] << 16) | (z.dsize[2] << 8) | (z.dsize[3] << 0);
			sys_spi_flash_init();
			sys_spi_flash_read(32768 + sizeof(struct zdesc_t), tmp, ssize);
			sys_spi_flash_exit();
			sys_decompress(tmp, ssize, mem, dsize);
		}
		else
		{
			sys_spi_flash_init();
			sys_spi_flash_read(0, mem, size);
			sys_spi_flash_exit();
		}
	}
}
