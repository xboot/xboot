/*
 * realview-logger.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <realview/reg-uart.h>

static void logger_uart0_init(void)
{
}

static void logger_uart0_exit(void)
{
}

static ssize_t logger_uart0_output(const char * buf, size_t count)
{
	virtual_addr_t regbase = phys_to_virt(REALVIEW_UART0_BASE);
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while(read8(regbase + 0x18) & (0x1 << 5));
		write8(regbase + 0x00, buf[i]);
	}
	return i;
}

static struct logger_t realview_logger = {
	.name	= "logger-uart0",
	.init	= logger_uart0_init,
	.exit	= logger_uart0_exit,
	.output	= logger_uart0_output,
};

static __init void realview_logger_init(void)
{
	if(register_logger(&realview_logger))
		LOG("Register logger '%s'", realview_logger.name);
	else
		LOG("Failed to register logger '%s'", realview_logger.name);
}

static __exit void realview_logger_exit(void)
{
	if(unregister_logger(&realview_logger))
		LOG("Unregister logger '%s'", realview_logger.name);
	else
		LOG("Failed to unregister logger '%s'", realview_logger.name);
}

core_initcall(realview_logger_init);
core_exitcall(realview_logger_exit);
