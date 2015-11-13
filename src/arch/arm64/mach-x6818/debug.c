/*
 * s5p6818-logger.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <s5p6818/reg-uart.h>

extern u8_t	__text_start[];
extern u8_t __text_end[];
extern u8_t __data_shadow_start[];
extern u8_t __data_shadow_end[];
extern u8_t __data_start[];
extern u8_t __data_end[];
extern u8_t __bss_start[];
extern u8_t __bss_end[];
extern u8_t __stack_start[];
extern u8_t __stack_end[];

void debug(char c)
{
	while( !(read32(S5P6818_UART0_BASE + UART_UTRSTAT) & UART_UTRSTAT_TXFE) );
	write8(S5P6818_UART0_BASE + UART_UTXH, c);
}

void copy_shadow(void)
{
	u64_t size;

	size = __data_shadow_end - __data_shadow_start;
	memcpy(__data_start, __data_shadow_start, size);
}

void clear_bss(void)
{
	u64_t size;

	size = __bss_end - __bss_start;
	memset(__bss_start, 0, size);
}
