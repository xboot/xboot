/*
 * s5p4418-logger.c
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
#include <s5p4418/reg-gpio.h>
#include <s5p4418/reg-uart.h>

static void logger_uart0_init(void)
{
}

static void logger_uart0_exit(void)
{
}

static ssize_t logger_uart0_output(const char * buf, size_t count)
{
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( (read8(phys_to_virt(S5P4418_UART0_BASE + UART_FR)) & UART_FR_TXFF) );
		write8(phys_to_virt(S5P4418_UART0_BASE + UART_DATA), buf[i]);
	}
	return i;
}

static struct logger_t s5p4418_logger = {
	.name	= "logger-uart0",
	.init	= logger_uart0_init,
	.exit	= logger_uart0_exit,
	.output	= logger_uart0_output,
};

static __init void s5p4418_logger_init(void)
{
	if(register_logger(&s5p4418_logger))
		LOG("Register logger '%s'", s5p4418_logger.name);
	else
		LOG("Failed to register logger '%s'", s5p4418_logger.name);
}

static __exit void s5p4418_logger_exit(void)
{
	if(unregister_logger(&s5p4418_logger))
		LOG("Unregister logger '%s'", s5p4418_logger.name);
	else
		LOG("Failed to unregister logger '%s'", s5p4418_logger.name);
}

pure_initcall(s5p4418_logger_init);
pure_exitcall(s5p4418_logger_exit);
