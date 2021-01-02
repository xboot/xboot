/*
 * sys-uart.c
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

enum {
	UART_TXFIFO	= 0x00,
	UART_RXFIFO	= 0x04,
	UART_TXCTRL	= 0x08,
	UART_RXCTRL = 0x0c,
	UART_IE		= 0x10,
	UART_IP 	= 0x14,
	UART_DIV 	= 0x18,
};

void sys_uart_init(void)
{
	virtual_addr_t addr = 0x38000000;

	write32(addr + UART_TXCTRL, (0 << 16) | (1 << 0));
	write32(addr + UART_RXCTRL, (0 << 16) | (1 << 0));
	write32(addr + UART_IP, (1 << 1) | (1 << 0));
	write32(addr + UART_IE, (0 << 1) | (0 << 0));
	write32(addr + UART_DIV, 403000000 / 115200 - 1);
	write32(addr + UART_TXCTRL, read32(addr + UART_TXCTRL) & ~(1 << 1));
}

void sys_uart_putc(char c)
{
	virtual_addr_t addr = 0x38000000;

	while(read32(addr + UART_TXFIFO) & (1 << 31));
	write32(addr + UART_TXFIFO, c);
}
