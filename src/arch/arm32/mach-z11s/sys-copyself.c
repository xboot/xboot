/*
 * sys-copyself.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

extern unsigned char __image_start;
extern unsigned char __image_end;
extern void return_to_fel(void);
extern void sys_uart_putc(char c);
extern void sys_spi_flash_init(void);
extern void sys_spi_flash_exit(void);
extern void sys_spi_flash_read(int addr, void * buf, int count);

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
	int d = get_boot_device();
	void * mem;
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
		size = &__image_end - &__image_start;

		sys_spi_flash_init();
		sys_spi_flash_read(0, mem, size);
		sys_spi_flash_exit();
	}
}
